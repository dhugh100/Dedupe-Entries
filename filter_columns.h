#ifndef filter_columns_h
#define filter_columns_h

void
setup_filters (user_data * udp);

gboolean
filter_match (DupItem *, user_data *);

void
get_filters_cb (GtkWidget *, user_data *);

#endif
