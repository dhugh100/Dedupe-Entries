/*

 ddup - a GTK program to find and take action on duplicate files
 Copyright (C) 2024  David Hugh

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https:www.gnu.org/licenses/>.

*/
#include "main.h"
#include "filter_columns.h"

// Set sensitivity of clear button - gray out if no text is in either entry buff text

void set_sensitivity (filter_entry *ep)
{
	if (strlen(ep->res_ebt) > 0 ||
	    strlen(ep->name_ebt) > 0)
		gtk_widget_set_sensitive (ep->clear_btn, TRUE);
	else gtk_widget_set_sensitive (ep->clear_btn, FALSE);
}	

// Filter the list based on the text in the entry buffers
// - Comparison function associated with the model
// - Invoked when establishing model, need to allocate early
// - And/or,  result yes/no, name yes/no -- 2^3 (8) possibilities

gboolean subres(DupItem *item, user_data *udp)
{
	if (udp->ep->res_n) {
		if (strstr(item->result, udp->ep->res_ebt))
			return FALSE;
		else
			return TRUE;
	}
	else {
		if (strstr(item->result, udp->ep->res_ebt))
			return TRUE;
		else
			return FALSE;
	}
}

gboolean subname(DupItem *item, user_data *udp)
{
	if (udp->ep->name_n) {
		if (strstr(item->name, udp->ep->name_ebt))
			return FALSE;
		else
			return TRUE;
	}
	else {
		if (strstr(item->name, udp->ep->name_ebt))
			return TRUE;
		else
			return FALSE;
	}
}	

gboolean filter_match(DupItem *item, user_data *udp)
{
	gboolean state =  (udp->ep->and && subres(item, udp) && subname(item, udp) ? TRUE : FALSE) || // T T T = T
			 (udp->ep->and && subres(item, udp) && !subname(item, udp) ? FALSE : FALSE) || // T T F = F
			 (udp->ep->and && !subres(item, udp) && subname(item, udp) ? FALSE : FALSE) || // T F T = F
			 (udp->ep->and && !subres(item, udp) && !subname(item, udp) ? FALSE : FALSE) || // T F F = F
			 (!udp->ep->and && subres(item, udp) && subname(item, udp) ? TRUE : FALSE) || // F T T = T
			 (!udp->ep->and && subres(item, udp) && !subname(item, udp) ? TRUE : FALSE) || // F T F = T
			 (!udp->ep->and && !subres(item, udp) && subname(item, udp) ? TRUE : FALSE) || // F F T = T
			 (!udp->ep->and && !subres(item, udp) && !subname(item, udp) ? FALSE : FALSE);  // F F F = F
												       
	return state;

}

// Filter the list based on the text in the entry buffers

void apply_filters_cb(GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->filter_window));
	gtk_filter_changed ((GtkFilter *)udp->custom_filter, GTK_FILTER_CHANGE_DIFFERENT);
	set_sensitivity(udp->ep);
	adjust_sfs_button_sensitivity(udp);
}

// Callback to get text from the name entry buffer

void name_buff_cb(GtkWidget *self, user_data *udp)
{
        const gchar *text = gtk_entry_buffer_get_text(udp->ep->name_eb);
        strcpy(udp->ep->name_ebt, text);
        printf("Text2: %s\n", text);
}

// Callback to get the text from the result entry buffer

void result_buff_cb(GtkWidget *self, user_data *udp)
{
        const gchar *text = gtk_entry_buffer_get_text(udp->ep->res_eb);
        strcpy(udp->ep->res_ebt, text);
        printf("Text1: %s\n", text);
}

// Set the logic and radio button to selected or unselected

void and_cb(GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self))
		udp->ep->and = TRUE;
	else
		udp->ep->and = FALSE;
}

// Set the logic or radio button to selected or unselected

void or_cb(GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self))
		udp->ep->or = TRUE;
	else
		udp->ep->or = FALSE;
}

// Set the result not radio button to selected or unselected

void res_n_cb(GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self))
		udp->ep->res_n = TRUE;
	else
		udp->ep->res_n = FALSE;
}

// Set the name not radio button to selected or unselected
void name_n_cb(GtkCheckButton *self, user_data *udp)
{
	if (gtk_check_button_get_active(self))
		udp->ep->name_n = TRUE;
	else
		udp->ep->name_n = FALSE;
}	

// Null the entry to clear the buffers, reset clear button sensitivity

void clear_filters_cb(GtkWidget *self, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->filter_window));
	gtk_entry_buffer_set_text(udp->ep->res_eb, "", -1);
	gtk_entry_buffer_set_text(udp->ep->name_eb, "", -1);

	gtk_filter_changed ((GtkFilter *)udp->custom_filter, GTK_FILTER_CHANGE_DIFFERENT);
	set_sensitivity(udp->ep);
	adjust_sfs_button_sensitivity(udp);
}

