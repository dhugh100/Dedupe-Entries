// This file, work-auto.c, is a part of the Dedupe Entries program.
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
#include "lib.h"
#include "work-auto.h"


// Comparison function for sort of result ascending modified time ascending 

int sort_modified_a (const void *a, const void *b)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result))  // Equal result on primary sort
		return strcmp(item1->modified,item2->modified); // Ascending modified time sort (oldest first)
	else
		return strcmp(item1->result, item2->result);
}
// Comparison function for sort of result ascending modified time ascending 

int sort_modified_d (const void *a, const void *b)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result))  // Equal result on primary sort
		return strcmp(item2->modified,item1->modified); // Decending modified time sort (oldest first)
	else
		return strcmp(item1->result, item2->result);
}

// Comparison function for sort of result ascending name len ascending 

int sort_name_len_a (const void *a, const void *b)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result))  // Equal result on primary sort
		if (strlen(item1->name) < strlen(item2->name)) return -1; // Descending name str len
		else if (strlen(item1->name) > strlen(item2->name)) return 1; 
		else return 0;						    
	else
		return strcmp(item1->result, item2->result);
}

// Comparison function for sort of result ascending name len descending 

int sort_name_len_d (const void *a, const void *b)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result))  // Equal result on primary sort
		if (strlen(item2->name) < strlen(item1->name)) return -1; // Descending name str len
		else if (strlen(item2->name) > strlen(item1->name)) return 1; 
		else return 0;						    
	else
		return strcmp(item1->result, item2->result);
}

// Comparison function for sort ascending result ascending name  

int sort_name_a (const void *a, const void *b)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result))  // Equal result on primary sort
		return strcmp(item1->name,item2->name); // Ascending name
	else
		return strcmp(item1->result, item2->result);
}

// Comparison function for sort ascending result descending name 

int sort_name_d (const void *a, const void *b)
{
        DupItem *item1 = (DupItem *) a;
        DupItem *item2 = (DupItem *) b;
        if (!strcmp(item1->result, item2->result))  // Equal result on primary sort
		return strcmp(item2->name,item1->name); // Descending name
	else
		return strcmp(item1->result, item2->result);
}

void auto_cancel_cb (GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->auto_prompt_window));
	gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);
}

void auto_proceed_cb (GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->auto_prompt_window));
	trash_em(udp);
	gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);
}

// Confirm the user wants to trash the duplicates
void auto_prompt_trash (user_data *udp)
{
	// Setup a box for the buttons
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	// Setup buttons
        GtkWidget *auto_proceed = gtk_button_new_with_label("Proceed");
        GtkWidget *auto_cancel = gtk_button_new_with_label("Cancel");

	// Connect buttons to signals
        g_signal_connect(auto_proceed, "clicked", G_CALLBACK(auto_proceed_cb), udp);
        g_signal_connect(auto_cancel, "clicked", G_CALLBACK(auto_cancel_cb), udp);

	// Add buttons to the box
	gtk_box_append(GTK_BOX(box), auto_proceed);
	gtk_box_append(GTK_BOX(box), auto_cancel);

	// Setup the window
	GtkWidget *auto_prompt_window = gtk_window_new();
	udp->auto_prompt_window = auto_prompt_window;
	gtk_window_set_title(GTK_WINDOW(auto_prompt_window),"Hash???");
	gtk_window_set_default_size(GTK_WINDOW(auto_prompt_window), 50, 50);
	gtk_window_set_child(GTK_WINDOW(auto_prompt_window), box);

	// Show the window
	gtk_window_present(GTK_WINDOW(auto_prompt_window));
	gtk_widget_grab_focus (auto_cancel);
}

// Factory setup

static void setup_auto_list_cb(GtkSignalListItemFactory *self, GtkListItem *listitem)
{
        GtkWidget *lb = gtk_label_new(NULL);
        gtk_label_set_xalign(GTK_LABEL(lb), 0.0);
        gtk_list_item_set_child(listitem, lb);
}

// Factory bind

static void bind_auto_list_cb(GtkSignalListItemFactory *self, GtkListItem *listitem)
{
        GtkWidget *lb = gtk_list_item_get_child(listitem);
        GtkStringObject *strobj = gtk_list_item_get_item(listitem);
        char *markup = g_markup_printf_escaped("<span font_desc='mono'>%s</span>", gtk_string_object_get_string(strobj));
        gtk_label_set_markup(GTK_LABEL(lb), markup);
        g_free(markup);
}

// Identify the entries to remain or be trashed
// - The first entry in a group is kept, the other entries are trashed

