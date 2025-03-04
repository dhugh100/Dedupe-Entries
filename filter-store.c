// This file, filter-store.c, is a part of the Dedupe Entries program.
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
#include "filter-store.h"

// Set sensitivity of clear button - gray out if no text is in either entry buff text

void set_sensitivity_clear_button (filter_entry *ep)
{

	if (strlen(ep->res_ebt) > 0 || strlen(ep->name_ebt) > 0)
		gtk_widget_set_sensitive(ep->clear_btn, TRUE);
	else
		gtk_widget_set_sensitive(ep->clear_btn, FALSE);
}

void shallow_copy_store (GListStore *src, GListStore *dst)
{
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(src));
	for (uint32_t i = 0; i < cnt; i++) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(src), i);
		g_list_store_append(G_LIST_STORE(dst), item);
		g_object_unref(item);
	}
}

// Check if the filter text in result
// - Return based on if a match and if a match is sought

gboolean subres (DupItem *item, user_data *udp)
{
	// False if not sought and filter text is in result
	if (udp->fep->res_n) {
		if (strstr(item->result, udp->fep->res_ebt))
			return FALSE;
		else
			return TRUE;
	}
	else {
		if (strstr(item->result, udp->fep->res_ebt))
			return TRUE;
		else
			return FALSE;
	}
}

// Check if the filter text in name
// - Return based on if a match and if a match is sought

gboolean subname (DupItem *item, user_data *udp)
{
	// False if not sought and filter text is in name
	if (udp->fep->name_n) {
		if (strstr(item->name, udp->fep->name_ebt))
			return FALSE;
		else
			return TRUE;
	}
	else {
		if (strstr(item->name, udp->fep->name_ebt))
			return TRUE;
		else
			return FALSE;
	}
}

// Filter check
// - For the match table, treat the combination of sought and match as one factor
// - And/or logic,  result match yes/no, name match yes/no; 2^3 (8)possibilities

gboolean filter_match (DupItem *item, user_data *udp)
{
	// Logic for and/or, result yes/no, name yes/no
	gboolean state = (udp->fep->and && subres(item, udp) && subname(item, udp) ? TRUE : FALSE) ||	// T T T = T
	    (udp->fep->and && subres(item, udp) && !subname(item, udp) ? FALSE : FALSE) ||	// T T F = F
	    (udp->fep->and && !subres(item, udp) && subname(item, udp) ? FALSE : FALSE) ||	// T F T = F
	    (udp->fep->and && !subres(item, udp) && !subname(item, udp) ? FALSE : FALSE) ||	// T F F = F
	    (!udp->fep->and && subres(item, udp) && subname(item, udp) ? TRUE : FALSE) ||	// F T T = T
	    (!udp->fep->and && subres(item, udp) && !subname(item, udp) ? TRUE : FALSE) ||	// F T F = T
	    (!udp->fep->and && !subres(item, udp) && subname(item, udp) ? TRUE : FALSE) ||	// F F T = T
	    (!udp->fep->and && !subres(item, udp) && !subname(item, udp) ? FALSE : FALSE); // F F F = F

	return state;
}

// Build the filtered list store
// - Remove all items from the new list store

void build_filtered_store(GListStore *current, GListStore *new, user_data *udp)
{
	// Build the list store based on filter
	uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(current));
	for (uint32_t i = 0; i < cnt; i++) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(current), i);
		if (filter_match(item, udp)) {
			g_list_store_append(new,item);
		}	
		g_object_unref(item);
	}
}


// Callback to apply the filter
// - Close the filter window
// - Filters are successively applied to the filtered list store
// - So need to save the original list store and the filtered list store
// - If the original list store is NULL, then this is the first filter

void apply_filters_cb (GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->filter_window));
	udp->filter_window = NULL;

	// If first filter
	if (!udp->org_list_store) { 
		udp->org_list_store = g_list_store_new(G_TYPE_OBJECT);
		udp->filtered_list_store = g_list_store_new(G_TYPE_OBJECT);
		shallow_copy_store(udp->list_store, udp->org_list_store);
		shallow_copy_store(udp->list_store, udp->filtered_list_store);
	}
	g_list_store_remove_all(udp->list_store);	
	build_filtered_store(udp->filtered_list_store, udp->list_store, udp);
}

// Callback to get text from the name entry buffer

void name_buff_cb (GtkWidget *self, user_data *udp)
{
	strncpy(udp->fep->name_ebt, gtk_entry_buffer_get_text(udp->fep->name_eb), STR_ENTRY);
}

// Callback to get the text from the result entry buffer

void result_buff_cb (GtkWidget *self, user_data *udp)
{
	strncpy(udp->fep->res_ebt, gtk_entry_buffer_get_text(udp->fep->res_eb), STR_ENTRY);
}

// Set the and logic radio button to true or false

void and_cb (GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self)) {
		udp->fep->and = TRUE;
		udp->fep->or = FALSE;
	}
	else {
		udp->fep->and = FALSE;
		udp->fep->or = TRUE;
	}
}

// Set the or logic radio button to true or false

void or_cb (GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self)) {
		udp->fep->or = TRUE;
		udp->fep->and = FALSE;
	}
	else {
		udp->fep->or = FALSE;
		udp->fep->and = TRUE;
	}
}

// Set the result not radio button to true of false

void res_n_cb (GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self))
		udp->fep->res_n = TRUE;
	else
		udp->fep->res_n = FALSE;
}

// Set the name not radio button to true or false

void name_n_cb (GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self))
		udp->fep->name_n = TRUE;
	else
		udp->fep->name_n = FALSE;
}


