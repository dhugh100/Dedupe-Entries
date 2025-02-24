// This file, main.h, is a part of the Dedupe Entries program.
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

#ifndef main_h
#define main_h

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <stdint.h>
#include <limits.h>
#include <openssl/evp.h> // For hash calculation
#include <assert.h> // for debugging
#include <ctype.h> 
#include <libgen.h> 

// General
#define READ_BUFF 16384 // Arbitrary
#define OPTION_SIZE 4 // Byte count for gvariant
#define SHA256_DIGEST_LENGTH 32 // SHA256 hash length
#define PRINTABLE_CHAR_SIZE 16 // Number of bytes to format for a line

// Limits
#define MAX_FOLDERS 20 // Arbitrary
#define MAX_ENTRIES 9999999 // Arbitrary 
#define MAX_GROUPS (MAX_ENTRIES / 2) // Arbitrary

// Char buffer sizes for strings
#define STR_PATH PATH_MAX // From limits.h includes null
#define STR_BNAME (NAME_MAX + 1) // From limits.h add null
#define STR_HASH (SHA256_DIGEST_LENGTH * 2) + 1 // Two asci hex digits for hash byte + Null
#define STR_CLIP (STR_PATH * 1000) // Clipboard mostly for small groups

// Avoid typos by using
#define STR_DIR "Directory\0"
#define STR_EMP "Empty\0"
#define STR_ERR "Error\0"
#define STR_UNI "Unique\0"
#define STR_SPACE " \0"

// Define the DupItem GObject type
#define DUP_TYPE_ITEM (dup_item_get_type ())
G_DECLARE_FINAL_TYPE (DupItem, dup_item, DUP, ITEM, GObject)

// Key type

struct _DupItem {
        GObject parent_instance;
        const char *result;
        const char *name;
        const char *hash;
        const char *file_size;
        const char *modified;
};

// Use when searching columns

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

// Use when filtering columns

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

// The key structure to pass between API calls
typedef struct user_data {

	// Application
	GtkApplication *app;

        // UI pointers
        GtkWidget *main_window;
	GtkWidget *progress_bar;
	GtkWidget *action_window;
	GtkWidget *trash_window;
	GtkWidget *option_window;
	GtkWidget *sort_window;
	GtkWidget *column_view;

	// Entry data
	GListStore *list_store;
	GListStore *filtered_list_store;
	GListStore *org_list_store;

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
	uint32_t sel_item_position;
        GtkMultiSelection *selection;
	GtkBitset *sel_bitset;

	// Filter 
	uint32_t remove_position;
        GtkWidget *filter_window;
        GtkFilterListModel  *filter;
        GtkCustomFilter *custom_filter;
	filter_entry *fep;

	// Search
	GtkWidget *search_window;
	search_entry *sep;
	GtkWidget *entry;
	uint32_t next_check;
	gboolean a_match;

	// Sort
	GtkWidget *result_a_button;
        GtkWidget *result_d_button;
        GtkWidget *name_a_button;
        GtkWidget *name_d_button;
        GtkWidget *name_a_sec_btn;
        GtkWidget *name_d_sec_btn;

	// View file
	GtkStringList *str_list; 

	// Clipboard
	GdkClipboard *clippy;

	// Switches
        gboolean cancel_request; // True if user has requested a cancel
        gboolean cancel_string_load; // True when view file windows is closed
        gboolean ut_active; // True when view file windows is closed

	// Options
	char *opt_name;
	gboolean opt_changed;
	gboolean opt_include_empty;
        gboolean opt_include_directory;
        gboolean opt_include_duplicate;
        gboolean opt_include_unique;

} user_data;

void option_init(user_data *udp);

#endif
