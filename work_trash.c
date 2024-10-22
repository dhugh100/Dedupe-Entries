#include "main.h"
#include "load_entry_data.h"
#include "work_selected.h"
#include "work_trash.h"

// Forward declaration
void prompt_trash(user_data * udp);

// Put minimum value from bitset into passed unsigned int
// - Called to see if there is another item to trash following a trash/removal
// - Return true if a) got and then b) removed that value, false if not
// - A poor man's loop: seek another item to trash AFTER trashing the last found item
// - Loop approach accommodates the prompt to continue

gboolean get_remove_min(GtkBitset *bitset, guint *pos)
{
	gboolean get = FALSE, remove = FALSE;
	GtkBitsetIter iter;

	get = gtk_bitset_iter_init_first(&iter, bitset, pos);
	if (get) remove = gtk_bitset_remove(bitset, *pos);

	if (get && remove) return TRUE;	
	else return FALSE; 
}

// Count the number of items in a group of dups
// - Called to determine how many items are in the same group of dups

uint32_t count_match(GListStore *list_store, const char *result)
{
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(list_store));
	uint32_t found = 0;

	// Loop through list store and count matches to result
	for (uint32_t i = 0; i < cnt; i++) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), i);
		if (item && !strcmp(item->result, result)) {
			found++;
		}
		g_object_unref(item);
	}
	return found;
}

// Change item result from a group number to unique
// - Called when an item in a group of two items is being removed
// - Remaining item must be changed to "Unique"

void change_result_to_unique(GListStore *list_store, const char *result)
{
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(list_store));

	// Loop through list store and change result to "Unique" if match
	for (uint32_t i = 0; i < cnt; i++) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), i);
		if (!strcmp(item->result, result)) {
			g_object_set(item, "result", "Unique", NULL);
			g_object_unref(item);
			break;	// Only one change needed
		}
		g_object_unref(item);
	}
	return;
}

// Count a result string in selected items
// - Returns the count of the result string in the selected items
// - Limit the number of iterations by setting a cnt limit, ignore if limit is 0

uint32_t count_selected_result(GtkBitset *bitset, GListStore *list_store, const char *result, uint32_t limit)
{
	uint32_t cnt = 0;
	
        // Seed the iterator
        GtkBitsetIter iter;
        guint value = 0;  
        gtk_bitset_iter_init_first (&iter, bitset, &value);

	do {
        	DupItem *item = g_list_model_get_item (G_LIST_MODEL (list_store), value);
		if (!strcmp(item->result, result)) cnt++;
		if (limit && 
		    cnt >= limit) break;
		g_object_unref(item);
	} while (gtk_bitset_iter_next(&iter, &value)); 

	return cnt;
}


// See if another to trash, if so get and prompt
// - Called after an item is trashed
// - If found another  update udp with found and call prompt

void try_another(user_data *udp)
{
	if (get_remove_min(udp->sel_bitset, &udp->sel_item_position)) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store),
						      udp->sel_item_position);
		udp->sel_item = item;
		prompt_trash(udp);
	} 
	else {
		wipe_selected(udp); // All done, clear selected
	}				    
}

// Remove all items from the list store
// - Called via idle when a directory is being removed
// - Don't look for more, just remove all and end

gboolean remove_all_items(user_data *udp)
{
	g_list_store_remove_all(udp->list_store);
	return FALSE; // g_idle_add requires a false return to end
}

// Remove an item from the list store
// - Called via idle add when an item should be removed
// - Then see if there is another item to trash and remove

gboolean remove_an_item(user_data *udp)
{
	g_list_store_remove(udp->list_store, udp->sel_item_position);

	try_another(udp);
	return FALSE; // g_idle_add requires a false return to end
}

// Remove an item and change the other entry to unique
// - Called via idle add when item should be remove
// - Look for partner item to change to unique
// - Then see if there is another item to trash and remove

