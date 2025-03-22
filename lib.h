// This file, lib.h, is a part of the Entry Dedupe program.
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

#ifndef lib_h
#define lib_h

void do_pending();
void cancel_clean_up (user_data *);
void clear_folders (char *[MAX_FOLDERS]);
void adjust_sfs_button_sensitivity(user_data *);
void wipe_selected(user_data *);
void clear_stores(user_data *);
void free_item_memory(DupItem *);
void clear_store_items(GListStore *);
void see_entry_data(GListStore *, GtkMultiSelection *);
void trash_em(user_data *);
gboolean read_options(unsigned char *, char *);

#endif
