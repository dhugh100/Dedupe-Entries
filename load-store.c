// This file, load-store.c, is a part of the Dedupe Entries program.
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
#include "show-columns.h"
#include "traverse.h"
#include "get-results.h"
#include "work-auto.h"
#include "lib.h"
#include "load-store.h"

// Cancel button hit, set cancel to true and do cleanup when cycles available

void cancel_cb (GtkWidget *self, user_data *udp)
{
	udp->cancel_request = TRUE;
	g_idle_add((GSourceFunc) cancel_clean_up, udp);
}

// Default sort compare
// - Default is result primary, name secondary, both ascending

int default_sort_cmp (const void *a, const void *b, user_data *udp)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result)) 
		return (strcmp(item1->name, item2->name)); 
        else 
                return (strcmp(item1->result, item2->result));
}
// Exclude the empty, directory, group and unique items if not directed to be included in options

void exclude_items (user_data *udp)
{
	// Get count of items in list store
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(udp->list_store));

	for (int i = 0; i < cnt; i++) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i);
		if ((!udp->opt_include_unique && !strcmp(item->result, STR_UNI)) ||
		    (!udp->opt_include_directory && !strcmp(item->result, STR_DIR)) ||
		    (!udp->opt_include_empty && !strcmp(item->result, STR_EMP)) ||
		    (!udp->opt_include_duplicate && isdigit(item->result[0]))) {
			free_item_memory(item);
			g_list_store_remove(udp->list_store, i);
			i--; // Adjust for removed item
			cnt--; // Adjust for removed item
		}
		g_object_unref(item);
	}
}

// Drive getting and showing entry duplicates and data
// - Create the store
// - Setup the progress box, progress bar, and cancel button
// - Launch a traverse of each folder and store the entry data in the list store
// - Using the entry data, determine duplicates and other values for the result column
// - Parse out any unwanted result types
// - Launch and show the data in the columns OR auto dedupe
// - Can re-enter multiple times (mutliple gets)

void load_entry_data (user_data *udp)
{
	clear_stores(udp); // Clear the stores and associated item memory

	if (!udp->list_store) {
		GListStore *list_store = g_list_store_new(G_TYPE_OBJECT);
		udp->list_store = list_store; // Save pointer to list store
	}

	// Remove and collect any existing child
	gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);

	// Create a box to hold the progress bar
	GtkWidget *progress_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_window_set_child(GTK_WINDOW(udp->main_window), progress_box);

	// Create a progress bar
	GtkWidget *progress_bar = gtk_progress_bar_new();
	udp->progress_bar = progress_bar; // Save pointer to progress bar
	gtk_progress_bar_set_show_text((GtkProgressBar *) progress_bar, TRUE);
	gtk_box_append(GTK_BOX(progress_box), progress_bar);

	// Create the cancel button
	GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
	udp->cancel_button = cancel_button;
	gtk_box_append(GTK_BOX(progress_box), cancel_button);
	gtk_widget_set_halign(cancel_button, GTK_ALIGN_CENTER);
	g_signal_connect(cancel_button, "clicked", G_CALLBACK(cancel_cb), udp);

	// Loop through the directory pointer array and gather entry data               
	for (int i = 0; udp->fdpp[i]; i++) { // If fdpp[i] is NULL, then no more directories

		// Recursively load store with directory and entry data
		if (!traverse(udp->fdpp[i], udp)) break; // 0 for show stopper 

		// No show stopper (0) so get results 
		if (!get_results(udp)) break;

		// No show stopper (0) so check to see if result type should be included
		if (!udp->opt_include_unique || !udp->opt_include_directory ||
	       	    !udp->opt_include_empty || !udp->opt_include_duplicate)
 			exclude_items(udp);

		g_list_store_sort(udp->list_store, (GCompareDataFunc) default_sort_cmp, NULL);

	} // End for

	// If something to work do manual or auto follow on
	// - Must be groups found for auto dedupe
	if (!udp->ut_active && g_list_model_get_n_items(G_LIST_MODEL(udp->list_store))) {
		adjust_sfs_button_sensitivity(udp);
		if (udp->auto_dedupe) {
			udp->auto_dedupe = FALSE; // Have to reselect auto dedupe
			DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), 0);
			// If no groups auto pointless throw a message
			if (!isdigit(item->result[0])) {  // Default sort puts groups (7 digits) first
				gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);
				GtkAlertDialog *alert = gtk_alert_dialog_new("No duplicates found");
				gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
				// g_object_unref(item);
			}	
			else {
				work_auto(udp); // Auto  dedupe
			}
			// g_object_unref(item);
		}	
		else {
		       	show_columns(udp); // Show and select for actions
		}			   
	}
}
