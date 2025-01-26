#ifndef lib_h
#define lib_h

void wipe_selected(user_data *);
void clear_stores(user_data *);
void free_item_memory(DupItem *);
void clear_store_items(GListStore *);
gboolean read_options(unsigned char *, char *);

#endif