// Prompt filter choice

void get_filters_cb(GtkWidget *self, user_data *udp)
{
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
	gtk_widget_set_margin_top (GTK_WIDGET(grid), 10);
	gtk_widget_set_margin_bottom (GTK_WIDGET(grid), 10);


	// Makes labels for entry widgets
	GtkWidget *result_label = gtk_label_new ("Result:");
	GtkWidget *name_label = gtk_label_new ("Name:");

	// Make labels for sections
	GtkWidget *logic_label = gtk_label_new ("LOGIC:");

        // Setup Check Boxes
        GtkWidget *and_chk_box = gtk_check_button_new_with_label ("AND");
        GtkWidget *or_chk_box = gtk_check_button_new_with_label ("OR");
        GtkWidget *res_n_chk_box = gtk_check_button_new_with_label ("Not");
        GtkWidget *name_n_chk_box = gtk_check_button_new_with_label ("Not");
	
	gtk_check_button_set_group (GTK_CHECK_BUTTON (and_chk_box), GTK_CHECK_BUTTON (or_chk_box));

	// Set check boxes to active or not
	// - If neither and/or logic selected earlier, force and as default

	if ( !udp->ep->and &&
	     !udp->ep->or ) 
		udp->ep->and = TRUE;		

	// Have check boxes reflect stored state
	if (udp->ep->and) {
		gtk_check_button_set_active((GtkCheckButton *) and_chk_box, TRUE);
		gtk_check_button_set_active((GtkCheckButton *) or_chk_box, FALSE);
	}	
	else {
		gtk_check_button_set_active((GtkCheckButton *) and_chk_box, FALSE);
		gtk_check_button_set_active((GtkCheckButton *) or_chk_box, TRUE);
	}	

	if (udp->ep->res_n) gtk_check_button_set_active((GtkCheckButton *) res_n_chk_box, TRUE);
	else gtk_check_button_set_active((GtkCheckButton *) res_n_chk_box, FALSE);

	if (udp->ep->name_n) gtk_check_button_set_active((GtkCheckButton *) name_n_chk_box, TRUE);
	else gtk_check_button_set_active((GtkCheckButton *) name_n_chk_box, FALSE);

	// Setup result and entry buffs
	// - Carry over into entry buffs any prior, uncleared filter
	udp->ep->res_eb = gtk_entry_buffer_new((const char *)udp->ep->res_ebt, strlen(udp->ep->res_ebt));
        udp->ep->res_e = gtk_entry_new_with_buffer(udp->ep->res_eb);

        udp->ep->name_eb = gtk_entry_buffer_new((const char *)udp->ep->name_ebt,strlen(udp->ep->name_ebt));
        udp->ep->name_e = gtk_entry_new_with_buffer(udp->ep->name_eb);

	// Apply Button
	GtkWidget *apply_btn = gtk_button_new_with_label ("Apply");
	gtk_widget_set_halign(apply_btn, GTK_ALIGN_CENTER);

	// Clear Button
	// - Need to pass around functions to adjust sensitivity
	udp->ep->clear_btn = gtk_button_new_with_label ("Clear");
	gtk_widget_set_halign(udp->ep->clear_btn, GTK_ALIGN_CENTER);

	// Grid attach by column, row, width, height
	
	// Row 1 
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(result_label), 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(udp->ep->res_e), 2, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(res_n_chk_box), 3, 1, 1, 1);

	// Row 2
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(name_label), 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(udp->ep->name_e), 2, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(name_n_chk_box), 3, 2, 1, 1);

	// Row 3
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(logic_label), 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(and_chk_box), 2, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(or_chk_box), 3, 3, 1, 1);

	// Row 5
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(apply_btn),   1, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(udp->ep->clear_btn),   3, 5, 1, 1);


	// Connect signals
	g_signal_connect(udp->ep->res_e, "changed", G_CALLBACK(result_buff_cb), udp);
	g_signal_connect(udp->ep->name_e, "changed", G_CALLBACK(name_buff_cb), udp);
	g_signal_connect(apply_btn, "clicked", G_CALLBACK(apply_filters_cb), udp);
	g_signal_connect(udp->ep->clear_btn, "clicked", G_CALLBACK(clear_filters_cb), udp);
	g_signal_connect(and_chk_box, "toggled", G_CALLBACK(and_cb), udp);
	g_signal_connect(or_chk_box, "toggled", G_CALLBACK(or_cb), udp);
	g_signal_connect(res_n_chk_box, "toggled", G_CALLBACK(res_n_cb), udp);
	g_signal_connect(name_n_chk_box, "toggled", G_CALLBACK(name_n_cb), udp);

	set_sensitivity(udp->ep);

	// Add grid to window and show
	gtk_window_set_child(GTK_WINDOW(filter_window), grid);
	gtk_window_present(GTK_WINDOW(filter_window));
}
