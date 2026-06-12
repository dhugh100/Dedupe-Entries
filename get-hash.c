// This file, get_hash.c, is a part of the Dedupe Entries program.
// 
// Copyright (C) 2025  David Hugh
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "main.h"
#include "lib.h"
#include "get-hash.h"

// Setup the progress bar
// - Show the percentage of the file processed basename being worked

void do_progress_bar (GtkProgressBar *progress_bar, char *percent, const char *name)
{
	char progress[128] = { 0x00 }; // Buffer for progress bar text
	snprintf(progress, sizeof(progress) - 1, "Processed %s %% of file %s", percent, name);
	gtk_progress_bar_set_text(progress_bar, progress);
	do_pending(); // Let progress bar update
}

// Get the sha256 hash of a file
// - Store into the item hash field as an ascii string of hex digits
// - Update the progress
// - Errors are recorded in the item result and still return 1
// - Return 0 only on a cancel request

int getsha256 (DupItem *item, user_data *udp)
{
	unsigned char ub_hash[EVP_MAX_MD_SIZE] = { 0x00 }; // Will hold hash
	char c_hash[STR_HASH] = { 0x00 }; // Will hold hash char representation in hex
	int rcode = 1; // 1 keep working, 0 stop

	// Setup file to read
	GFile *file = g_file_new_for_path(item->name);

	// Setup input stream
	GFileInputStream *in = g_file_read(file, NULL, NULL);
	if (!in) {
		g_object_set(item, "result", "Error: file read failure", "hash", "", NULL);
		g_object_unref(file);
		return 1;
	}

	// Get buffer to read from file
	unsigned char *read_buff = g_malloc0(READ_BUFF);

	// Get the digest type and initial context
	const EVP_MD *md = EVP_get_digestbyname("SHA256"); //Setup digest type
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new(); // Create new context
	EVP_DigestInit_ex(mdctx, md, NULL);

	// Initial read seeding the while
	int read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);
	if (read == -1) {
		g_object_set(item, "result", "Error: seed read failure", "hash", "", NULL);
		goto cleanup;
	}

	// Parse the file size once for progress computation
	uint64_t fsize = strtoull(item->file_size, NULL, 10);
	uint64_t all_read = read;
	int last_pct = -1; // Last percent shown, only update bar on change
	char percent_read[7]; // 3 digit number plus null

	// Loop to read into buffer, update hash and progress bar
	while (read > 0) {

		if (udp->cancel_request == TRUE) {
			g_object_set(item, "result", "Error: Hash Canceled", "hash", "", NULL);
			rcode = 0;
			goto cleanup;
		}

		// Update hash
		if (!EVP_DigestUpdate(mdctx, read_buff, read)) {
			g_object_set(item, "result", "Error: Digest update issue", "hash", "", NULL);
			goto cleanup;
		}

		// Update progress bar only when the displayed percent changes
		int pct = fsize ? (int)(all_read * 100 / fsize) : 100;
		if (pct != last_pct) {
			last_pct = pct;
			snprintf(percent_read, sizeof(percent_read), "%d", pct);
			do_progress_bar((GtkProgressBar *) udp->progress_bar, percent_read, basename((char *)item->name)); // Show progress bar
		}
		else
			do_pending(); // Keep the cancel button responsive

		// Fill read buffer from file
		read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);
		all_read += read;

	} // End read while

	// Get the hash into the hash buffer
	uint32_t md_len = 0;
	if (!EVP_DigestFinal_ex(mdctx, ub_hash, &md_len)) {
		g_object_set(item, "result", "Error: Digest final issue", "hash", "", NULL);
		goto cleanup;
	}

	// Make unsigned byte hash into str
	for (uint32_t i = 0; i < md_len; i++) {
		snprintf(c_hash + i * 2, sizeof(c_hash) - i * 2, "%02x", ub_hash[i]);
	}

	// Set the hash into the item
	g_object_set(item, "hash", c_hash, NULL);

 cleanup:
	g_object_unref(file);
	EVP_MD_CTX_free(mdctx);
	g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
	g_object_unref(in);
	g_free(read_buff);
	return rcode;
}

// Comparison function to sort the list store by numeric file size
// - file_size is a decimal string, strcmp would mis-order different digit counts
// - Directories, errors, and empties parse to 0 and cluster together

static int cmp_size (const void *a, const void *b, void *data)
{
	const DupItem *a_item = a;
	const DupItem *b_item = b;
	uint64_t a_size = strtoull(a_item->file_size, NULL, 10);
	uint64_t b_size = strtoull(b_item->file_size, NULL, 10);
	return (a_size > b_size) - (a_size < b_size);
}

// Hash only files that could have a duplicate
// - Two files can only be duplicates if they are the same size
// - A file with a unique size is marked Unique without reading it
// - Items with a result already set (directory, error, empty) are skipped
// - Return 0 to stop on a cancel request, otherwise return 1

int hash_candidates (user_data *udp)
{
	// Sort by size so candidate files of equal size are adjacent
	g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_size, NULL);

	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(udp->list_store));
	uint32_t i = 0;

	while (i < cnt) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i);

		// Skip items with a result already set
		if (item->result != 0x00) {
			g_object_unref(item);
			i++;
			continue;
		}

		// Find the run of same-size candidate files starting at i
		uint64_t size = strtoull(item->file_size, NULL, 10);
		g_object_unref(item);
		uint32_t run_end = i + 1; // Run is items [i, run_end)
		while (run_end < cnt) {
			DupItem *next = g_list_model_get_item(G_LIST_MODEL(udp->list_store), run_end);
			int same = (next->result == 0x00) && (strtoull(next->file_size, NULL, 10) == size);
			g_object_unref(next);
			if (!same) break;
			run_end++;
		}

		// A unique size means no duplicate is possible, no need to hash
		if (run_end - i == 1) {
			item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i);
			g_object_set(item, "result", STR_UNI, NULL);
			g_object_unref(item);
			i++;
			continue;
		}

		// Two or more files share this size, hash each one
		for (uint32_t j = i; j < run_end; j++) {
			item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), j);
			int rcode = getsha256(item, udp);
			g_object_unref(item);
			if (rcode == 0) return 0; // Stop on cancel request
		}
		i = run_end;
	}
	return 1;
}
