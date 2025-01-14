#ifndef lib_h
#define lib_h

void free_item_memory(DupItem *);
void clear_store_items(GListStore *);
gboolean read_options(unsigned char *, char *);

#endif
