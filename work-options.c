// This file, work_options.c, is a part of the Dedupe Entries program.
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
#include "work-options.h"

// Apply the option change via reload of entry data

void apply_cb(GtkCheckButton *self, user_data *udp)
{
	if (udp->opt_changed) {
		load_entry_data(udp);
		udp->opt_changed = FALSE;
	}	
        gtk_window_close (GTK_WINDOW (udp->option_window));
        gtk_window_set_child (GTK_WINDOW (udp->main_window), NULL);
}

// Save the option change to a file

void save_cb(GtkCheckButton *self, user_data *udp)
{
        // Setup file and stream for writing
        GFile *file = g_file_new_for_path (udp->opt_name);
        GFileOutputStream *out = g_file_replace (file, NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL);

        // Creat variant from current values
        GVariant *value = g_variant_new ("(bbbbi)", udp->opt_include_empty, udp->opt_include_directory, udp->opt_include_duplicate, udp->opt_include_unique, udp->opt_preserve);

        // Serialize for writing
        int sz = g_variant_get_size (value);
        unsigned char data[sz];
        g_variant_store (value, data);

        // Write in one fell swoop
        gsize wrote;
        gboolean result = g_output_stream_write_all (G_OUTPUT_STREAM (out), data, sz, &wrote, NULL, NULL);

	// Clean up
	g_variant_unref (value);
       	g_output_stream_close (G_OUTPUT_STREAM (out), NULL, NULL);
	g_object_unref (out);
	g_object_unref (file);

	if (!result) {
	        GtkAlertDialog *alert = gtk_alert_dialog_new ("Problem writing file");
        	gtk_alert_dialog_show (alert, GTK_WINDOW (udp->main_window));
        }

	gtk_window_close (GTK_WINDOW (udp->option_window));  
        gtk_window_set_child (GTK_WINDOW (udp->main_window), NULL);
}

// Callback for unique option

void unique_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	if (gtk_check_button_get_active(self))
		udp->opt_include_unique = TRUE;
	else
		udp->opt_include_unique = FALSE;
}

// Callback for empty option

void empty_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	if (gtk_check_button_get_active(self))
		udp->opt_include_empty = TRUE;
	else
		udp->opt_include_empty = FALSE;
}

// Callback for duplicate option

void duplicate_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	if (gtk_check_button_get_active(self))
		udp->opt_include_duplicate = TRUE;
	else
		udp->opt_include_duplicate = FALSE;
}

// Callback for directory option

void directory_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	if (gtk_check_button_get_active(self))
		udp->opt_include_directory = TRUE;
	else
		udp->opt_include_directory = FALSE;
}

// Callback for auto preserve options

void mod_first_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	udp->opt_preserve = AP_MOD_FIRST;
}

// Callback for auto preserve options

void mod_last_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	udp->opt_preserve = AP_MOD_LAST;
}
// Callback for auto preserve options

void shortest_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	udp->opt_preserve = AP_SHORTEST;
}

// Callback for auto preserve options

void longest_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	udp->opt_preserve = AP_LONGEST;
}

// Callback for auto preserve options

void a_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	udp->opt_preserve = AP_ASCENDING;
}
// Callback for auto preserve options

void d_cb(GtkCheckButton *self, user_data *udp)
{
	udp->opt_changed = TRUE;
	udp->opt_preserve = AP_DESCENDING;
}

// Display the options window

