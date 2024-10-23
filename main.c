#include "main.h"
#include "sort_columns.h"
#include "filter_columns.h"
#include "work_options.h"
#include "load_entry_data.h"
#include "get_folders.h"

// Provide standard about dialog

void about_cb(GSimpleAction *action, GVariant *parm, user_data *udp)
{
	GtkWidget *dialog = gtk_about_dialog_new();
	const char *author[] = { "Author", "\nDave Hugh", NULL };

	GFile *logo_file = g_file_new_for_path("/home/dhugh/Documents/gimp.logo.png");
	GdkTexture *example_logo = gdk_texture_new_from_file(logo_file, NULL);
	g_object_unref(logo_file);

	gtk_show_about_dialog(NULL,
			      "program-name", "Deduplicate",
			      "logo", example_logo,
			      "title", "About Deduplicate",
			      "authors", author,
			      "version", "Version 1.0",
			      "comments",
			      "A program to find and take action on duplicate files.\nRelies on SHA256 hashes to identify duplicates.",
			      "copyright", "Copyright 2024 David Hugh", "license-type", GTK_LICENSE_GPL_3_0_ONLY, NULL);
}

// Adjust the senstivity of the sort, filter buttons and search bar based on the status of the list store
// - No sort, filter, or search if no list store
// - No search if filtering on

void adjust_sfs_button_sensitivity(user_data *udp)
{
	if ( udp->list_store && g_list_model_get_n_items(G_LIST_MODEL(udp->list_store)) > 0) {
		gtk_widget_set_sensitive (udp->sort_button, TRUE);	
		if (strlen(udp->ep->res_ebt) ||
		    strlen(udp->ep->name_ebt)) {
			gtk_widget_set_sensitive (udp->filter_button, TRUE);	
			gtk_widget_set_sensitive (udp->search_bar, FALSE);	
		}
		else {
			gtk_widget_set_sensitive (udp->filter_button, TRUE);	
			gtk_widget_set_sensitive (udp->search_bar, TRUE);	
		}
	} 
	else {
		gtk_widget_set_sensitive (udp->sort_button, FALSE);		
		gtk_widget_set_sensitive (udp->filter_button, FALSE);	
		gtk_widget_set_sensitive (udp->search_bar, FALSE);	
	}
}

// Initialize saved options
// - Read in saved options from file with fixed name 
// - If no file, then use default options
// - Show options get initialized
// - Calls function to implement filters associated with options

void option_init(user_data *udp)
{
	// Construct file name
	char *name = g_malloc0(STR_NAME);
	strcpy (name, g_get_home_dir());
	strcat (name, "/.config/ddup.cfg"); 
	udp->opt_name = name;

	// Read any saved options in gvariant serialized format
	unsigned char buff[OPTION_SIZE];
	if (read_options (buff, name)) {
		GVariant *value = g_variant_new ("(bbbb)", buff[0], buff[1], buff[2], buff[3]);
		g_variant_get (value, "(bbbb)", &udp->opt_include_empty,
			      &udp->opt_include_directory, &udp->opt_include_duplicate, &udp->opt_include_unique);
		g_variant_unref(value);
	} 
	else {
		udp->opt_include_empty = TRUE;	// Default to show empty entries
		udp->opt_include_directory = TRUE;	// Default to show a directory
		udp->opt_include_duplicate = TRUE;	// Default to show duplicate files
		udp->opt_include_unique = TRUE;	// Default to show unqiue files
	}
}

// Find search entry match in list store
// - Matching based on substring of either result or name
// - Will scroll to first match found to make sure visible

void work_search_entry_cb(GtkEditable *entry, user_data *udp)
{
	if (!udp->list_store) return; // Bug out if no list store
	
	// Get text from entry
        const char *text = gtk_editable_get_text (GTK_EDITABLE(entry));
        if (!strlen(text)) return; // Bug out if no search text

        // Get number of items in list store
        uint32_t cnt = g_list_model_get_n_items(G_LIST_MODEL(udp->list_store));
	if (!cnt) return; // Bug out if list_store is empty

        // Loop through list store and find match
        for (uint32_t i = 0; i < cnt; i++) {
                DupItem *item = g_list_model_get_item(G_LIST_MODEL(udp->list_store), i);
                if (strstr(item->result, text) || strstr(item->name, text)) {
                        gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), i, NULL , GTK_LIST_SCROLL_FOCUS, NULL);
                        return;
                }
        }
}

// Create the main window
// - Buttons get, sort, filter
// - Search bar with entry
// - Hamburger menu with options and about
// - Adjust sensitivity of buttons based on list store status
// - Disallow search if filtering on
 
