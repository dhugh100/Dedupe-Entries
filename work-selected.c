// This file, work_selected.c, is a part of the Dedupe Entries program.
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
#include "view-file.h"
#include "work-trash.h"
#include "lib.h"
#include "work-selected.h"

// View the selected file in ascii dump format
// - Only view files, not directories or files with errors
// - Only invoked when one item is selected

void view_cb (GtkCheckButton *self, user_data *udp)
{
	// Close action window
	gtk_window_close(GTK_WINDOW(udp->action_window));

	// Get the selected item
	udp->sel_item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), gtk_bitset_get_minimum(udp->sel_bitset));

	// Just view files
	if (gtk_check_button_get_active(self)) {
		if (strcmp(udp->sel_item->result, STR_DIR) || strncmp(udp->sel_item->result, STR_ERR, 5)) {
			view_file(udp);
		}
		else {
			GtkAlertDialog *alert = gtk_alert_dialog_new("Can't view Directories or files with errors");
			gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		}
	}
	wipe_selected(udp);
}

// Copy the selected entry(s) data to the clipboard
// - Don't allow text to exceed clipboard size
// - Could be one or more selected entries

void copy_cb (GtkCheckButton *self, user_data *udp)
{
	// Shutdown window
	gtk_window_close(GTK_WINDOW(udp->action_window));

	GdkClipboard *clippy = gtk_widget_get_clipboard(udp->main_window);
	udp->clippy = clippy;

	// Easy case is just 1
	if (gtk_bitset_get_size(udp->sel_bitset) == 1) {
		DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store),
						      gtk_bitset_get_minimum(udp->sel_bitset));
		gdk_clipboard_set_text(clippy, item->name);
		wipe_selected(udp);
		return;
	}

	// Must have multiple items, so loop through the bitset

	// Seed the iterator
	GtkBitsetIter iter;
	guint value = 0;
	gtk_bitset_iter_init_first(&iter, udp->sel_bitset, &value);

	// Start the text to copy
	char *clip_text = g_malloc0(STR_CLIP);
	char *work = g_malloc0(STR_PATH);
	DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), value);
	sprintf(clip_text, "%s\n", item->name);

	// Loop through the bitset
	while (gtk_bitset_iter_next(&iter, &value)) {
		value = gtk_bitset_iter_get_value(&iter);
		item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), value); // Get the selected item

		sprintf(work, "%s\n", item->name); // Next name to add

		// Don't overrun size of text buffer for clipboard
		if ((strlen(clip_text) + strlen(work) + 2) > STR_CLIP) {
			GtkAlertDialog *alert = gtk_alert_dialog_new("Truncating Text for Clipboard");
			gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
			break;
		}

		strcat(clip_text, work);
	}

	// Set the clipboard
	gdk_clipboard_set_text(clippy, clip_text);

	g_free(clip_text);
	g_free(work);
	wipe_selected(udp);

	return;
}

// Finish the launch for the selected item

void launch_async_cb (GtkFileLauncher *launcher, GAsyncResult *res, GError **error)
{
	gtk_file_launcher_launch_finish(launcher, res, error);
}

// Launch an application for the selected item
// - Set to always prompt for the application
// - Only invoked when one item is selected

void launch_cb (GtkCheckButton *self, user_data *udp)
{
	// Shutdown window
	gtk_window_close(GTK_WINDOW(udp->action_window));

	// Get the selected item
	DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store),
					      gtk_bitset_get_minimum(udp->sel_bitset));

	// Launch the application
	GFile *file = g_file_new_for_path(item->name);
	GtkFileLauncher *file_launcher = gtk_file_launcher_new(file);
	gtk_file_launcher_set_file(file_launcher, file);
	g_object_unref(file);
	gtk_file_launcher_set_always_ask(file_launcher, TRUE);
	gtk_file_launcher_launch(file_launcher, GTK_WINDOW(udp->main_window), NULL, (GAsyncReadyCallback) launch_async_cb, NULL);

	wipe_selected(udp);
	g_object_unref(file_launcher);
}

// An entry was selected, show the actions for the item

void work_selected_file_cb (GtkGestureClick *self, int n_press, double x, double y, user_data *udp)
{
	GtkWidget *box_action;

	GtkBitset *sel_bitset = gtk_selection_model_get_selection((GtkSelectionModel *) udp->selection);
	udp->sel_bitset = sel_bitset;
	guint64 sel_bs_size = gtk_bitset_get_size(sel_bitset);

	if (sel_bs_size == 0) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("No Selection");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		return;
	}

	// If one item selected, offer copy, view, trash, launch
	// If more then one item, just offer copy and trash

	// Items common to both single and multi
	udp->action_window = gtk_window_new();

	GtkWidget *copy_chkb = gtk_check_button_new_with_label("Copy Name to Clipboard");
	GtkWidget *move_chkb = gtk_check_button_new_with_label("Move to Trash");

	g_signal_connect(copy_chkb, "toggled", G_CALLBACK(copy_cb), udp);
	g_signal_connect(move_chkb, "toggled", G_CALLBACK(work_trash_cb), udp);

	box_action = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_append(GTK_BOX(box_action), copy_chkb);
	gtk_box_append(GTK_BOX(box_action), move_chkb);

	// Create the rest of the single selection action window
	if (sel_bs_size == 1) {
		gtk_window_set_title(GTK_WINDOW(udp->action_window), "Single Selection Actions");

		GtkWidget *view_chkb = gtk_check_button_new_with_label("View");
		GtkWidget *launch_chkb = gtk_check_button_new_with_label("Launch application");

		gtk_check_button_set_group(GTK_CHECK_BUTTON(copy_chkb), GTK_CHECK_BUTTON(view_chkb));
		gtk_check_button_set_group(GTK_CHECK_BUTTON(move_chkb), GTK_CHECK_BUTTON(copy_chkb));
		gtk_check_button_set_group(GTK_CHECK_BUTTON(launch_chkb), GTK_CHECK_BUTTON(copy_chkb)); // Had to circle back to the original copy in 2nd parameter

		g_signal_connect(view_chkb, "toggled", G_CALLBACK(view_cb), udp);
		g_signal_connect(launch_chkb, "toggled", G_CALLBACK(launch_cb), udp);

		gtk_box_append(GTK_BOX(box_action), view_chkb);
		gtk_box_append(GTK_BOX(box_action), launch_chkb);
	}
	else {
		// Finish the mutli selection action window setup
		gtk_window_set_title(GTK_WINDOW(udp->action_window), "Multiple Selection Actions");

		gtk_check_button_set_group(GTK_CHECK_BUTTON(copy_chkb), GTK_CHECK_BUTTON(move_chkb));
	}

	gtk_window_set_default_size(GTK_WINDOW(udp->action_window), 512, 256);
	gtk_window_set_child(GTK_WINDOW(udp->action_window), box_action);
	gtk_window_present(GTK_WINDOW(udp->action_window));

	return;
}
