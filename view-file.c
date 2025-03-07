// This file, view-file.c, is a part of the Dedupe Entries program.
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
#include "view-file.h"

// Signal time to stop the string load

gboolean view_file_close_cb (GtkWindow *self, user_data *udp)
{
	udp->cancel_string_load = TRUE;
	return FALSE; // False to allow the default window close to run
}

// Factory setup

static void setup_list_view_cb(GtkSignalListItemFactory *self, GtkListItem *listitem)
{
	GtkWidget *lb = gtk_label_new(NULL);
	gtk_label_set_xalign(GTK_LABEL(lb), 0.0);
	gtk_list_item_set_child(listitem, lb);
}

// Factory bind

static void bind_list_view_cb(GtkSignalListItemFactory *self, GtkListItem *listitem)
{
	GtkWidget *lb = gtk_list_item_get_child(listitem);
	GtkStringObject *strobj = gtk_list_item_get_item(listitem);
	char *markup = g_markup_printf_escaped("<span font_desc='mono'>%s</span>", gtk_string_object_get_string(strobj));
	gtk_label_set_markup(GTK_LABEL(lb), markup);
	g_free(markup);
}

// Make the dump string format
// - Format: offset, space, 2 hex digits followd by space x 16, bar, printable chars, bar
// - Offset is 8 hex digits; 
// - Printable chars are 16 chars or '.' if not printable, with bar at start and end

void make_line_string (unsigned char *input, int len_in, char *output, int offset)
{
	char work[128] = { 0x00 }; // Work 
	memset(output, 0x00, STR_LINE); // Clear output string

	// Output string starts with an offset in hex
	snprintf(work, sizeof(work), "%08x ", offset); // Add offset to the output string
	strncat(output, work, sizeof(output) - (strlen(work) + 1));

	// Looup through input bytes, building output string and printable string
	char printable[STR_PRINTABLE] = { 0x00 }; // Array for single printable char or . w/trailing null
	int i;

	for (i = 0; i < len_in; i++) {
		// Put hex digits in the output string
		snprintf(work, sizeof(work), "%02x ", input[i]);
		strncat(output, work, sizeof(output) - (strlen(work) + 1));

		// Put the printable character in the printable string or a '.'
		if ((input[i] < 0x20) || (input[i] > 0x7e)) {
			strncat(printable, ".", sizeof(printable - 2)); // Add a . if not printable
		}
		else {
			snprintf(work, sizeof(work), "%c", input[i]);
			strncat(printable, work, sizeof(printable) - (strlen(work) + 1)); // Add printable character
		}
	}

	// We may get less then FORMAT UNIT bytes to process if so pad out with spaces 
	while (i < FORMAT_UNIT) {
		strncat(output, "   ", sizeof(output) - 4); // Add three spaces to output string        
		strncat(printable, " ", sizeof(output) - 2); // Add one space to printable string      
		i++; // Increment the index to the input string
	}

	// Format the final output string, adding on the local string for printable characters
	snprintf(work, sizeof(work), "|%s|", printable);
	strncat(output, work, sizeof(output) - (strlen(work) + 1)); // Add the printable string to the output string
}

// Read file and make strings in dump format

void read_and_make_strings (user_data *udp)
{
	// Make the file object for streaming
	GFile *file = g_file_new_for_path(udp->sel_item->name);

	// Setup input stream
	GFileInputStream *in = g_file_read(file, NULL, NULL);
	if (!in) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't read file %s\n", udp->sel_item->name);
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		return;
	}

	// Get buffer to read from file
	unsigned char *read_buff = g_malloc0(READ_BUFF);
	if (!read_buff) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't allocate read buff");
		gtk_alert_dialog_show(alert, GTK_WINDOW(udp->main_window));
		return;
	}

	// Initial read seeding the while
	size_t read = 0;
	read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);

	// Variables for formatting
	int offset = 0;
	unsigned char *format;
	char output[STR_LINE] = { 0x00 };

	// Outer loop fills read buffer from file
	int i;
	while (read != 0) {
		// Inner loop sets up calls for formatting input
		format = read_buff;
		// Get as many groups of 16 as we can
		for (i = 0; i < read; i++) {
			// Check for cancel
			if (udp->cancel_string_load) {
				do_pending();
				goto read_and_make_strings_exit;
			}
			if (i != 0 && !(i % 10)) do_pending();
			// Point the format ptr to start of unit to format
			if (i != 0 && !(i % FORMAT_UNIT)) {
				make_line_string(format, FORMAT_UNIT, output, offset);
				gtk_string_list_append(udp->str_list, output);
				offset += FORMAT_UNIT;
				format = &read_buff[i]; // Point format to current position in read buffer
			}
		}

		// Format any stragglers in the read buffer
		if (read - offset > 0) {
			if (i != 0 && !(i % FORMAT_UNIT)) do_pending();
			make_line_string(format, read - offset, output, offset);
			gtk_string_list_append(udp->str_list, output);
		}

		// Fill read buffer from file
		read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);
	}

	// Clean up     
 read_and_make_strings_exit:
	do_pending();
	g_object_unref(in);
	g_free(read_buff);
	g_object_unref(file);
}

// View file in hex dump format
// - Use a GtkStringList to hold the strings with no selection
// - The dump string format: 8 digit offset, 16 hex digits (2 groups of 8), 16 printable characters

void view_file (user_data *udp)
{
	udp->cancel_string_load = FALSE;

	// Initial string list model
	udp->str_list = gtk_string_list_new(NULL);

	// Don't want to select anything for model
	GtkNoSelection *ns = gtk_no_selection_new(G_LIST_MODEL(udp->str_list));

	// Setup the factory for the list view
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_list_view_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_list_view_cb), NULL);

	// Setup window and scrolled_window for file view
	GtkWidget *fview_window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(fview_window), basename((char *)udp->sel_item->name));
	gtk_window_set_default_size(GTK_WINDOW(fview_window), 896, 256);
	GtkWidget *scrolled_window = gtk_scrolled_window_new();
	gtk_window_set_child(GTK_WINDOW(fview_window), GTK_WIDGET(scrolled_window));

	// Connect signal to close window
	g_signal_connect(fview_window, "close-request", G_CALLBACK(view_file_close_cb), udp);

	// Setup and show the list view with no selection
	GtkWidget *list_view = gtk_list_view_new(GTK_SELECTION_MODEL(ns), factory);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_view);
	gtk_window_present(GTK_WINDOW(fview_window));

	// Read and make strings in dump format
	// - Show the strings in the list view while formatting, don't wait for all to be done
	read_and_make_strings(udp);
}
