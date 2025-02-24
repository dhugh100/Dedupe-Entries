// This file, traverse.c, is a part of the Dedupe Entries program.
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
#include "traverse.h"

// Re-entrant traverse and data store of entry information
// - Check for cancel request and max entries in loops, return if hit
// - Return of 1 means continue working, 0 means stop

int traverse (char *dir_str, user_data *udp)
{
	int rcode = 1; // Return code, 1 is good, 0 is error
	char full_name[STR_PATH] = { 0x00 }; // Create full names from passed dir and dir entry
	char buff[100] = { 0x00 }; // Buffer for conversions

	// Attribute and time structures
	struct stat attr; // Structue for the stat command
	struct tm *tinfo; // Structure pointer for time

	// Directory structure and pointers
	struct dirent *entry = NULL; // Directory entry ptr
	DIR *dir; // Directory ptr

	// Check max entries before storing folder name
	if (g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)) >= MAX_ENTRIES) {
		return 0;
	}

	// Setup directory item
	DupItem *item =
	    g_object_new(DUP_TYPE_ITEM, "name", dir_str, "hash", "", "file_size", "", "modified", "", NULL);

	// Open the directory in preparation for loop
	dir = opendir(dir_str);
	if (!dir) {
		// Store the result on bad open
		g_object_set(item, "result", "Error: can't open directory", NULL);
		g_list_store_append(udp->list_store, item);
		g_object_unref(item);
		return 1;
	}

	// Store the result on good open
	g_object_set(item, "result", STR_DIR, NULL);
	g_list_store_append(udp->list_store, item);
	g_object_unref(item);

	// Loop through the directory and store the files - recurse when hit a directory
	while ((entry = readdir(dir))) {

		// Skip . and .. entries, nothing to store
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue; 

		// Don't want to add a forward slash if root was the passed directory
		if (dir_str[strlen(dir_str) - 1] != '/') sprintf(full_name, "%s/%s", dir_str, entry->d_name);
		else sprintf(full_name, "%s%s", dir_str, entry->d_name);

		// If the entry is a directory, recurse
		if (entry->d_type == DT_DIR) {
			// If return from lower level is 0, then pass 0 back up
			if (!traverse(full_name, udp)) {
				if (dir) closedir(dir);
			       	return 0;
			}	
		}

		// Only working on POSIX regular files and directories, ignore all others
		if (entry->d_type != DT_REG) continue;

		// Check for cancel request and max entries hit before storing file name
		if (udp->cancel_request == TRUE) { 
			if (dir) closedir(dir);
		       	return 0;
		}

		if (g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)) >= MAX_ENTRIES) {
			g_idle_add((GSourceFunc) cancel_clean_up, udp); // treat as if cancel requested
			if (dir) closedir(dir);
			GtkAlertDialog *alert =
			    gtk_alert_dialog_new("Max entries of %d hit - Pick folders with fewer entries", MAX_ENTRIES);
			gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
			return 0;
		}

		// Create object to store
		item = g_object_new(DUP_TYPE_ITEM, "name", full_name, NULL);

		// Get stat structure initialed with current entry
		if (stat(full_name, &attr) == -1) { // -1 is error on stat
			g_object_set(item, "result", "Error: stat failed", "hash", " ", "file_size", "", "modified", "", NULL);
			g_list_store_append(udp->list_store, item);
			g_object_unref(item);
			continue;
		}

		// Get file size in Bytes
		sprintf(buff, "%lu", attr.st_size);
		g_object_set(item, "file_size", buff, NULL);

		// Convert to local time, and then to a string
		tinfo = localtime(&attr.st_mtim.tv_sec);
		strftime(buff, sizeof(buff), "%F %H:%M:%S", tinfo);
		g_object_set(item, "modified", buff, NULL);

		// Get the hash into item and record any error in result
		if (attr.st_size != 0) {
			rcode = getsha256(item, udp); // Will store hex digits representing hash in item
			g_list_store_append(udp->list_store, item);
			g_object_unref(item);
		}
		else {
			g_object_set(item, "result", STR_EMP, "hash", "", NULL);
			g_list_store_append(udp->list_store, item);
			g_object_unref(item);
		}
		if (rcode == 0) {
			if (dir) closedir(dir);
			return 0; // Stop if error in hash
		} 
	} // End Dir read while   

	if (dir) closedir(dir);
	return 1;
}
