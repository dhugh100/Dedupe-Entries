#include "main.h"
#include "about.h"

// Provide standard about dialog
// -Build script will update version and release

void
about_cb (GSimpleAction *action,
		GVariant *parm,
		user_data *udp)
{
	const char *author[] = { "Author", "\nDave Hugh", NULL };

	GFile *logo_file = g_file_new_for_path ("/home/dhugh/Documents/gimp.logo.png");
	GdkTexture *example_logo = gdk_texture_new_from_file (logo_file, NULL);
	g_object_unref (logo_file);

	gtk_show_about_dialog (NULL,
			      "program-name", "ddup",
			      "logo", example_logo,
			      "title", "About ddup",
			      "authors", author,
			      "version", "Version 0.0.0 Release 1",
			      "comments",
			      "A program to find and take action on duplicate files.\nRelies on SHA256 hashes to identify duplicates.",
			      "copyright", "Copyright 2024 David Hugh", "license-type", GTK_LICENSE_GPL_3_0, NULL);
}
