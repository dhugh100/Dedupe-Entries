// This file, lib.c, is a part of the Entry Dedupe program.
// 
// Copyright (C) 2024  David Hugh
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

// Cean up pending events

void do_pending ()
{
        if (g_main_context_pending(NULL)) g_main_context_iteration(NULL, FALSE);
}

// Used when debugging programs

void see_entry_data (GListStore *list_store, GtkMultiSelection *selection)
{ 
        int cnt = g_list_model_get_n_items (G_LIST_MODEL (list_store));

        // Print out file data from the store to stdout
        for (int i = 0; i < cnt; i++)   {

                DupItem *item = g_list_model_get_item (G_LIST_MODEL (list_store), i);
                if (item == NULL) {
                        printf ("Error: item is NULL\n");
                        return;
                }
                printf ("\nRecord: %d Result: %s Result Ptr: %p", i, item->result, item->result);
                printf (" Name: %s Name Ptr: %p\n", item->name, item->name);

                printf ("Hash: %s", item->hash); 

                printf (" Size: %s", item->file_size);
                printf (" Modified: %s\n", item->modified);

                if (selection) 
                        printf ("Selected?: %s\n", gtk_selection_model_is_selected(GTK_SELECTION_MODEL (selection) , i) ? "Yes" : "No");
	}
}

// Cancel clean up
// - For cancel remove any entries from list store (could be partial and misleading)
// - Clear the folders 

void cancel_clean_up (user_data *udp)
{
        // Reset cancel request
        udp->cancel_request = FALSE;

        // Trash any child window
        gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);

        // Clean up any data
        g_idle_add((GSourceFunc)clear_stores, udp);

        // Clear the folder pointers, but leave array of pointers
        if (udp->fdpp) clear_folders(udp->fdpp);
}

// Clear folders
// - Free up the memory allocated for the folder strings
void clear_folders (char *fopp[MAX_FOLDERS])
{
        // Clear the folder data
        for (int i = 0; i < MAX_FOLDERS; i++) {
                if (fopp[i]) {
                        g_free (fopp[i]);
                        fopp[i] = NULL;
                }
                else
                        break;
        }
}

// Adjust the sensitivity of the sort, filter buttons and search bar based on the status of the list store
// - No sort, filter, or search if no list store
// - No search if filtering on

void adjust_sfs_button_sensitivity (user_data *udp)
{
       if (udp->list_store && 
           g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)) > 0 &&
           !udp->auto_dedupe) {
               gtk_widget_set_sensitive(udp->sort_button, TRUE);
               gtk_widget_set_sensitive(udp->filter_button, TRUE);
               gtk_widget_set_sensitive(udp->search_bar, TRUE);
       }
       else {
               gtk_widget_set_sensitive(udp->sort_button, FALSE);
               gtk_widget_set_sensitive(udp->filter_button, FALSE);
               gtk_widget_set_sensitive(udp->search_bar, FALSE);
       }
}

// Clear selected items

void wipe_selected(user_data *udp)
{
        gtk_selection_model_unselect_all(GTK_SELECTION_MODEL (udp->selection));

        gtk_bitset_remove_all (udp->sel_bitset);
        gtk_bitset_unref (udp->sel_bitset);
        udp->sel_bitset = NULL;
}

// Free up a store's item memory
 
void free_item_memory (DupItem *item)
{
        g_free((const gpointer)item->result);  
        g_free((const gpointer)item->name);  
        g_free((const gpointer)item->hash);
        g_free((const gpointer)item->file_size);
        g_free((const gpointer)item->modified);
}

// Clear the store items
// - Have to free up the memory allocated for each string
// - Gio remove doesn't free up the memory

void clear_store_items(GListStore *list_store)
{
        uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(list_store));
        for (int i = 0; i < cnt; i++) {
                DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), i); 
                free_item_memory(item);  
                g_object_unref(item);
        }
        g_list_store_remove_all(list_store);
}

void clear_stores(user_data *udp)
{
	// Clean up
        if (udp->org_list_store) {  // Will be most complete if not null
                clear_store_items(udp->org_list_store); 
                g_object_unref(udp->org_list_store);
		udp->org_list_store = NULL;
                if (udp->filtered_list_store) {
                        g_list_store_remove_all(udp->filtered_list_store);
                        g_object_unref(udp->filtered_list_store);
			udp->list_store = NULL;
                }
                if (udp->list_store) {
                        g_list_store_remove_all(udp->list_store);
                        g_object_unref(udp->list_store);
			udp->list_store = NULL;
                }
        }
        else if (udp->list_store) {
                clear_store_items(udp->list_store); 
                g_object_unref(udp->list_store);
		udp->list_store = NULL;
        } 
}

// Read options from file
// - Store in buffer

gboolean read_options(unsigned char *buff, char *name)
{
        // Get file size, and if 0 exit
        struct stat attr;
        if (stat(name, &attr) == -1) {
		return FALSE;
	}
        //if (attr.st_size != OPTION_SIZE) {
        //        return FALSE;
        //}

        // Setup file and stream
        GFile *file = g_file_new_for_path(name);
        GFileInputStream *in = g_file_read(file, NULL, NULL);

        // Read into buff
        gsize read;
        gboolean result =  g_input_stream_read_all (G_INPUT_STREAM(in), buff, 8, &read,  NULL, NULL);

        // Clean up
        g_input_stream_close (G_INPUT_STREAM(in), NULL, NULL);
        g_object_unref (in);
        g_object_unref (file);
        return result;
}

// Trash on system and remove item
// - Trash, not delete, the selected item
// - Called when user elects to proceed with trash
// - Selected items may be a Directory, Empty, Unique or duplicate (group numbers)
// - After altering file system via trash reload store

void trash_em (user_data *udp)
{
        // Seed the iterator
        GtkBitsetIter iter;
        guint value = 0;
        gtk_bitset_iter_init_first(&iter, udp->sel_bitset, &value);
        GFile *gf = NULL;

        do {
                DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), value);
                gf = g_file_new_for_path(item->name); // Get the name
                if (!g_file_trash(gf, NULL, NULL)) {
                        GtkAlertDialog *alert = gtk_alert_dialog_new("Can't trash entry");
                        gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
                        g_object_unref(gf);
                        wipe_selected(udp); // Clear selected
                        return;
		}	
                g_object_unref(gf);
                g_object_unref(item);

        } while (gtk_bitset_iter_next(&iter, &value));

        load_entry_data(udp); // Reload list store
        return;
}

