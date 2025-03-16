// This file, work-trash.c, is a part of the Entry Dedupe program.
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
#include "work-trash.h"

// Trash on system and remove item
// - Trash, not delete, the selected item (manual or auto)
// - Selected items may be a Directory, Empty, Unique or duplicate (group numbers)
// - After altering file system clear stores and remove any child window

void trash_em (user_data *udp)
{
        // Seed the iterator
        GtkBitsetIter iter;
        guint value = 0;
        gtk_bitset_iter_init_first(&iter, udp->sel_bitset, &value);
        GFile *gf = NULL;

	// Use a spinner to show activity if needed
	guint64 cnt = gtk_bitset_get_size (udp->sel_bitset);
	GtkWidget *spinner = gtk_spinner_new ();
	if (cnt > 1000) {
		gtk_window_set_child(GTK_WINDOW(udp->main_window), spinner);
		gtk_spinner_start ((GtkSpinner *)spinner);
	}


        do {
		do_pending();
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

	if (cnt > 1000) 
		gtk_spinner_stop ((GtkSpinner *)spinner);

	g_object_ref_sink(spinner);
	g_object_unref(spinner);

	// Clean up
        clear_stores(udp); // Clear the stores and associated item memory

	// Reinitial store following clear (errors to stdout otherwise)
	GListStore *list_store = g_list_store_new(G_TYPE_OBJECT);
        udp->list_store = list_store; // Save pointer to list store

        // Remove and collect any existing child
        gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);

        return;
}

// Cancel trash

void trash_cancel_cb (GtkWidget *self, user_data *udp)
{
        gtk_window_close(GTK_WINDOW(udp->trash_prompt_window));
        gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);
}

// Trash the items

void trash_proceed_cb (GtkWidget *self, user_data *udp)
{
        gtk_window_close(GTK_WINDOW(udp->trash_prompt_window));
        trash_em(udp);
        gtk_window_set_child(GTK_WINDOW(udp->main_window), NULL);
}

// Confirm the user wants to trash the duplicates

void prompt_trash (user_data *udp)
{
        // Setup a box for the buttons
        GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

        // Setup buttons
        GtkWidget *trash_proceed = gtk_button_new_with_label("Proceed");
        GtkWidget *trash_cancel = gtk_button_new_with_label("Cancel");

        // Connect buttons to signals
        g_signal_connect(trash_proceed, "clicked", G_CALLBACK(trash_proceed_cb), udp);
        g_signal_connect(trash_cancel, "clicked", G_CALLBACK(trash_cancel_cb), udp);

        // Add buttons to the box
        gtk_box_append(GTK_BOX(box), trash_proceed);
        gtk_box_append(GTK_BOX(box), trash_cancel);

        // Setup the window
        GtkWidget *trash_prompt_window = gtk_window_new();
        udp->trash_prompt_window = trash_prompt_window;
        gtk_window_set_transient_for (GTK_WINDOW(trash_prompt_window), GTK_WINDOW(udp->main_window));
        gtk_window_set_modal (GTK_WINDOW(trash_prompt_window), TRUE);

        gtk_window_set_title(GTK_WINDOW(trash_prompt_window),"Trash ?");
        gtk_window_set_default_size(GTK_WINDOW(trash_prompt_window), 50, 50);
        gtk_window_set_child(GTK_WINDOW(trash_prompt_window), box);

        // Show the window
        gtk_window_present(GTK_WINDOW(trash_prompt_window));
        gtk_widget_grab_focus (trash_cancel);
}
