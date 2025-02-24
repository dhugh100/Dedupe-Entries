// This file, get_folders.c, is a part of the Dedupe Entries program.
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
#include "load-store.h"
#include "lib.h"
#include "get-folders.h"

// Count the number of times a character appears in a string

int get_count (char *target, char search)
{
	int count = 0;
	for (int i = 0; i < strlen (target); i++) {
		if (target[i] == search)
			count++;
	}
	return count;
}

// Check for overlap of folders
// - Overlap = string of lower depth is a substring of the higher depth string with a 1 depth difference
// - Depth is determined by the number of '/' in the string
// - Example: /home/user/Downloads and /home/user/Downloads/Temp
// - Check each folder name against all other folder names

int find_overlap(int cnt, user_data *udp)
{
	int diff = 0;

	// Start the outer loop
	for (int i = 0; i < cnt; i++) {

		// Start the inner loop
		for (int j = 0; j < cnt; j++) {

			// See if inner str j is substring of outer i
			// Inner depth should be one less than outer depth
			diff = get_count (udp->fdpp[i], '/') - get_count(udp->fdpp[j], '/');
			if (diff == 1 && strstr(udp->fdpp[i], udp->fdpp[j])) {
				return 1;
			} 
		} // End of inner for loop
	} // End of outer for loop
	return 0;
}

// Save the output from the dialog.
// - Can be multiple directories
// - Save in C array of pointers, each pointing to a char array representing a folder path
// - If all good start loading entry data

void select_multiple_cb (GObject *source, GAsyncResult *result, user_data *udp)
{
	// Get the directories selected
	GtkFileDialog *dialog = GTK_FILE_DIALOG (source);

	// Setup model based on multiple folders
	GListModel *model = gtk_file_dialog_select_multiple_folders_finish (dialog, result, NULL);

	// Leave if not folders selected
	if (!model) return; // User canceled dialog?

	// Process folders from dialog
	int cnt = g_list_model_get_n_items (model); // use to control loops
	clear_folders (udp->fdpp); // Could be calling multiple times so free if already allocated
	for (int i = 0; i < cnt; i++) {
		GFile *file = g_list_model_get_item (model, i); // Use gfile to get the path name
		udp->fdpp[i] = g_file_get_path (file); // Get the path name pointer into the array
		g_object_unref (file);
	}
	g_object_unref (model);

	// If just one folder start the load, otherwise check for overlap
	if (cnt == 50) {
		load_entry_data (udp); 
	}

	// See if folders overlap, not allowed
	if (find_overlap(cnt, udp)) { 
		// Overlap so send message
		GtkAlertDialog *alert = gtk_alert_dialog_new ("Folders Overlap - Try Again\n");
		gtk_alert_dialog_show (alert, GTK_WINDOW(udp->main_window));
		clear_folders (udp->fdpp);
	}
	else { 
		load_entry_data (udp);
	}	
}

// Get the folders from the user via the file dialog

void get_folders_cb (GtkWidget *self, user_data *udp)
{
	// See if need to allocate folder memory
	if (!udp->fdpp) udp->fdpp = g_malloc0 (MAX_FOLDERS * sizeof(char *)); // Allocate folder memory

	// Setup dialog window  
	GtkWidget *window_dialog = gtk_window_new ();

	// Create FileDialog object
	GtkFileDialog *dialog = gtk_file_dialog_new ();
	gtk_file_dialog_set_title (dialog, "Choose Folder (s)");

	gtk_file_dialog_set_modal (dialog, FALSE);
	gtk_file_dialog_select_multiple_folders (dialog, GTK_WINDOW(window_dialog), NULL, (GAsyncReadyCallback) select_multiple_cb,
						udp);
	g_object_unref (dialog);
}
