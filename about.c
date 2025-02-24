// This file, about.c, is a part of the Dedupe Entries program.
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

#include "main.h"
#include "logo.h"
#include "about.h"

// Provide standard about dialog
//- Build script will update version and release
//- Build script compiles logo and header source, linked into the program, proving inline logo

void about_cb (GSimpleAction *action, GVariant *parm, user_data *udp)
{
	const char *author[] = { "Author", "\nDavid Hugh", NULL };

	// Get logo from inline resource compiled into Dedupe Entries binary
	GdkTexture *logo_t = gdk_texture_new_from_resource ("/Dedupe Entries/dedupee.png");

	gtk_show_about_dialog (NULL,
		      "program-name", "Dedupe Entries",
		      "title", "About Dedupe Entries",
		      "logo", logo_t,
		      "authors", author,
		      "version", "Major  1 Minor 0 Patch 0",
		      "comments",
		      "A program to find and take action on duplicate files.\nRelies on SHA256 hashes to identify duplicates.",
		      "copyright", "Copyright 2025 David Hugh",
		      "license-type", GTK_LICENSE_GPL_3_0, NULL);
}
