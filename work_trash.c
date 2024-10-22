#include "main.h"
#include "load_entry_data.h"
#include "work_selected.h"
#include "work_trash.h"

// Forward
void prompt_trash(user_data * udp);

// Put minimum value from bitset into passed unsigned int
// - Return true if got and then removed that value, false if not
// - A poor man's loop: seek another item to trash AFTER trashing the last found item
// - Loop approach is needed to accomodate the prompt to continue

gboolean get_remove_min(GtkBitset *bitset, guint *pos)
{
	gboolean get = FALSE, remove = FALSE;
	GtkBitsetIter iter;

	get = gtk_bitset_iter_init_first(&iter, bitset, pos);
	if (get)
		remove = gtk_bitset_remove(bitset, *pos);

	if (get && remove) {
		printf("get_remove_min true\n");
		return TRUE;	/* Nothing to get or remove */
	} else {
		printf("get_remove_min false\n");
		return FALSE;
	}
}

// Count the number of items in a group

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

// See if another to trash, if so work it
void try_another(user_data *udp)
{
	if (get_remove_min(udp->sel_bitset, &udp->sel_item_position)) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store),
						      udp->sel_item_position);
		udp->sel_item = item;
		prompt_trash(udp);
	} 
	else udp->sel_item = NULL;
}

// Remove all items from the list store
gboolean remove_all_items(user_data *udp)
{
	g_list_store_remove_all(udp->list_store);
	return FALSE; // g_idle_add requires a false return to end
}

// Remove a single item from the list store
gboolean remove_single_item(user_data *udp)
{
	g_list_store_remove(udp->list_store, udp->sel_item_position);
	try_another(udp);
	return FALSE; // g_idle_add requires a false return to end
}

// Remove a single item and change the other entry to unique
gboolean remove_item_and_change(user_data *udp)
{
	const char *result = udp->sel_item->result;
	g_list_store_remove(udp->list_store, udp->sel_item_position);

	change_result_to_unique(udp->list_store, result);

	try_another(udp);
	return FALSE; // g_idle_add requires a false return to end
}

// Trash on system and remove item  
// - Selected item may be a Directory, empty, unique or duplicate (group numbers)
// - Change result to "Unique" if group of two and one remains
// - Idle call back is required to avoid segment fault when removing last item in list store
// - Trash, not delete, the file or directory on the system

void trash_and_remove(user_data *udp)
{
	if (!udp->sel_item)
		return;		// Nothing to do

	// Trash the file
	GFile *gf = g_file_new_for_path(udp->sel_item->name);	// Get the name
	if (!g_file_trash(gf, NULL, NULL)) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't trash entry");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		wipe_selected(udp); // Clear selected			
		return;
	}
	g_object_unref(gf);

	// If trash is good have to remove the entry from the list store and perform any follow-on actions
	const char *selected_result = udp->sel_item->result;	// save so can pass
	if (!strcmp(selected_result, "Unique") || !strcmp(selected_result, "Empty")) {
		g_idle_add((GSourceFunc) remove_single_item, udp);
	} else if (!strcmp(selected_result, "Directory")) {
		g_idle_add((GSourceFunc) remove_all_items, udp);
	} else if (count_match(udp->list_store, selected_result) == 2) {	// Must be a group, if two then change other to unique
		g_idle_add((GSourceFunc) remove_item_and_change, udp);
	} else {
		// If here, more than two items in group
		g_idle_add((GSourceFunc) remove_single_item, udp);
	}
}

// Process choice for trash proceed or cancel prompt
// - Proceed is button 0, cancel is 1 (default and escape)
// - If proceed then remove the item via idle callback

void work_trash_choice(GObject *source_object, GAsyncResult *res, void *ptr)
{
	GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
	user_data *udp = ptr;

	int button = gtk_alert_dialog_choose_finish(dialog, res, NULL);
	if (button == 0) trash_and_remove(udp);
}

// Trash (or not) the selected item
// - Weed out errors
// - Prompt to make sure should proceed

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

/* 
Seed the trash process
- Close the action window
- Get the item to trash
- Prompt to trash the item
*/
void work_trash_cb(GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->action_window));

	if (get_remove_min(udp->sel_bitset, &udp->sel_item_position)) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store),
						      udp->sel_item_position);
		udp->sel_item = item;
		prompt_trash(udp);
	}
}
