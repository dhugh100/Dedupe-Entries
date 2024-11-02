#include "main.h"
#include "work_selected.h"
#include "filter_columns.h"
#include "show_columns.h"

// Right btn pressed
void right_btn_pressed(user_data udp) 
{
	printf("right button pressed\n");
}

// Right btn released
void right_btn_released(user_data udp) 
{
	printf("right button released\n");
}

// Return ptr to result
static const char* dup_item_get_result (DupItem *item) {
	return item->result;
}

// Return ptr to name
static const char* dup_item_get_name (DupItem *item) {
	return item->name;
}

// 

// Setup the label for the cell, set alignment to left
static void setup_cb (GtkSignalListItemFactory *factory,  GObject  *listitem) {
	GtkWidget *label = gtk_label_new (NULL);
	gtk_label_set_xalign (GTK_LABEL (label), 0.0);
	gtk_list_item_set_child (GTK_LIST_ITEM (listitem), label);

}

// For the factory, get the result for the item into the label
// - Bind_property is used to bind the result to the label as may make changes after initial setup
static void bind_result_cb (GtkSignalListItemFactory *factory, GtkListItem *listitem, user_data *udp) {
	GtkWidget *label = gtk_list_item_get_child (listitem);
	GObject *item = gtk_list_item_get_item (GTK_LIST_ITEM (listitem));
	GBinding *prop_bind = g_object_bind_property (item, "result", label, "label", G_BINDING_SYNC_CREATE);
}

// For the factory, get the fullname for the item into the label
static void bind_name_cb (GtkSignalListItemFactory *factory, GtkListItem *listitem, user_data *udp) {
	GtkWidget *label = gtk_list_item_get_child (listitem);
	GObject *item = gtk_list_item_get_item (GTK_LIST_ITEM (listitem));
	Gobject *citem = gtk_column_view_calllist_item_get_item (GTK_LIST_ITEM (listitem)
	const char *string = dup_item_get_name (DUP_ITEM (item));
	// const gboolean should_highlight = filter_match (udp, string);
	gtk_label_set_text (GTK_LABEL (label), string);
}

// Show the column view in the main window
void show_columns (user_data *udp) {

        // Remove and collect any existing child
        gtk_window_set_child(GTK_WINDOW (udp->main_window), NULL);

	// Setup the scrolled window for the column view
	GtkWidget *scrolled_window = gtk_scrolled_window_new ();
	gtk_window_set_child (GTK_WINDOW (udp->main_window), scrolled_window);

	// Setup the column view in the scrolled window with separators
	GtkWidget *column_view = gtk_column_view_new (NULL); // Need for columns
	udp->column_view = column_view;							     
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrolled_window), column_view);
	gtk_column_view_set_show_column_separators (GTK_COLUMN_VIEW (column_view), TRUE);

	// Setup the factory for result column
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
	g_signal_connect (factory, "setup", G_CALLBACK (setup_cb), NULL);
	g_signal_connect (factory, "bind", G_CALLBACK (bind_result_cb), udp);
	GtkColumnViewColumn *column = gtk_column_view_column_new ("Result", factory);
	gtk_column_view_append_column (GTK_COLUMN_VIEW (column_view), column);
	g_object_unref (column);
  
	// Setup the factory for name column
	factory = gtk_signal_list_item_factory_new ();
	g_signal_connect (factory, "setup", G_CALLBACK (setup_cb), NULL);
	g_signal_connect (factory, "bind", G_CALLBACK (bind_name_cb), udp);
	column = gtk_column_view_column_new ("Name", factory);
	gtk_column_view_append_column (GTK_COLUMN_VIEW (column_view), column);
	g_object_unref (column);

        // Setup the filter model	
	GtkFilterListModel *filter = gtk_filter_list_model_new (G_LIST_MODEL(udp->list_store), NULL);	
        udp->filter = filter;
        GtkCustomFilter *custom_filter = gtk_custom_filter_new((GtkCustomFilterFunc)filter_match, udp, NULL);
        udp->custom_filter = custom_filter;
        gtk_filter_list_model_set_filter (filter, GTK_FILTER(custom_filter));

	// Setup to allow multiple selections
	GtkMultiSelection *selection = gtk_multi_selection_new (G_LIST_MODEL (filter));
	udp->selection = selection;
 
	// Associate selection model with column view 
	gtk_column_view_set_model (GTK_COLUMN_VIEW (column_view), GTK_SELECTION_MODEL (selection));

	// Setup the right button gesture event, will be used to select action on right button from column view
	GtkGesture *gesture = gtk_gesture_click_new (); 
	gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 3); // Right button
	g_signal_connect (gesture, "released",  G_CALLBACK (work_selected_file_cb), udp);
	gtk_widget_add_controller (column_view, GTK_EVENT_CONTROLLER (gesture));

	gtk_window_present (GTK_WINDOW (udp->main_window));
  
	// Need to unselect first item as by default is selected
	if (gtk_selection_model_is_selected ((GtkSelectionModel *) selection, 0)) { 
		gtk_selection_model_unselect_item ((GtkSelectionModel *) selection, 0);
	} 
}
