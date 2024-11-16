// Get the user to select folders and store the data in the folder structure

#include "main.h"
#include "load_entry_data.h"
#include "get_folders.h" 

void clear_folders (char *fopp[MAX_FOLDERS]) 
{
	// Clear the folder data
	for (int i = 0; i < MAX_FOLDERS; i++) {
		if (fopp[i]) {
			g_free (fopp[i]);
			fopp[i] = NULL;
		}
		else break;
	}
}

// Count the number of times a character appears in a string

int get_count(char *target, char search) 
{
	int count = 0;
	for (int i = 0; i < strlen(target); i++) {
		if (target[i] == search) count++; 
	}
	return count;
}

// Save the output from the dialog.
// - Can be multiple directories
// - Save in C array of pointers, each point ing to a char array representing a folder path

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
	clear_folders(udp->fdpp); // Could be calling multiple times so free if already allocated
	for (int i = 0; i < cnt; i++) {
		GFile *file = g_list_model_get_item (model, i); // Use gfile to get the path name
		udp->fdpp[i] = g_file_get_path (file); // Get the path name pointer into the array
		g_object_unref (file); 
	}

	// If just one folder start the load, otherwise check for overlap
	if (cnt == 1) {
		load_entry_data(udp); // Load the data into the entry fields
		return;	      
	}


	// Check for overlap of folders
	// - Overlap = string of lower depth is a substring of the higher depth string
	// - Depth is determined by the number of '/' in the string
	// - Example: /home/user/Downloads and /home/user/Downloads/Temp
	// - Check each folder name against all other folder names
	
	// Start the outer loop
	for (int i = 0; i < cnt; i++) {

		// Start the inner loop
		for (int j = 0; j < cnt; j++) {

			// See if inner str overlaps with outer
			if (get_count(udp->fdpp[j],'/') < get_count(udp->fdpp[i],'/') &&
			    strstr(udp->fdpp[i], udp->fdpp[j])) {

				// Overlap so send message
				GtkAlertDialog *alert = gtk_alert_dialog_new ("Folders Overlap - Try Again\n\n1: %s\n2: %s", udp->fdpp[i], udp->fdpp[j]);
				gtk_alert_dialog_show (alert, GTK_WINDOW (udp->main_window));

				// Clean up
				g_object_unref(model);
				clear_folders(udp->fdpp);
				return;

			} // End of issue check
		} // End of inner for loop
	} // End of outer for loop
	
	load_entry_data(udp); // Load the data into the entry fields

}

// Get the folders from the user via the file dialog

void get_folders_cb (GtkWidget *self, user_data *udp) 
{
	// See if need to allocate folder memory
	if (!udp->fdpp) udp->fdpp = g_malloc0(MAX_FOLDERS * sizeof(char *)); // Allocate folder memory

	// Setup dialog window 	
	GtkWidget *window_dialog = gtk_window_new ();

	// Create FileDialog object
	GtkFileDialog *dialog = gtk_file_dialog_new ();
	gtk_file_dialog_set_title (dialog, "Choose Folder (s)");

	gtk_file_dialog_set_modal (dialog, FALSE);
	gtk_file_dialog_select_multiple_folders (dialog, GTK_WINDOW (window_dialog), 
						NULL, (GAsyncReadyCallback) select_multiple_cb, 
						udp);
}
