// This file, search.c, is a part of the Dedupe Entries program.
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
#include "search.h"

// Process choice for search next or cancel prompt
// - Called by prompt function
// - Proceed is button 0, cancel is 1 (default and escape)
// - If proceed then find next match

void work_next_choice (GObject *source_object, GAsyncResult *res, void *ptr)
{
	GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
	user_data *udp = ptr;

	int button = gtk_alert_dialog_choose_finish(dialog, res, NULL);
	if (button == 0) { // Proceed
		work_search_entry_cb((GtkWidget *)NULL, udp);
	}
	else { // No next check
		// Scroll to next check if not at end or top row
		if (udp->next_check < g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)))
			gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view),
						  udp->next_check, NULL, GTK_LIST_SCROLL_NONE, NULL);
		else
			gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), 0, NULL, GTK_LIST_SCROLL_NONE, NULL);

		// Unselect and clear search
		gtk_selection_model_unselect_all(GTK_SELECTION_MODEL(udp->selection));
		udp->next_check = 0;
		gtk_editable_set_text(GTK_EDITABLE(udp->search_entry), ""); // Clear search entry
		udp->a_match = FALSE;
	}
}

// Prompt to See if should find next match in search entry

void prompt_next (user_data *udp)
{
	// Null terminated button list
	const char *buttons[] = { "Next", "Cancel", NULL };

	GtkAlertDialog *alert = gtk_alert_dialog_new("Find Next Match");
	gtk_alert_dialog_set_detail(alert, gtk_editable_get_text(GTK_EDITABLE(udp->search_entry)));
	gtk_alert_dialog_set_buttons(alert, buttons);
	gtk_alert_dialog_set_cancel_button(alert, 1); // For escape
	gtk_alert_dialog_set_default_button(alert, 1);
	gtk_alert_dialog_set_modal(alert, TRUE);

	gtk_alert_dialog_choose(alert, GTK_WINDOW(udp->main_window), NULL, work_next_choice, udp);
}

// Loop through list store to find match
// - Called by search entry callback
// - Returns index of match or end of list
// - If target text is substring of name or results then a match

uint32_t search_match_loop (GListStore *list_store, uint32_t cnt, uint32_t *next_check, const char *text)
{
	uint32_t i = *next_check;
	DupItem *item = g_object_new(DUP_TYPE_ITEM, NULL);
	for (; i < cnt; i++) {
		item = g_list_model_get_item(G_LIST_MODEL(list_store), i);
		if (strstr(item->result, text) || strstr(item->name, text)) {
			*next_check = i + 1;
			g_object_unref(item);
			break;
		}
		g_object_unref(item);
	}
	return i;
}

// Find search entry match in list store
// - Matching based on substring of either result or name
// - Will scroll to first match found to make sure visible
// - Can be called by search entry, prompt next, or self (auto restart from 0) 

void work_search_entry_cb (GtkWidget *self, user_data *udp)
{
	// Early exit if no list store or search entry  
	if (!udp->list_store) return; // Bug out if no list store
	if (!udp->search_entry) return; // Bug out if no entry

	// If self is positive then coming from signal, 1st time on search
	if (self) udp->next_check = 0;
	else gtk_selection_model_unselect_all(GTK_SELECTION_MODEL(udp->selection));
		
	// Get text from entry
	const char *text = gtk_editable_get_text(GTK_EDITABLE(udp->search_entry));
	if (!strlen(text)) return; // Bug out if search text is 0 length

	// Get number of items in list store
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(udp->list_store));
	if (!cnt) return; // Bug out if list_store is empty

	// Loop through list store and find match
	uint32_t i = search_match_loop (udp->list_store, cnt, &udp->next_check, text);

	if (udp->next_check == 0 && i == cnt) { // Did not find a match
		GtkAlertDialog *alert = gtk_alert_dialog_new("No match found");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
	}
	else if (udp->next_check > 0 && i < cnt) { // Found at least once, need to see if want to go again
		gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), i, NULL, GTK_LIST_SCROLL_SELECT, NULL);
		prompt_next(udp);
	}
	else if (udp->next_check > 0 && i == cnt) { // Found at least once, but not this time, rollover automatically and search from 0 
		gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), i, NULL, GTK_LIST_SCROLL_SELECT, NULL);
		udp->next_check = 0;
		work_search_entry_cb((GtkWidget *)NULL, udp);
	}
}
