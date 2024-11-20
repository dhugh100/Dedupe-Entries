#include "main.h"
#include "get_folders.h"
#include "show_columns.h"
#include "get_hash.h"
#include "traverse.h"
#include "get_results.h"
#include "sort_columns.h"
#include "load_entry_data.h"

// Cancel clean up
// - Remove any entries from list store (could be partial and misleading)
// - Clear the folders 

void clean_up(user_data *udp)
{
	// Reset cancel request
	udp->cancel_request = FALSE; 

	// Trash any child window
	gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);

	// Clean up any data
	if (udp->list_store) {
		if (g_list_model_get_n_items (G_LIST_MODEL(udp->list_store))) 
			g_list_store_remove_all (udp->list_store); // Remove all entries from list store
	}	

	// Clear the folder pointers, but leave array of pointers
	if (udp->fdpp) 
		clear_folders(udp->fdpp); 
}

// Cancel button hit, set cancel to true and do cleanup when cycles available

void cancel_cb (GtkWidget *self, user_data *udp)
{
	udp->cancel_request = TRUE;
	g_idle_add ((GSourceFunc) clean_up, udp);
}

gboolean good_group_number(const char *str)
{
	errno = 0;
	uint32_t result = strtol(str, NULL, 10);
	if (errno || !result) return FALSE;
	else return TRUE;
}

// Compare function for find in result column
// Could be Error, Empty, Directory, Group, Unique

gboolean find_result (DupItem *item1, DupItem *item2)
{
	if (!strcmp(item1->result, item2->result)) return TRUE;
	else return FALSE;
}

// Run through list store to weed out the group items
// - Invalid strtol signals shift from group to something else
// - Must be sorted by result column to use

void splice_group(GListStore *list_store)
{
	uint32_t start_position = 0, end_position = 0, i = 0, j = 0;

	uint32_t cnt = g_list_model_get_n_items (G_LIST_MODEL(list_store));

	for (; i < cnt; i++) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), i);
		if (good_group_number(item->result)) {
			start_position = i;
			for (j = i + 1; j < cnt; j++) {
				DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), j);
				if (!good_group_number(item->result)) {
					end_position = j;
					break;
				}
				g_object_unref(item);
			}
			break;
		}
		g_object_unref(item);
	}

	// Account for edge condition where end_position is not set
	if (end_position == 0) end_position = i; // One more than end of list store, need to setup substraction for splice
	if (j == 0) return; // Never found a good group number

	// Splice out the entries
	g_list_store_splice(list_store, start_position, end_position - start_position, NULL, 0);
}


// Run though list store to weed out the empty, directory, and unique items if not directed to be included in options
// - Can't implement completely during load process as have to load item before determining unique/dup results
// - App default is to include all files, cutting down on the need to work this function
// - Iterate with remove on the fly problematic (cnt is less, but what is position ?)
// - Must be sorted by result column to use 

void find_and_splice(GListStore *list_store, char *str)
{
	uint32_t start_position = 0, end_position = 0, i = 0;
	gboolean result = FALSE;

	// Get count of items in list store
	uint32_t cnt = g_list_model_get_n_items (G_LIST_MODEL(list_store));

	// Get the first item in the list store
	DupItem *item =g_object_new (DUP_TYPE_ITEM, "result", str, NULL); // Have to pass item to find   
	result = g_list_store_find_with_equal_func(list_store, item, (GEqualFunc) find_result, &start_position);
	
	// If found the start, now need to find the end
	if (result) {
		end_position = 0;
		// Loop until find a different result
		for (i = start_position; i < cnt; i++) {
			DupItem *next = g_list_model_get_item(G_LIST_MODEL(list_store), i);
			if (strcmp(str, next->result)) { // Doesn't match so has changed
				end_position = i;
				g_object_unref(next);
				break;
			}
			g_object_unref(next);
		}
		// Account for edge condition  where end_position is not set
		if (end_position == 0) end_position = i; // One more than end of list store, need to setup substraction for splice

		// Splice out the entries
		g_list_store_splice(list_store, start_position, end_position - start_position, NULL, 0);
	}	
	g_object_unref(item);
}

// Sort compare
// - Default is result primary, name secondary, both ascending
int cmp_a(const void *a, const void *b, user_data *udp)
{
        DupItem *item1 = (DupItem *)a;
        DupItem *item2 = (DupItem *)b;
        if (!strcmp(item1->result,item2->result))
		return(strcmp(item1->name, item2->name));
	else 
		return(strcmp(item1->result, item2->result));			
}

// Drive getting and showing entry duplicates and data
// - Create the store
// - Setup the progress box, progress bar, and cancel button
// - Launch a traverse of each folder and store the entry data in the list store
// - Using the entry data, determine duplicates and other values for the result column
// - Parse out any unwanted result types
// - Launch and show the data in the columns
// - Can re-enter multiple times (mutliple gets)

void load_entry_data(user_data *udp)
{
	// Create a list store if not there yet
	if (!udp->list_store) {
		GListStore *list_store = g_list_store_new (G_TYPE_OBJECT);
		udp->list_store = list_store; // Save pointer to list store
	}

	// If have entries in the list store, time to remove since working new directories
	if (g_list_model_get_n_items (G_LIST_MODEL(udp->list_store))) 
		g_list_store_remove_all (udp->list_store);

        // Remove and collect any existing child
        gtk_window_set_child(GTK_WINDOW (udp->main_window), NULL);

	// Create a box to hold the progress bar
	GtkWidget *progress_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_window_set_child (GTK_WINDOW (udp->main_window), progress_box);

	// Create a progress bar
	GtkWidget *progress_bar = gtk_progress_bar_new ();
	udp->progress_bar = progress_bar; // Save pointer to progress bar
	gtk_progress_bar_set_show_text((GtkProgressBar *)progress_bar, TRUE);
	gtk_box_append (GTK_BOX(progress_box), progress_bar);

	// Create the cancel button
	GtkWidget *cancel_button = gtk_button_new_with_label ("Cancel");
	udp->cancel_button = cancel_button;
	gtk_box_append (GTK_BOX (progress_box), cancel_button);
	gtk_widget_set_halign (cancel_button, GTK_ALIGN_CENTER);
	g_signal_connect (cancel_button, "clicked", G_CALLBACK (cancel_cb), udp);

	// Loop through the directory pointer array and gather entry data		
	for (int i = 0; udp->fdpp[i]; i++) { // if fdpp[i] is NULL, then no more directories

		// Recursively load store with directory and entry data
		if (traverse (udp->fdpp[i], udp)) {

			// No show stopper (0) so get results 
			if (get_results (udp->list_store, udp)) { 

				// No show stopper (0) so check to see if result type should be included
				// Sort the list store by result so can find start and end of unique, empty, group and directory results as needed
				g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_a, udp); // Sort by result column ascending

				// Remove the entries from the list store that are not to be included
				if (!udp->opt_include_empty) find_and_splice(udp->list_store, STR_EMP);
				if (!udp->opt_include_directory) find_and_splice(udp->list_store, STR_DIR);
				if (!udp->opt_include_unique) find_and_splice(udp->list_store, STR_UNI);
				if (!udp->opt_include_duplicate) splice_group(udp->list_store); // Remove the group entries

				adjust_sfs_button_sensitivity(udp);  // Turn on the sort, filter, search bar
			        show_columns(udp);	
			}
			else { // Results return is 0
				break;
			}	
		}
		else { // Traverse return is 0
			break;
		}
	} // End for
}       	
