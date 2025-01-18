#include "main.h"
#include "lib.h"

// Free up the store items memory
 
void free_item_memory (DupItem *item)
{
        if (item->result) g_free((const gpointer)item->result);  
        if (item->name) g_free((const gpointer)item->name);  
        if (item->hash) g_free((const gpointer)item->hash);
        if (item->file_size) g_free((const gpointer)item->file_size);
        if (item->modified) g_free((const gpointer)item->modified);
}

// Clear the store items
// - Have to free up the memory allocated for each string
// - Gio remove doesn't free up the memory

void clear_store_items(GListStore *list_store)
{
/*	
        uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(list_store));
        for (int i = 0; i < cnt; i++) {
                DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), i); 
                free_item_memory(item);  
                g_object_unref(item);
        }
*/	
        g_list_store_remove_all(list_store);
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
