#include "main.h"
#include "view_file.h"
#include "work_trash.h"
#include "work_selected.h"
#include "show_columns.h"
#include "load_entry_data.h"

void null_selected(user_data *udp)
{
	udp->sel_item = NULL;
	udp->sel_item_position = 0;
}


void view_cb(GtkCheckButton *self, user_data *udp)
{
	// Close action window
	gtk_window_close(GTK_WINDOW(udp->action_window));
		DupItem *item = g_list_model_get_item (G_LIST_MODEL (udp->list_store), udp->sel_item_position);

	// Just view files, not directories
	if (gtk_check_button_get_active(self)) {
		if (strcmp(item->result, "Directory") &&
		    !strstr(item->result, "Error")) {
			view_file(udp);
		} 
		else {
			GtkAlertDialog *alert = gtk_alert_dialog_new("Can't view Directories or files with errors");
			gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		}
	}
	null_selected(udp);
}

// Copy the entry name to the clipboard

void copy_cb(GtkCheckButton *self, user_data *udp)
{
	// Shutdown window
	gtk_window_close(GTK_WINDOW(udp->action_window));

	// Copy to clipboard based on selected position
	if (gtk_check_button_get_active(self)) {
		GdkClipboard *clippy = gtk_widget_get_clipboard(udp->main_window);
		DupItem *item = g_list_model_get_item (G_LIST_MODEL (udp->list_store), udp->sel_item_position);
		gdk_clipboard_set_text(clippy, item->name);
	}
	null_selected(udp);
}

void launch_async_cb (GtkFileLauncher *launcher, GAsyncResult *res, GError **error)
{
        gtk_file_launcher_launch_finish (launcher, res, error);
}

// Launch an application for the selected item
// - Set to always prompt

void launch_cb(GtkCheckButton *self, user_data *udp)
{
	// Shutdown window
	gtk_window_close(GTK_WINDOW(udp->action_window));

	GFile *file = g_file_new_for_path (udp->sel_item->name);
        GtkFileLauncher *file_launcher = gtk_file_launcher_new (file);
        gtk_file_launcher_set_file (file_launcher, file);
        gtk_file_launcher_set_always_ask (file_launcher, TRUE);
	udp->sel_item = NULL;

	gtk_file_launcher_launch (file_launcher, GTK_WINDOW(udp->main_window), NULL, (GAsyncReadyCallback) launch_async_cb, NULL);
	null_selected(udp);
}	

// An entry was selected, show the actions for the item
// - Do select item, don't rely on the position passed

void work_selected_file_cb(GtkSingleSelection *self, uint32_t position, uint32_t n_items, user_data *udp)
{

	// Get the selected position of the selected item (not the position in the current list)
	DupItem *item = gtk_single_selection_get_selected_item (self);
	udp->sel_item = item;

	// Make sure a good selection
	if (!g_list_store_find (udp->list_store, item, &udp->sel_item_position)) { // will return good position if found
		GtkAlertDialog *alert = gtk_alert_dialog_new("Bad Selection");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		null_selected(udp);
		return;
	}

	// Create the check buttons
	GtkWidget *copy_chkb = gtk_check_button_new_with_label("Copy Name to Clipboard");
	GtkWidget *view_chkb = gtk_check_button_new_with_label("View");
	GtkWidget *move_chkb = gtk_check_button_new_with_label("Move to Trash");
	GtkWidget *launch_chkb = gtk_check_button_new_with_label("Launch application");

	// Set the group
	gtk_check_button_set_group(GTK_CHECK_BUTTON(copy_chkb), GTK_CHECK_BUTTON(view_chkb));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(move_chkb), GTK_CHECK_BUTTON(copy_chkb));	
	gtk_check_button_set_group(GTK_CHECK_BUTTON(launch_chkb), GTK_CHECK_BUTTON(copy_chkb));	// Had to circle back to the original copy in 2nd parameter

	// Connect the signals
	g_signal_connect(copy_chkb, "toggled", G_CALLBACK(copy_cb), udp);
	g_signal_connect(view_chkb, "toggled", G_CALLBACK(view_cb), udp);
	g_signal_connect(move_chkb, "toggled", G_CALLBACK(work_trash_cb), udp);
	g_signal_connect(launch_chkb, "toggled", G_CALLBACK(launch_cb), udp);

	// Create the box
	GtkWidget *box_action = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_append(GTK_BOX(box_action), copy_chkb);
	gtk_box_append(GTK_BOX(box_action), view_chkb);
	gtk_box_append(GTK_BOX(box_action), move_chkb);
	gtk_box_append(GTK_BOX(box_action), launch_chkb);

	// Create the window and show
	udp->action_window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(udp->action_window), "Actions");
	gtk_window_set_default_size(GTK_WINDOW(udp->action_window), 512, 256);
	gtk_window_set_child(GTK_WINDOW(udp->action_window), box_action);
	gtk_window_present(GTK_WINDOW(udp->action_window));

	return;
}
