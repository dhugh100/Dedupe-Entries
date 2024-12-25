#include "main.h"
#include "see-entry-data.h"

// Print out file data from the GListStore to stdout
void see_entry_data (GListStore *list_store, GtkMultiSelection *selection)
{ 
	int cnt = g_list_model_get_n_items (G_LIST_MODEL (list_store));

	// Print out file data from the array to stdout
	for (int i = 0; i < cnt; i++) 	{

		DupItem *item = g_list_model_get_item (G_LIST_MODEL (list_store), i);
		if (item == NULL) {
			printf ("Error: item is NULL\n");
			return;
		}
		printf ("\nRecord: %d Result: %s", i, item->result);
		printf (" Name: %s\n", item->name);

		printf ("Hash: %s", item->hash); 

		printf (" Size: %u", item->file_size);
		printf (" Modified: %s\n", item->modified);

		printf (" Selected?: %s\n", gtk_selection_model_is_selected(GTK_SELECTION_MODEL (selection) , i) ? "Yes" : "No");
	}
}