gboolean remove_item_and_change(user_data *udp)
{
	const char *result = udp->sel_item->result;
	g_list_store_remove(udp->list_store, udp->sel_item_position);

	change_result_to_unique(udp->list_store, result);

	try_another(udp);
	return FALSE; // g_idle_add requires a false return to end
}

// Trash on system and remove item  
// - Trash, not delete, the selected item
// - Called when user elects to proceed with trash
// - Selected item may be a Directory, Empty, Unique or duplicate (group numbers)
// - Process adjustments to list store after successful trash
// - Idle call back is required to avoid segment fault when removing last item in list store

void trash_and_remove(user_data *udp)
{
	if (!udp->sel_item) return; // Nothing to do

	// Trash the file
	GFile *gf = g_file_new_for_path(udp->sel_item->name);	// Get the name
	if (!g_file_trash(gf, NULL, NULL)) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't trash entry");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		wipe_selected(udp); // Clear selected			
		return;
	}
	g_object_unref(gf);

	// If trash is good remove the entry from the list store and perform any follow-on actions
	const char *selected_result = udp->sel_item->result;	// sa ve so can pass

	if (!strcmp(selected_result, "Unique") || 
	    !strcmp(selected_result, "Empty")) 
		g_idle_add((GSourceFunc) remove_an_item, udp);

	else if (!strcmp(selected_result, "Directory")) 
		g_idle_add((GSourceFunc) remove_all_items, udp);

	else if (count_match(udp->list_store, selected_result) == 2) 
		// If here must be a group of dups, if two dups then change other to unique
		g_idle_add((GSourceFunc) remove_item_and_change, udp);
	else 
	 	 // If here, more than two items in group
		g_idle_add((GSourceFunc) remove_an_item, udp);
	
}

// Process choice for trash proceed or cancel prompt
// - Called by prompt function
// - Proceed is button 0, cancel is 1 (default and escape)
// - If proceed then trash and remove

void work_trash_choice(GObject *source_object, GAsyncResult *res, void *ptr)
{
	GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
	user_data *udp = ptr;

	int button = gtk_alert_dialog_choose_finish(dialog, res, NULL);
	if (button == 0) trash_and_remove(udp);
}

// Trash (or not) prompt for the selected item
// - Called by seed function and after a trash/remove
// - Cancel is default and escape

void prompt_trash(user_data *udp)
{

	// Don't try to trash an error entry
	if (!strncmp(udp->sel_item->result, "Error", 5)) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't trash an error entry");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		wipe_selected(udp); // Clear selected
		return;
	}

	// Null terminated button list
	const char *buttons[] = { "Proceed", "Cancel", NULL };

	GtkAlertDialog *alert = gtk_alert_dialog_new("Trash ???");
	gtk_alert_dialog_set_detail(alert, udp->sel_item->name);
	gtk_alert_dialog_set_buttons(alert, buttons);
	gtk_alert_dialog_set_cancel_button(alert, 1);	// For escape
	gtk_alert_dialog_set_default_button(alert, 1);
	gtk_alert_dialog_set_modal(alert, FALSE);
	gtk_alert_dialog_choose(alert, GTK_WINDOW(udp->main_window), NULL, work_trash_choice, udp);
}

 
// Seed the trash process
// - Called by the trash button in the action window
// - Close the action window
// - Get the item to trash
// - Prompt to trash the item

void work_trash_cb(GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->action_window));

	// A directory trash should be associated with just one request
	if (gtk_bitset_get_size(udp->sel_bitset) > 1 &&
	    count_selected_result(udp->sel_bitset, udp->list_store, "Directory", 1 ) > 0) { 
		GtkAlertDialog *alert = gtk_alert_dialog_new("Directory removals can't be combined");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		wipe_selected(udp); // Clear selected
		return;
	}

	if (get_remove_min(udp->sel_bitset, &udp->sel_item_position)) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store),
						      udp->sel_item_position);
		udp->sel_item = item;
		prompt_trash(udp);
	}
}
