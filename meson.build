project('ddup', 'c')
gtkdep = dependency('gtk4')
cryptodep = dependency('openssl')
gnome=import('gnome')
extra_args = ['-lcrypto']
sourcefiles=files('main.c', 'get_folders.c', 'load_entry_data.c', 'traverse.c', 'get_hash.c', 'get_results.c', 'show_columns.c', 'install_property.c', 'work_selected.c', 'see_entry_data.c', 'view_file.c', 'sort_columns.c', 'filter_columns.c', 'work_trash.c', 'work_options.c')
executable('ddup', sourcefiles, dependencies: [gtkdep, cryptodep], c_args: extra_args, install: false)
