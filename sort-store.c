// This file, sort-store.c, is a part of the Dedupe Entries program.
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
#include "sort-store.h"

// Comparsion function for sort of name descending
// - No secondary, as name should be unique

int cmp_name_d (const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *) a;
	DupItem *item2 = (DupItem *) b;
	return (strcmp(item2->name, item1->name)); // Item2 before item1 for descending
}

// Comparison function for sort of name ascending
// - No secondary, as name should be unique

int cmp_name_a (const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *) a;
	DupItem *item2 = (DupItem *) b;
	return strcmp(item1->name, item2->name);
}

// Used for secondary comparison
// - Item order is reversed at call time for ascending or descending

int cmp_sec (const void *a, const void *b)
{
	DupItem *item1 = (DupItem *) a;
	DupItem *item2 = (DupItem *) b;
	return strcmp(item1->name, item2->name);
}

// Comparison function for sort of result decending
// - Check for secondary sort direction

int cmp_result_d (const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *) a;
	DupItem *item2 = (DupItem *) b;
	if (!strcmp(item1->result, item2->result)) {
		if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_a_sec_btn)))
			return cmp_sec(a, b); // Ascending name sort on equal
		else if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_d_sec_btn)))
			return cmp_sec(b, a); // Descending name sort on equal
	}
	return strcmp(item2->result, item1->result);
}

// Comparison function for sort of result ascending
// - Check for secondary sort direction

int cmp_result_a (const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *) a;
	DupItem *item2 = (DupItem *) b;
	if (!strcmp(item1->result, item2->result)) {
		if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_a_sec_btn)))
			return cmp_sec(a, b); // Ascending name sort on equal
		else if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_d_sec_btn)))
			return cmp_sec(b, a); // Descending name sort on equal
	}
	return strcmp(item1->result, item2->result);
}

// Apply the sort
// - This function is called when the apply button is toggled

void apply_sort_cb (GtkCheckButton *button, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->sort_window));

	// Sort the list store
	if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->result_a_button)))
		g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_result_a, udp);
	else if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->result_d_button)))
		g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_result_d, udp);
	else if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_a_button)))
		g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_name_a, udp);
	else if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_d_button)))
		g_list_store_sort(udp->list_store, (GCompareDataFunc) cmp_name_d, udp);

	gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), 1, NULL, GTK_LIST_SCROLL_NONE, NULL);
}

// Alter sensitivity of secondary name options
// - Only valid when primary sort column is result
// - No secondary for primary sort on name as names are unique

void alter_secondary_sensitvity (GtkCheckButton *button, user_data *udp)
{
	if (gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_a_button))
	    || gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_d_button))) {
		gtk_widget_set_sensitive(udp->name_a_sec_btn, FALSE);
		gtk_widget_set_sensitive(udp->name_d_sec_btn, FALSE);
		gtk_check_button_set_active(GTK_CHECK_BUTTON(udp->name_a_sec_btn), FALSE);
		gtk_check_button_set_active(GTK_CHECK_BUTTON(udp->name_d_sec_btn), FALSE);
	}
	else {
		gtk_widget_set_sensitive(udp->name_a_sec_btn, TRUE);
		gtk_widget_set_sensitive(udp->name_d_sec_btn, TRUE);
		if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_a_sec_btn))
		    || !gtk_check_button_get_active(GTK_CHECK_BUTTON(udp->name_d_sec_btn))) {
			gtk_check_button_set_active(GTK_CHECK_BUTTON(udp->name_a_sec_btn), TRUE);
		}
	}
}

// Prompt the user to choose proceed or cancel
void get_sort_type_cb (GtkWidget *self, user_data *udp)
{
	// Create labels
	const char *format = "<span weight=\"bold\">\%s</span>";

	// Primary setup
	GtkWidget *primary_l = gtk_label_new(NULL);
	char *str = "Primary Sort Option";
	char *markup = g_markup_printf_escaped(format, str);
	gtk_label_set_markup(GTK_LABEL(primary_l), markup);
	g_free(markup);

	// Secondary setup
	GtkWidget *secondary_l = gtk_label_new("NULL");
	str = "Secondary Sort Option";
	markup = g_markup_printf_escaped(format, str);
	gtk_label_set_markup(GTK_LABEL(secondary_l), markup);
	g_free(markup);

	// Create check buttons
	// - No result secondary since no secondary for primary name sort 
	udp->result_a_button = gtk_check_button_new_with_label("Result Ascending");
	gtk_check_button_set_active(GTK_CHECK_BUTTON(udp->result_a_button), TRUE);
	udp->result_d_button = gtk_check_button_new_with_label("Result Descending");
	udp->name_a_button = gtk_check_button_new_with_label("Name Ascending");
	udp->name_d_button = gtk_check_button_new_with_label("Name Descending");
	udp->name_a_sec_btn = gtk_check_button_new_with_label("Name Ascending");
	gtk_check_button_set_active(GTK_CHECK_BUTTON(udp->name_a_sec_btn), TRUE);
	udp->name_d_sec_btn = gtk_check_button_new_with_label("Name Descending");

	// Setup apply button and callback
	GtkWidget *apply_btn = gtk_button_new_with_label("Apply");
	gtk_widget_set_halign(apply_btn, GTK_ALIGN_CENTER);
	g_signal_connect(apply_btn, "clicked", G_CALLBACK(apply_sort_cb), udp);

	// Create grid for entry widgets
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_widget_set_margin_top(GTK_WIDGET(grid), 10);
	gtk_widget_set_margin_bottom(GTK_WIDGET(grid), 10);

	// Place widgets in grid
	gtk_grid_attach(GTK_GRID(grid), primary_l, 0, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), udp->result_a_button, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), udp->result_d_button, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), udp->name_a_button, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), udp->name_d_button, 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), secondary_l, 0, 3, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), udp->name_a_sec_btn, 0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), udp->name_d_sec_btn, 1, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), apply_btn, 0, 5, 2, 1);

	// Set the group for the check buttons
	// Primary
	gtk_check_button_set_group(GTK_CHECK_BUTTON(udp->result_a_button), GTK_CHECK_BUTTON(udp->result_d_button));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(udp->name_a_button), GTK_CHECK_BUTTON(udp->result_a_button));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(udp->name_d_button), GTK_CHECK_BUTTON(udp->result_a_button));

	// Secondary
	gtk_check_button_set_group(GTK_CHECK_BUTTON(udp->name_a_sec_btn), GTK_CHECK_BUTTON(udp->name_d_sec_btn));

	// Setup callback for when primary name buttons are clicked, have to make secondary insensitive
	g_signal_connect(udp->name_a_button, "toggled", G_CALLBACK(alter_secondary_sensitvity), udp);
	g_signal_connect(udp->name_d_button, "toggled", G_CALLBACK(alter_secondary_sensitvity), udp);
	g_signal_connect(udp->result_a_button, "toggled", G_CALLBACK(alter_secondary_sensitvity), udp);
	g_signal_connect(udp->result_d_button, "toggled", G_CALLBACK(alter_secondary_sensitvity), udp);

	// Create window and add title
	GtkWidget *sort_window = gtk_window_new();
	udp->sort_window = sort_window;
	gtk_window_set_title(GTK_WINDOW(sort_window), "Sort Options");

	// Add box to window and show
	gtk_window_set_child(GTK_WINDOW(sort_window), grid);
	gtk_window_present(GTK_WINDOW(sort_window));
}
