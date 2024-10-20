#include "main.h"
#include "sort_columns.h"

// Comparsion function for name descending
int cmp_name_d(const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *)a;
	DupItem *item2 = (DupItem *)b;
	return(strcmp(item2->name, item1->name));
}

// Sort name descending
void name_d_cb(GtkCheckButton *button, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->option_window));
	g_list_store_sort (udp->list_store, (GCompareDataFunc)cmp_name_d, udp);
	gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), 1, NULL, GTK_LIST_SCROLL_NONE, NULL);
}

// Comparison function for sort of name ascending
int cmp_name_a(const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *)a;
	DupItem *item2 = (DupItem *)b;
	return strcmp(item1->name, item2->name);
}

// Sort name ascending
void name_a_cb(GtkCheckButton *button, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->option_window));
	g_list_store_sort (udp->list_store, (GCompareDataFunc)cmp_name_a, udp);
	gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), 1, NULL, GTK_LIST_SCROLL_NONE, NULL);
}

// Comparison function for sort of result descending
int cmp_result_d(const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *)a;
	DupItem *item2 = (DupItem *)b;
	return strcmp(item2->result, item1->result);
}
// Sort result descending
void result_d_cb(GtkCheckButton *button, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->option_window));
	g_list_store_sort (udp->list_store, (GCompareDataFunc)cmp_result_d, udp);
	gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), 1, NULL, GTK_LIST_SCROLL_NONE, NULL);
}

// Comparison function for sort of result ascending
int cmp_result_a(const void *a, const void *b, user_data *udp)
{
	DupItem *item1 = (DupItem *)a;
	DupItem *item2 = (DupItem *)b;
	return strcmp(item1->result, item2->result);
}

// Sort result ascending
void result_a_cb(GtkCheckButton *button, user_data *udp)
{
	gtk_window_close(GTK_WINDOW(udp->option_window));
	g_list_store_sort (udp->list_store, (GCompareDataFunc)cmp_result_a, udp);
	gtk_column_view_scroll_to(GTK_COLUMN_VIEW(udp->column_view), 1, NULL, GTK_LIST_SCROLL_NONE, NULL);
}

// Prompt the user to choose proceed or cancel
void get_sort_type_cb (GtkWidget *self, user_data *udp)
{
        // Create check buttons
        GtkWidget *result_a_button = gtk_check_button_new_with_label("Result Ascending");
        GtkWidget *result_d_button = gtk_check_button_new_with_label("Result Descending");
        GtkWidget *name_a_button = gtk_check_button_new_with_label("Name Ascending");
        GtkWidget *name_d_button = gtk_check_button_new_with_label("Name Descending");

	// Setup callbacks
        g_signal_connect(result_a_button, "toggled", G_CALLBACK(result_a_cb), udp);
        g_signal_connect(result_d_button, "toggled", G_CALLBACK(result_d_cb), udp);
        g_signal_connect(name_a_button, "toggled", G_CALLBACK(name_a_cb), udp);
        g_signal_connect(name_d_button, "toggled", G_CALLBACK(name_d_cb), udp);

        // Create box and add check buttons
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_append(GTK_BOX(box), result_a_button);
        gtk_box_append(GTK_BOX(box), result_d_button);
        gtk_box_append(GTK_BOX(box), name_a_button);
        gtk_box_append(GTK_BOX(box), name_d_button);

       // Set the group
       gtk_check_button_set_group(GTK_CHECK_BUTTON(result_a_button), GTK_CHECK_BUTTON(result_d_button));
       gtk_check_button_set_group(GTK_CHECK_BUTTON(name_a_button), GTK_CHECK_BUTTON(result_a_button));
       gtk_check_button_set_group(GTK_CHECK_BUTTON(name_d_button), GTK_CHECK_BUTTON(result_a_button));

        // Create window and add title
       GtkWidget *option_window = gtk_window_new();
       udp->option_window = option_window;
       gtk_window_set_title(GTK_WINDOW(option_window), "Sort Options");

        // Add box to window and show
        gtk_window_set_child(GTK_WINDOW(option_window), box);
        gtk_window_present(GTK_WINDOW(option_window));
}
