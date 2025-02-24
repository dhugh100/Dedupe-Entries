// This file, work_trash.c, is a part of the Dedupe Entries program.
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
#include "work-trash.h"

// Forward declaration
void prompt_trash (user_data * udp);

// Count the number of selected items with a specific result

int count_selected_result (GtkBitset *bitset, GListStore *list_store, const char *result)
{
	GtkBitsetIter iter;
	uint32_t value = 0;
	uint32_t hit = 0;
	gtk_bitset_iter_init_first(&iter, bitset, &value);
	do {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(list_store), value);
		if (!strncmp(item->result, result, strlen(result))) hit++;
		g_object_unref(item);

	} while (gtk_bitset_iter_next(&iter, &value));

	return hit;
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

// Process choice for trash proceed or cancel prompt
// - Called by prompt function
// - Confirm is button 0, cancel is 1 (default and escape)
// - If confirmed trash and remove

void work_trash_choice (GObject *source_object, GAsyncResult *res, void *ptr)
{
	GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
	user_data *udp = ptr;

	int button = gtk_alert_dialog_choose_finish(dialog, res, NULL);
	if (button == 0) trash_em(udp);
	g_object_unref(dialog);
}

// Trash (or not) prompt for selected items
// - Cancel is default and escape

void prompt_trash (user_data *udp)
{
	// Null terminated button list
	const char *buttons[] = { "Confirm", "Cancel", NULL };

	GtkAlertDialog *alert = gtk_alert_dialog_new("??? Trash ???");
	gtk_alert_dialog_set_detail(alert, "Confirm trash all selected items");
	gtk_alert_dialog_set_buttons(alert, buttons);
	gtk_alert_dialog_set_cancel_button(alert, 1); // For escape
	gtk_alert_dialog_set_default_button(alert, 1);
	gtk_alert_dialog_set_modal(alert, FALSE);
	gtk_alert_dialog_choose(alert, GTK_WINDOW(udp->main_window), NULL, work_trash_choice, udp);
}

// Start the trash process
// - Called by the trash button in the action window
// - Close the action window
// - Prompt for confirmation
// - If confirmed, trash all items

void work_trash_cb (GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->action_window));

	// A directory trash should be associated with just one request
	if (gtk_bitset_get_size(udp->sel_bitset) > 1 && count_selected_result(udp->sel_bitset, udp->list_store, STR_DIR) > 0) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Directory removals must be one at a time");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		wipe_selected(udp); // Clear selected
		return;
	}
	if (gtk_bitset_get_size(udp->sel_bitset) > 0 && count_selected_result(udp->sel_bitset, udp->list_store, STR_ERR) > 0) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can not trash error entries");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		wipe_selected(udp); // Clear selected
		return;
	}

	prompt_trash(udp);
}
