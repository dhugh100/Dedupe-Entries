// This file, work_trash.c, is a part of the Dedupe Entries program.
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
