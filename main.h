#ifndef main_h
#define main_h

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <stdint.h>
#include <limits.h>
#include <openssl/evp.h> // For hash calculation
#include <assert.h> // for debugging

// General
#define READ_BUFF 16384 // Arbitrary
#define OPTION_SIZE 4 // Gboolean count for gvariant
#define SHA256_DIGEST_LENGTH 32 // SHA256 hash length
#define PRINTABLE_CHAR_SIZE 16 // Number of bytes to format for a line

// Limits
#define MAX_FOLDERS 10 // Arbitrary
#define MAX_ENTRIES 9999999 // Arbitrary
#define MAX_GROUPS (MAX_ENTRIES / 2) // Arbitrary

// Char buffer sizes for strings
#define STR_PATH (PATH_MAX *2) // From limits.h includes null, double for possible unicode
#define STR_BNAME (NAME_MAX *2) // From limits.h add null, double for possible unicode
#define STR_NAME (PATH_MAX + NAME_MAX)
#define STR_HASH (SHA256_DIGEST_LENGTH * 2) + 1 // Two asci hex digits for hash byte + Null
#define STR_CLIP (STR_PATH * 1000) // Clipboard mostly for small groups

#define STR_DIR "Directory\0"
#define STR_EMP "Empty\0"
#define STR_ERR "Error\0"
#define STR_UNI "Unique\0"

// Define the DupItem GObject type
#define DUP_TYPE_ITEM (dup_item_get_type ())
G_DECLARE_FINAL_TYPE (DupItem, dup_item, DUP, ITEM, GObject)

struct _DupItem {
        GObject parent_instance;
        const char *result;
        const char *name;
        const char *hash;
        const char *file_size;
        const char *modified;
};

typedef struct search_entry {
        GtkWidget *res_e;
        GtkEntryBuffer *res_eb;
        char res_ebt[256];

        GtkWidget *name_e;
        GtkEntryBuffer *name_eb;
        char name_ebt[256];

        GtkWidget *clear_btn;

        gboolean and;
        gboolean or; 
} search_entry;

typedef struct filter_entry {
        GtkWidget *res_e;
        GtkEntryBuffer *res_eb;
        char res_ebt[256];

        GtkWidget *name_e;
        GtkEntryBuffer *name_eb;
        char name_ebt[256];

        GtkWidget *clear_btn;

        gboolean and;
        gboolean or; 
        gboolean res_n;
        gboolean name_n;	
} filter_entry;

// A key structure to pass between API calls
typedef struct user_data {

	// Application
	GtkApplication *app;

        // UI pointers
        GtkWidget *main_window;
	GtkWidget *progress_bar;
	GtkWidget *action_window;
	GtkWidget *trash_window;
	GtkWidget *option_window;
	GtkWidget *column_view;

	// Entry data
	GListStore *list_store;

	// Buttons - need to adjust sensivity
	GtkWidget *sort_button;
	GtkWidget *filter_button;
	GtkWidget *search_entry;
	GtkWidget *search_bar;
	GtkWidget *cancel_button;

	// Folder 
	char **fdpp; // A pointer to an array of pointers to folder names

	// Selection processing fields
	DupItem *sel_item;
	guint sel_item_position;
        GtkMultiSelection *selection;
	GtkBitset *sel_bitset;

	// Filter 
        GtkWidget *filter_window;
        GtkFilterListModel  *filter;
        GtkCustomFilter *custom_filter;
	filter_entry *ep;

	// Search
	GtkWidget *search_window;
	search_entry *sep;


        // Switches
        gboolean cancel_request; // True if user has requested a cancel
        gboolean changed_list; // True if sorted, filtered or searched or selected option that filters
        gboolean mem_alloc_err; // True if bad error and need to stop processing - useful for recursion functions which will overlay return

	// Options
	char *opt_name;
	gboolean opt_changed;
	gboolean opt_include_empty;
        gboolean opt_include_directory;
        gboolean opt_include_duplicate;
        gboolean opt_include_unique;

} user_data;

// Function prototypes

int main_window(GtkApplication *, user_data *);

void adjust_sfs_button_sensitivity(user_data *);

void adjust_restore_button_sensitivity(user_data *);

#endif
