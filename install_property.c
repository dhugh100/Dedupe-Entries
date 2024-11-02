#include "main.h"
#include "install_property.h"

G_DEFINE_TYPE (DupItem, dup_item, G_TYPE_OBJECT)


// Have to start at 1 because 0 is reserved for G_TYPE_OBJECT	
typedef enum {
	PROP_RESULT = 1,
	PROP_NAME,
	PROP_HASH,
	PROP_FILE_SIZE,
	PROP_MODIFIED,
	PROP_HIGHLIGHT,
	N_PROPERTIES
} DupItemProperty;

// Required by boiler plate
static void dup_item_init (DupItem *item)
{
}

// Set the properties
static void dup_item_set_property (GObject *object, uint32_t property_id, const GValue *value, GParamSpec *pspec) {

	DupItem *self = (DupItem *) object;

	switch ((DupItemProperty) property_id) {

		case PROP_RESULT:
			self->result = g_value_dup_string (value);
			break;

		case PROP_NAME:
			self->name = g_value_dup_string (value);
			break;

		case PROP_HASH:
			self->hash = g_value_dup_string (value);
			break;

		case PROP_FILE_SIZE:
			self->file_size = g_value_dup_string (value);
			break;

		case PROP_MODIFIED:
			self->modified = g_value_dup_string (value);
			break;

		case PROP_HIGHLIGHT:
			self->highlight = g_value_get_boolean (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

// Get the property values from dup item object
void dup_item_get_property (GObject *object, uint32_t property_id, GValue *value, GParamSpec *pspec)
{
	DupItem *self = (DupItem *) object;

	switch ((DupItemProperty) property_id) {

		case PROP_RESULT:
			g_value_set_string (value, self->result);
			break;

		case PROP_NAME:	
			g_value_set_string (value, self->name);
			break;

		case PROP_HASH:
			g_value_set_string (value, self->hash);
			break;

		case PROP_FILE_SIZE:	
			g_value_set_string (value, self->file_size);
			break;

		case PROP_MODIFIED:	
			g_value_set_string (value, self->modified);
			break;

		case PROP_HIGHLIGHT:	
			g_value_set_boolean (value, self->highlight);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, }; // Use in class init

// Drive the property installation
static void dup_item_class_init (DupItemClass *klass) {

	// Set and get properties for class
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->set_property = dup_item_set_property;
	object_class->get_property = dup_item_get_property;


	// Define param spec for each property
	obj_properties[PROP_RESULT] = g_param_spec_string ("result", "Result", "1 column is result", NULL,
							G_PARAM_READWRITE);

	obj_properties[PROP_NAME] = g_param_spec_string ("name", "Name", "2 column is name", NULL,
							G_PARAM_READWRITE);

	obj_properties[PROP_HASH] = g_param_spec_string ("hash", "Hash", "3 column is hash", NULL,
							G_PARAM_READWRITE);

	obj_properties[PROP_FILE_SIZE] = g_param_spec_string ("file_size", "File_size", "4 column is file size", NULL,
							G_PARAM_READWRITE);
	
	obj_properties[PROP_MODIFIED] = g_param_spec_string ("modified", "Modified", "5 column is modified", NULL,
							G_PARAM_READWRITE);

	obj_properties[PROP_HIGHLIGHT] = g_param_spec_string ("highlight", "Highlight", "6 column is highlight", NULL,
							G_PARAM_READWRITE);

	// Install properties
	g_object_class_install_properties (object_class, N_PROPERTIES, 	obj_properties);
}
