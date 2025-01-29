#include "main.h"
#include "lib.h"

// Adjust the sensitivity of the sort, filter buttons and search bar based on the status of the list store
// - No sort, filter, or search if no list store
// - No search if filtering on

void adjust_sfs_button_sensitivity (user_data *udp)
{
       if (udp->list_store && g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)) > 0) {
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
        stat(name, &attr);
        if (attr.st_size != OPTION_SIZE) {
                return FALSE;
        }

        // Setup file and stream
        GFile *file = g_file_new_for_path(name);
        GFileInputStream *in = g_file_read(file, NULL, NULL);

        // Read into buff
        gsize read;
        gboolean result =  g_input_stream_read_all (G_INPUT_STREAM(in), buff, OPTION_SIZE, &read,  NULL, NULL);

        // Clean up
        g_input_stream_close (G_INPUT_STREAM(in), NULL, NULL);
        g_object_unref (in);
        g_object_unref (file);
        return result;
}