void work_options_cb(GSimpleAction *self, GVariant *parm, user_data *udp)
{
	// Create labels
	GtkWidget *include = gtk_label_new("Include Options");
	gtk_label_set_xalign(GTK_LABEL(include), 0.0);
	GtkWidget *auto_preserve = gtk_label_new("\nAuto Preserve Options");
	gtk_label_set_xalign(GTK_LABEL(auto_preserve), 0.0);

	// Create check buttons
	GtkWidget *empty = gtk_check_button_new_with_label("Empty files");
	GtkWidget *directory = gtk_check_button_new_with_label("Directories");
	GtkWidget *duplicate = gtk_check_button_new_with_label("Duplicate files");
	GtkWidget *unique = gtk_check_button_new_with_label("Unique files");
	GtkWidget *mod_first = gtk_check_button_new_with_label("First Modified");
	GtkWidget *mod_last = gtk_check_button_new_with_label("Last Modified");
	GtkWidget *shortest = gtk_check_button_new_with_label("Shortest Name");
	GtkWidget *longest = gtk_check_button_new_with_label("Longest Name");
	GtkWidget *a = gtk_check_button_new_with_label("First Name Ascending");
	GtkWidget *d = gtk_check_button_new_with_label("First Name Descending");

	// Set button status based on current values in memory
	if (udp->opt_include_empty)
		gtk_check_button_set_active((GtkCheckButton *) empty, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) empty, FALSE);

	if (udp->opt_include_unique)
		gtk_check_button_set_active((GtkCheckButton *) unique, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) unique, FALSE);

	if (udp->opt_include_duplicate)
		gtk_check_button_set_active((GtkCheckButton *) duplicate, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) duplicate, FALSE);

	if (udp->opt_include_directory)
		gtk_check_button_set_active((GtkCheckButton *) directory, TRUE);
	else
		gtk_check_button_set_active((GtkCheckButton *) directory, FALSE);

	if (udp->opt_preserve == AP_MOD_FIRST)
		gtk_check_button_set_active((GtkCheckButton *) mod_first, TRUE);
	else if (udp->opt_preserve == AP_MOD_LAST)
		gtk_check_button_set_active((GtkCheckButton *) mod_last, TRUE);
	else if (udp->opt_preserve == AP_SHORTEST)
		gtk_check_button_set_active((GtkCheckButton *) shortest, TRUE);
	else if (udp->opt_preserve == AP_LONGEST)
		gtk_check_button_set_active((GtkCheckButton *) longest, TRUE);
	else if (udp->opt_preserve == AP_ASCENDING)
		gtk_check_button_set_active((GtkCheckButton *) a, TRUE);
	else if (udp->opt_preserve == AP_DESCENDING)
		gtk_check_button_set_active((GtkCheckButton *) d, TRUE);

	// Create check button group for auto preserve options
	gtk_check_button_set_group(GTK_CHECK_BUTTON(mod_first), GTK_CHECK_BUTTON(mod_last));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(shortest), GTK_CHECK_BUTTON(mod_first));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(longest), GTK_CHECK_BUTTON(mod_first));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(a), GTK_CHECK_BUTTON(mod_first));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(d), GTK_CHECK_BUTTON(mod_first));


	// Setup callbacks
	g_signal_connect(empty, "toggled", G_CALLBACK(empty_cb), udp);
	g_signal_connect(unique, "toggled", G_CALLBACK(unique_cb), udp);
	g_signal_connect(duplicate, "toggled", G_CALLBACK(duplicate_cb), udp);
	g_signal_connect(directory, "toggled", G_CALLBACK(directory_cb), udp);
	g_signal_connect(mod_last, "toggled", G_CALLBACK(mod_last_cb), udp);
	g_signal_connect(mod_first, "toggled", G_CALLBACK(mod_first_cb), udp);
	g_signal_connect(shortest, "toggled", G_CALLBACK(shortest_cb), udp);
	g_signal_connect(longest, "toggled", G_CALLBACK(longest_cb), udp);
	g_signal_connect(a, "toggled", G_CALLBACK(a_cb), udp);
	g_signal_connect(d, "toggled", G_CALLBACK(d_cb), udp);

	// Create box and add check buttons
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_append(GTK_BOX(box), include);
	gtk_box_append(GTK_BOX(box), empty);
	gtk_box_append(GTK_BOX(box), directory);
	gtk_box_append(GTK_BOX(box), duplicate);
	gtk_box_append(GTK_BOX(box), unique);
	gtk_box_append(GTK_BOX(box), auto_preserve);
	gtk_box_append(GTK_BOX(box), mod_first);
	gtk_box_append(GTK_BOX(box), mod_last);
	gtk_box_append(GTK_BOX(box), shortest);
	gtk_box_append(GTK_BOX(box), longest);
	gtk_box_append(GTK_BOX(box), a);
	gtk_box_append(GTK_BOX(box), d);

	// Create window and add title
	udp->option_window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(udp->option_window), "Options");

	// Create header bar to use as titlebar, include save button
	GtkWidget *header = gtk_header_bar_new();

	GtkWidget *save = gtk_button_new_with_label("Save");
	g_signal_connect(save, "clicked", G_CALLBACK(save_cb), udp);

	GtkWidget *reshow = gtk_button_new_with_label("Apply");
	g_signal_connect(reshow, "clicked", G_CALLBACK(apply_cb), udp);

	gtk_header_bar_pack_start((GtkHeaderBar *) header, save);
	gtk_header_bar_pack_end((GtkHeaderBar *) header, reshow);
	gtk_window_set_titlebar(GTK_WINDOW(udp->option_window), header);

	// Add box to window and show
	gtk_window_set_child(GTK_WINDOW(udp->option_window), box);
	gtk_window_present(GTK_WINDOW(udp->option_window));
}
