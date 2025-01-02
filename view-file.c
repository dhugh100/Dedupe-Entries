// This file, view-file.c, is a part of the Dedupe Entries program.
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
#include "view-file.h"

// Factory setup

static void setup_list_view_cb (GtkSignalListItemFactory *self, GtkListItem *listitem)
{
	GtkWidget *lb = gtk_label_new(NULL);
	gtk_label_set_xalign(GTK_LABEL(lb), 0.0);
	gtk_list_item_set_child(listitem, lb);
}

// Factory bind
// - Using mono font 

static void bind_list_view_cb (GtkSignalListItemFactory *self, GtkListItem *listitem, user_data *udp)
{
	GtkWidget *lb = gtk_list_item_get_child(listitem);
	GtkStringObject *strobj = gtk_list_item_get_item(listitem);
	char *markup = g_markup_printf_escaped("<span font_desc='mono'>%s</span>", gtk_string_object_get_string(strobj));
	gtk_label_set_markup(GTK_LABEL(lb), markup);
}

// Make the dump string format
// - Format: offset, space, 1st group of 8, space, 2nd group of 8, space, bar, printable chars, bar
// - Offset is 8 hex digits; group of 8 is 2 hex digits, space, char; 
// - Printable chars are 16 chars or '.' if not printable, with bar at start and end

void make_format (unsigned char *inp, int len_in, char *oup, int offset)
{
	// Output string starts with an offset in hex
	sprintf(oup, "%08x ", offset); // Add offset to the output string
	*(oup += 9); // Increment the pointer to the output string

	// Looup through input bytes, building output string and printable string
	char printable[PRINTABLE_CHAR_SIZE] = { 0x00 }; // Array for single printable char or .
	char *prp = printable; 
	int i;

	for (i = 0; i < len_in; i++) {
		// Insert a space between groups of 8 hex digits
		if ((i > 0) && (i % 8 == 0)) {
			strcat(oup, " "); // Add a space to the output string
			*(oup += 1); // Increment the pointer to the output string 
		}

		// Put hex digits in the output string
		sprintf(oup, "%02x ", inp[i]);
		*(oup += 3); // Increment the pointer to the output buffer

		// Put the printable character in the local string or a .
		if ((inp[i] < 0x20) || (inp[i] > 0x7e)) sprintf(prp, "%c", 0x2e); // Add a . if not printable
		else sprintf(prp, "%c", inp[i]); // Add printable character

		*(prp++); // Increment the pointer to printable string
	}

	// We may get less then PRINTABLE_CHAR_SIZE bytes to process if so pad out with spaces 
	while (i < PRINTABLE_CHAR_SIZE) {

		// Insert an extra space after 8 characters
		if (i % 8 == 0) {
			strcat(oup, " "); // Add a space to the output string
			*(oup += 1); // Increment the pointer to the output buffer
		}

		strcat(oup, "   "); // Add three spaces to output string        
		strcat(prp, " "); // Add one space to printable string      
		*(oup += 3); // Increment the pointer to the output string
		*(prp++); // Increment the pointer to printable string
		i++; // Increment the index to the input string
	}

	// Format the final output string, adding on the local string for printable characters
	sprintf(oup, "%s |%s|", oup, printable);
}

// Read file and make strings in dump format

void read_and_make_strings (GtkStringList *str_list, const char *name, GtkWidget *main_window)
{
	// Make the file object for streaming
	GFile *file = g_file_new_for_path(name);

	// Setup input stream
	GFileInputStream *in = g_file_read(file, NULL, NULL);
	if (!in) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't read file %s\n", name);
		gtk_alert_dialog_show(alert, GTK_WINDOW(main_window));
		return;
	}

	// Get buffer to read from file
	unsigned char *read_buff = g_malloc0(READ_BUFF);
	if (!read_buff) {
		GtkAlertDialog *alert = gtk_alert_dialog_new("Can't allocate read buff");
		gtk_alert_dialog_show(alert, GTK_WINDOW(main_window));
		return;
	}

	// Initial read seeding the while
	size_t read = 0;
	read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);

	// Variables for formatting
	int offset = 0; // Used to introduce offset of line of PRINTABLE_CHAR_SIZE byes in file
	unsigned char format_buff[PRINTABLE_CHAR_SIZE] = { 0x00 }; // Small buffer to pass to format routine
	unsigned char *fbp = format_buff; // Point to small buffer to pass to format routine     
	char output[128] = { 0x00 }; // Output buffer used by format routine

	// Outer loop fills read buffer from file
	while (read != 0) {
		// Inner loop formats byte in read buffer
		for (int i = 0; i < read; i++) {
			// Get the read bytes into the small buffer used by the formatting routine
			if (i != 0 && !(i % PRINTABLE_CHAR_SIZE)) {
				make_format(format_buff, PRINTABLE_CHAR_SIZE, output, offset); // Get output filled with dump string
				gtk_string_list_append(str_list, output); // Add output to string list
				offset += PRINTABLE_CHAR_SIZE; // Increment offset for next line
				fbp = format_buff; // Reset ptr to start of input buffer
			}
			*fbp = read_buff[i]; // Fill input buffer to pass for format
			*(fbp += 1); // Increment unsigned char pointer to buffer
		}

		// Format any bytes not yet formatted in for loop (< PRINTABLE_CHAR_SIZE)
		if (fbp - format_buff) {
			make_format(format_buff, fbp - format_buff, output, offset);
			gtk_string_list_append(str_list, output);
			offset += PRINTABLE_CHAR_SIZE;
		}

		// Fill read buffer from file
		read = g_input_stream_read(G_INPUT_STREAM(in), read_buff, READ_BUFF, NULL, NULL);
	}

	// Clean up     
	g_object_unref(in);
	g_free(read_buff);
}

// View file in hex dump format
// - Use a GtkStringList to hold the strings with no selection
// - The dump string format: 8 digit offset, 16 hex digits (2 groups of 8), 16 printable characters

void view_file (DupItem *item, GtkWidget *main_window)
{
	// Initial string list model
	GtkStringList *str_list = gtk_string_list_new(NULL);

	// Don't want to select anything for model
	GtkNoSelection *ns = gtk_no_selection_new(G_LIST_MODEL(str_list));

	// Fill up the string list with strings in dump format
	read_and_make_strings(str_list, item->name, main_window);

	// Setup the factory for the list view
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_list_view_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_list_view_cb), NULL);

	// Setup window and scrolled_window for file view
	GtkWidget *fview_window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(fview_window), basename((char *)item->name));
	gtk_window_set_default_size(GTK_WINDOW(fview_window), 896, 256);
	GtkWidget *scrolled_window = gtk_scrolled_window_new();
	gtk_window_set_child(GTK_WINDOW(fview_window), scrolled_window);

	// Setup and show the list view with no selection
	GtkWidget *list_view = gtk_list_view_new(GTK_SELECTION_MODEL(ns), factory);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_view);
	gtk_window_present(GTK_WINDOW(fview_window));
}
