// This file, install_property.c, is a part of the Dedupe Entries program.
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
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "main.h"
#include "install-property.h"

G_DEFINE_TYPE(DupItem, dup_item, G_TYPE_OBJECT)

// Have to start at 1 because 0 is reserved for G_TYPE_OBJECT   
typedef enum {
	PROP_RESULT = 1,
	PROP_NAME,
	PROP_HASH,
	PROP_FILE_SIZE,
	PROP_MODIFIED,
	N_PROPERTIES
} DupItemProperty;

// Required by boiler plate
static void dup_item_init(DupItem *item)
{
}

// Free the item strings when the last reference drops
// - Lets shared items (the filter stores hold the same pointers) be
//   managed by reference counting instead of manual frees
static void dup_item_finalize(GObject *object)
{
	DupItem *self = (DupItem *) object;

	g_free((gpointer) self->result);
	g_free((gpointer) self->name);
	g_free((gpointer) self->hash);
	g_free((gpointer) self->file_size);
	g_free((gpointer) self->modified);

	G_OBJECT_CLASS(dup_item_parent_class)->finalize(object);
}

// Set the properties
// - Free any prior value so a re-set does not leak it
static void dup_item_set_property(GObject *object, uint32_t property_id, const GValue *value, GParamSpec *pspec)
{
	DupItem *self = (DupItem *) object;

	switch ((DupItemProperty) property_id) {

	case PROP_RESULT:
		g_free((gpointer) self->result);
		self->result = g_value_dup_string(value);
		break;

	case PROP_NAME:
		g_free((gpointer) self->name);
		self->name = g_value_dup_string(value);
		break;

	case PROP_HASH:
		g_free((gpointer) self->hash);
		self->hash = g_value_dup_string(value);
		break;

	case PROP_FILE_SIZE:
		g_free((gpointer) self->file_size);
		self->file_size = g_value_dup_string(value);
		break;

	case PROP_MODIFIED:
		g_free((gpointer) self->modified);
		self->modified = g_value_dup_string(value);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

// Get the property values from dup item object
void dup_item_get_property (GObject *object, uint32_t property_id, GValue *value, GParamSpec *pspec)
{
	DupItem *self = (DupItem *) object;

	switch ((DupItemProperty) property_id) {

	case PROP_RESULT:
		g_value_set_string(value, self->result);
		break;

	case PROP_NAME:
		g_value_set_string(value, self->name);
		break;

	case PROP_HASH:
		g_value_set_string(value, self->hash);
		break;

	case PROP_FILE_SIZE:
		g_value_set_string(value, self->file_size);
		break;

	case PROP_MODIFIED:
		g_value_set_string(value, self->modified);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, }; // Use in class init

// Drive the property installation
static void dup_item_class_init(DupItemClass *klass)
{

	// Set, get, and finalize for class
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->set_property = dup_item_set_property;
	object_class->get_property = dup_item_get_property;
	object_class->finalize = dup_item_finalize;

	// Define param spec for each property
	obj_properties[PROP_RESULT] = g_param_spec_string("result", "Result", "1 column is result", NULL, G_PARAM_READWRITE);

	obj_properties[PROP_NAME] = g_param_spec_string("name", "Name", "2 column is name", NULL, G_PARAM_READWRITE);

	obj_properties[PROP_HASH] = g_param_spec_string("hash", "Hash", "3 column is hash", NULL, G_PARAM_READWRITE);

	obj_properties[PROP_FILE_SIZE] = g_param_spec_string("file_size", "File_size", "4 column is file size", NULL,
							     G_PARAM_READWRITE);

	obj_properties[PROP_MODIFIED] = g_param_spec_string("modified", "Modified", "5 column is modified", NULL,
							    G_PARAM_READWRITE);

	// Install properties
	g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