int main_window(GtkApplication *app, user_data *udp) 
{
	// Initialize options from saved file or use defaults
	option_init(udp);

	// Create main window
	GtkWidget *main_window = gtk_application_window_new(app);
	udp->main_window = main_window;
	gtk_window_set_title(GTK_WINDOW(main_window), "Deduplicate");
	gtk_window_set_default_size(GTK_WINDOW(main_window), 1024, 256);

	// Create header bar to use as titlebar
	GtkWidget *header_bar = gtk_header_bar_new();

	// Setup get, sort, filter, search, restore buttons
	GtkWidget *get_button = gtk_button_new_with_label("Get");
	GtkWidget *sort_button = gtk_button_new_with_label("Sort");
	GtkWidget *filter_button = gtk_button_new_with_label("Filter");

	// Save buttons, will have to adjust sensitivity based on list_store store
	udp->sort_button = sort_button;
	udp->filter_button = filter_button;
	// udp->search_button = search_button;

	// Connect buttons to signals
	g_signal_connect(get_button, "clicked", G_CALLBACK(get_folders_cb), udp);
	g_signal_connect(sort_button, "clicked", G_CALLBACK(get_sort_type_cb), udp);
	g_signal_connect(filter_button, "clicked", G_CALLBACK(get_filters_cb), udp);

	// Create search bar
        GtkWidget *search_bar = gtk_search_bar_new (); 
	udp->search_bar = search_bar;

        // Create search box and add to search bar
        GtkWidget *search_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6); 
        gtk_search_bar_set_child (GTK_SEARCH_BAR (search_bar), search_box);

        // Create search entry and connect to search box
        GtkWidget *entry = gtk_search_entry_new (); 
        gtk_box_append (GTK_BOX (search_box), entry);
        gtk_search_bar_connect_entry (GTK_SEARCH_BAR (search_bar), GTK_EDITABLE (entry)); 

	// Set key capture widget, search mode, and search delay
	gtk_search_bar_set_key_capture_widget (GTK_SEARCH_BAR (search_bar), main_window);
        gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (search_bar), TRUE);
        gtk_search_entry_set_search_delay (GTK_SEARCH_ENTRY (entry), 2000);

	// Connect search entry to search callback (not changed signal)
        g_signal_connect(entry, "search-changed", G_CALLBACK(work_search_entry_cb),udp);

	// Setup hamburger menu button
	GtkWidget *menu_button = gtk_menu_button_new();
	gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menu_button), "open-menu-symbolic");

	// Setup menu for hamburger button
	GMenu *menu = g_menu_new();

	// Add items to the menu
	GMenuItem *options = g_menu_item_new("Options", "app.options");
	GMenuItem *about = g_menu_item_new("About", "app.about");
	g_menu_append_item(menu, options);
	g_menu_append_item(menu, about);

	// Create actions and add to action map
	GSimpleAction *action_options = g_simple_action_new("options", NULL);
	GSimpleAction *action_about = g_simple_action_new("about", NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_options));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_about));

	// Connect signals to menu actions
	g_signal_connect(action_options, "activate", G_CALLBACK(work_options_cb), udp);
	g_signal_connect(action_about, "activate", G_CALLBACK(about_cb), udp);

	// Set the menu model for the menu button
	gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_button), (GMenuModel *) menu);

	// Setup header bar
	gtk_header_bar_pack_start((GtkHeaderBar *) header_bar, get_button);
	gtk_header_bar_pack_start((GtkHeaderBar *) header_bar, sort_button);
	gtk_header_bar_pack_start((GtkHeaderBar *) header_bar, filter_button);
	gtk_header_bar_pack_end((GtkHeaderBar *) header_bar, menu_button);
	gtk_header_bar_pack_end((GtkHeaderBar *) header_bar, search_bar);

	// Connect headerbar to titlebar
	gtk_window_set_titlebar(GTK_WINDOW(main_window), header_bar);

	// Should be sensitive or not depending on list store status
	adjust_sfs_button_sensitivity(udp);

	// Now present the windows
	gtk_window_present(GTK_WINDOW(main_window));
}

void cmd_line_directory_cb(GApplication *app, GFile **files, gint n_files, const char *hint, user_data *udp)
{
	udp->fdpp[0] = g_file_get_path (*files); // Get the path name pointer into the array
	main_window((GtkApplication *)app, udp); // Create the main window 
	load_entry_data(udp); // Load the list store with the folder contents						 
}


// Start application
// - Alloc memory for user data, filters, folders

int main(int argc, char **argv)
{
	// Memory allocations
	user_data *udp = g_malloc0(sizeof(user_data));
	udp->fdpp = g_malloc0(MAX_FOLDERS * sizeof(char *)); // Allocate folder memory
	udp->ep = g_malloc0(sizeof(filter_entry)); // Allocate filter entry memory
	udp->sep = g_malloc0(sizeof(search_entry)); // Allocate search entry memory
	
	// App setup
	GtkApplication *app = gtk_application_new("ddup.gtk.org", G_APPLICATION_DEFAULT_FLAGS);
	udp->app = app;
	g_signal_connect(app, "activate", G_CALLBACK(main_window), udp);
	g_signal_connect(app, "open", G_CALLBACK(cmd_line_directory_cb), udp); // will open directory if valid
	g_application_set_flags (G_APPLICATION(app), G_APPLICATION_HANDLES_OPEN);
	int status = g_application_run(G_APPLICATION(app), argc, argv);

	// Clean up
	g_object_unref(app);
	if (udp->opt_name) g_free(udp->opt_name); // Free up search memory
	if (udp->sep) g_free(udp->sep); // Free up search memory
	if (udp->ep) g_free(udp->ep); // Free up filter memory
	if (udp->fdpp) clear_folders(udp->fdpp); // Free up folder memory if allocated
	if (udp) g_free(udp); // Free up main user data memory

	return status;
}
