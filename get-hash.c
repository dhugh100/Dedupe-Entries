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
// along with this program.  If not, see <https:www.gnu.org/licenses/>.

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
// - Return 0 if mem allocation error, otherwise return 1

int getsha256 (DupItem *item, user_data *udp)
{

	unsigned char ub_hash[EVP_MAX_MD_SIZE] = { 0x00 }; // Will hold hash
	char c_hash[STR_HASH] = { 0x00 }; // Will hold hash char representation in hex

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
	if (!read_buff) {
		g_object_set(item, "result", "Error: read buffer allocation failure", NULL);
		g_object_unref(file);
		g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
		g_object_unref(in);
		return 0;
	}

	// Get the digest type and initial context
	const EVP_MD *md = EVP_get_digestbyname("SHA256"); //Setup digest type
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new(); // Create new context
	EVP_DigestInit_ex(mdctx, md, NULL);

	// Initial read seeding the while
	int read = 0;
	read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);
	if (read == -1) {
		g_object_set(item, "result", "Error: seed read failure", "hash", "", NULL);
		g_object_unref(file);
		g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
		g_object_unref(in);
		return 1;
	}
	float all_read = read;
	char percent_read[7]; // 3 digit number plus '.' plus 2 digit fraction  plus null

	// Loop to read into buffer, update hash and progress bar
	while (read > 0) {

		if (udp->cancel_request == TRUE) {
			g_object_set(item, "result", "Error: Hash Canceled", "hash", "", NULL);
			g_object_unref(file);
			EVP_MD_CTX_free(mdctx);
			g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
			g_object_unref(in);
			g_free(read_buff);
			return 0;

		}

		// Update hash
		if (!EVP_DigestUpdate(mdctx, read_buff, read)) {
			g_object_set(item, "result", "Error: Digest update issue", "hash", "", NULL);
			g_object_unref(file);
			EVP_MD_CTX_free(mdctx);
			g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
			g_object_unref(in);
			g_free(read_buff);
			return 1;
		}

		// Update progress bar
		snprintf(percent_read, sizeof(percent_read), "%3.2f", (float)all_read / strtol(item->file_size, NULL, 10) * 100.0);
		do_progress_bar((GtkProgressBar *) udp->progress_bar, percent_read, basename((char *)item->name)); // Show progress bar

		// Fill read buffer from file
		read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);
		all_read += read;

	} // End read while

	// Get the hash into the hash buffer
	uint32_t md_len = 0;
	if (!EVP_DigestFinal_ex(mdctx, ub_hash, &md_len)) {
		g_object_set(item, "result", "Error: Digest final issue", "hash", "", NULL);
		g_object_unref(file);
		g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
		g_object_unref(in);
		return 1;
	}

	// Make unsigned byte hash into str
	char *chp = c_hash;
	for (int i = 0; i < md_len; i++) {
		snprintf((char *)chp, sizeof(c_hash), "%02x", ub_hash[i]); // Convert to hex and store
		chp += 2;
	}

	// Set the hash into the item
	g_object_set(item, "hash", c_hash, NULL);

	// Cleanup
	g_object_unref(file);
	EVP_MD_CTX_free(mdctx);
	g_input_stream_close(G_INPUT_STREAM(in), NULL, NULL);
	g_object_unref(in);
	g_free(read_buff);
	return 1;
}