void id_remain_trash(user_data *udp)
{
	// Strings for the auto view
	char str[256] = {0x00};
	char group[10] = {0x00};

	// No selection needed, setup the bitset for the trash function
	udp->sel_bitset = gtk_bitset_new_empty ();

	// Setup an interation through the store
	guint32 cnt = g_list_model_get_n_items(G_LIST_MODEL(udp->list_store));
	guint32 i = 0;
	DupItem *item = g_object_new(DUP_TYPE_ITEM, NULL); 
	DupItem *next_item = g_object_new(DUP_TYPE_ITEM, NULL); 

	// Sort the store based on option for which members of a group remain or get trashed
	see_entry_data(udp->list_store, (GtkMultiSelection *)NULL);
	switch (udp->opt_preserve) {
		case AP_MOD_FIRST:
			g_list_store_sort(udp->list_store, (GCompareDataFunc)sort_modified_a, NULL);
			break;
		case AP_MOD_LAST:			
			g_list_store_sort(udp->list_store, (GCompareDataFunc)sort_modified_d, NULL);
			break;
		case AP_SHORTEST:
			g_list_store_sort(udp->list_store, (GCompareDataFunc)sort_name_len_a, NULL);
			break;
		case AP_LONGEST:			
			g_list_store_sort(udp->list_store, (GCompareDataFunc)sort_name_len_d, NULL);
			break;
		case AP_ASCENDING:
			g_list_store_sort(udp->list_store, (GCompareDataFunc)sort_name_a, NULL);
			break;
		case AP_DESCENDING:			
			g_list_store_sort(udp->list_store, (GCompareDataFunc)sort_name_d, NULL);
			break;
		default:	
			break;
	}		
			
	// Loop through the store, checking to see if next item is a fellow group member
	// - Selects first member of group to preserve, all others are marked for trash via bitmap
	// - Relies on sorting above to ensure order of group members is correct
	
	for (;  i + 1 < cnt; i++) {
		// Get current and next items
		item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i);
		next_item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i + 1);

		// Stop if either are not a group
		if (!isdigit(item->result[0]) || !isdigit(next_item->result[0])) break;

		// If equal then either start or following entries in group
		if (!strcmp(item->result, next_item->result)) {
			if (strcmp(item->result, group)) { // 1st in group if not equal
				snprintf(group, sizeof(group), "%s", item->result); // Use to identify last in group
				snprintf(str, sizeof(str), "%s - Group %s - Modified %s - Name: %s", "Remain", item->result, item->modified, item->name);
				gtk_string_list_append(udp->auto_list, str);
			}	
			else { // Middle of group
				gtk_bitset_add (udp->sel_bitset,i); // Add to bitset for trashing
				snprintf(str, sizeof(str), "%s - Group %s - Modified %s - Name: %s", "Trash ", item->result, item->modified, item->name);
				gtk_string_list_append(udp->auto_list, str);
			}
		}
		// Must be the last in the group
		else if (!strcmp(item->result, group)) { // Last in group
			gtk_bitset_add (udp->sel_bitset,i); // Add to bitset for trashing
			snprintf(str, sizeof(str), "%s - Group %s - Modified %s - Name: %s", "Trash ", item->result, item->modified, item->name);
			gtk_string_list_append(udp->auto_list, str);
		}
	}

	// Here because only one left or next was not in group
	item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i);
	if (!strcmp(item->result, group)) { // Last in group
		gtk_bitset_add (udp->sel_bitset,i); // add to bitset for trashing
		snprintf(str, sizeof(str), "%s - Group %s - Modified %s - Name: %s", "Trash ", item->result, item->modified, item->name);
		gtk_string_list_append(udp->auto_list, str);
	}

	g_object_unref(item);	
	g_object_unref(next_item);

	if (udp->opt_auto_prompt) auto_prompt_trash(udp);
	else trash_em(udp);
}

// Put the view of the auto dedupe result in the main window
// - Use a string list rather than columns
// - No actions/selection, just confirmation this is what is wanted

void work_auto (user_data *udp)
{
        // Initial string list model
        udp->auto_list = gtk_string_list_new(NULL);

	// Collect any child window
	gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);

        // Don't want to select anything for model
        GtkNoSelection *ns = gtk_no_selection_new(G_LIST_MODEL(udp->auto_list));

        // Setup the factory for the list view
        GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
        g_signal_connect(factory, "setup", G_CALLBACK(setup_auto_list_cb), NULL);
        g_signal_connect(factory, "bind", G_CALLBACK(bind_auto_list_cb), NULL);

        // Setup window and scrolled_window for file view
        GtkWidget *scrolled_window = gtk_scrolled_window_new();
        gtk_window_set_child(GTK_WINDOW(udp->main_window), GTK_WIDGET(scrolled_window));

        // Setup and show the list view with no selection
        GtkWidget *auto_view = gtk_list_view_new(GTK_SELECTION_MODEL(ns), factory);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), auto_view);
	id_remain_trash(udp);
}
