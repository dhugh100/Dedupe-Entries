#include "main.h"
#include "logo.h"
#include "about.h"

// Provide standard about dialog
// -Build script will update version and release
// -Build script compiles logo and header source, linked into the program, proving inline logo

void
about_cb (GSimpleAction *action,
	  GVariant      *parm,
	  user_data     *udp)
{
	const char *author[] = { "Author", "\nDave Hugh", NULL };

	// Get logo from inline resource compiled into ddup binary
	GResource *logo_r = logo_get_resource(); 
	g_resources_register(logo_r); 
	GdkTexture *logo_t = gdk_texture_new_from_resource ("/ddup/ddup.png");

	gtk_show_about_dialog (NULL,
			      "program-name", "ddup",
			      "title", "About ddup",
			      "logo", logo_t,		
			      "authors", author,
			      "version", "Version 0.1.0 Release 1",
			      "comments",
			      "A program to find and take action on duplicate files.\nRelies on SHA256 hashes to identify duplicates.",
			      "copyright", "Copyright 2024 David Hugh", "license-type", GTK_LICENSE_GPL_3_0, NULL);
}