void initialize_filter (user_data *udp)
{
	// Clear the entry buffers if initialed and set defaults
	if (udp->fep->res_eb) {
		g_object_unref(udp->fep->res_eb); 
		g_object_unref(udp->fep->name_eb);
	}       	

	memset(udp->fep, 0x00, sizeof(filter_entry));
	udp->fep->and = TRUE;

	// Copy the saved store to list store if there
	if (udp->org_list_store) {
		g_list_store_remove_all(udp->list_store);
		shallow_copy_store(udp->org_list_store, udp->list_store);

		// Now clear the saved store
		g_list_store_remove_all(udp->org_list_store);
		g_list_store_remove_all(udp->filtered_list_store);
		g_object_unref(udp->org_list_store);
		g_object_unref(udp->filtered_list_store);
		udp->org_list_store = NULL;
		udp->filtered_list_store = NULL;
	}
}

void clear_filters_cb (GtkWidget *self, user_data *udp)
{	
	gtk_window_close(GTK_WINDOW(udp->filter_window));
	initialize_filter(udp);
}

// Prompt filter choice

void get_filters_cb (GtkWidget *self, user_data *udp)
{
	// Don't do succesive filters
	// initialize_filter(udp);

	// Create window and add title
	GtkWidget *filter_window = gtk_window_new();
	udp->filter_window = filter_window;
	gtk_window_set_title(GTK_WINDOW(filter_window), "Filter Options");

	// Create grid for entry widgets
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_widget_set_margin_top(GTK_WIDGET(grid), 10);
	gtk_widget_set_margin_bottom(GTK_WIDGET(grid), 10);

	// Makes labels for entry widgets
	GtkWidget *result_label = gtk_label_new("Result:");
	GtkWidget *name_label = gtk_label_new("Name:");

	// Make labels for sections
	GtkWidget *logic_label = gtk_label_new("LOGIC:");

	// Setup Check Boxes
	GtkWidget *and_chk_box = gtk_check_button_new_with_label("AND");
	GtkWidget *or_chk_box = gtk_check_button_new_with_label("OR");
	GtkWidget *res_n_chk_box = gtk_check_button_new_with_label("Not");
	GtkWidget *name_n_chk_box = gtk_check_button_new_with_label("Not");

	gtk_check_button_set_group(GTK_CHECK_BUTTON(and_chk_box), GTK_CHECK_BUTTON(or_chk_box));

	// Set check boxes to active or not
	// - If neither and/or logic selected earlier, force and as default

	if (!udp->fep->and && !udp->fep->or)
		udp->fep->and = TRUE;

	// Have check boxes reflect stored state
	if (udp->fep->and)
		gtk_check_button_set_active((GtkCheckButton *) and_chk_box, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) and_chk_box, FALSE);

	if (udp->fep->res_n)
		gtk_check_button_set_active((GtkCheckButton *) res_n_chk_box, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) res_n_chk_box, FALSE);

	if (udp->fep->name_n)
		gtk_check_button_set_active((GtkCheckButton *) name_n_chk_box, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) name_n_chk_box, FALSE);

	// Setup result and entry buffs
	// - Carry over into entry buffs any prior, uncleared filter

	udp->fep->res_eb = gtk_entry_buffer_new((const char *)udp->fep->res_ebt, strlen(udp->fep->res_ebt));
	udp->fep->res_e = gtk_entry_new_with_buffer(udp->fep->res_eb);

	udp->fep->name_eb = gtk_entry_buffer_new((const char *)udp->fep->name_ebt, strlen(udp->fep->name_ebt));
	udp->fep->name_e = gtk_entry_new_with_buffer(udp->fep->name_eb);

	// Apply Button
	GtkWidget *apply_btn = gtk_button_new_with_label("Apply");
	gtk_widget_set_halign(apply_btn, GTK_ALIGN_CENTER);

	// Clear Button
	udp->fep->clear_btn = gtk_button_new_with_label("Clear");
	gtk_widget_set_halign(udp->fep->clear_btn, GTK_ALIGN_CENTER);

	// Grid attach by column, row, width, height

	// Row 1 
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(result_label), 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(udp->fep->res_e), 2, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(res_n_chk_box), 3, 1, 1, 1);

	// Row 2
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(name_label), 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(udp->fep->name_e), 2, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(name_n_chk_box), 3, 2, 1, 1);

	// Row 3
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(logic_label), 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(and_chk_box), 2, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(or_chk_box), 3, 3, 1, 1);

	// Row 5
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(apply_btn), 1, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(udp->fep->clear_btn), 3, 5, 1, 1);

	// Connect signals
	g_signal_connect(udp->fep->res_e, "changed", G_CALLBACK(result_buff_cb), udp);
	g_signal_connect(udp->fep->name_e, "changed", G_CALLBACK(name_buff_cb), udp);
	g_signal_connect(apply_btn, "clicked", G_CALLBACK(apply_filters_cb), udp);
	g_signal_connect(udp->fep->clear_btn, "clicked", G_CALLBACK(clear_filters_cb), udp);
	g_signal_connect(and_chk_box, "toggled", G_CALLBACK(and_cb), udp);
	g_signal_connect(or_chk_box, "toggled", G_CALLBACK(or_cb), udp);
	g_signal_connect(res_n_chk_box, "toggled", G_CALLBACK(res_n_cb), udp);
	g_signal_connect(name_n_chk_box, "toggled", G_CALLBACK(name_n_cb), udp);

	set_sensitivity_clear_button(udp->fep);

	// Add grid to window and show
	gtk_window_set_child(GTK_WINDOW(filter_window), grid);
	gtk_window_present(GTK_WINDOW(filter_window));
}
