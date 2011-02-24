/*
A library to communicate a menu object set accross DBus and
track updates and maintain consistency.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of either or both of the following licenses:

1) the GNU Lesser General Public License version 3, as published by the 
Free Software Foundation; and/or
2) the GNU Lesser General Public License version 2.1, as published by 
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the applicable version of the GNU Lesser General Public 
License for more details.

You should have received a copy of both the GNU Lesser General Public 
License version 3 and version 2.1 along with this program.  If not, see 
<http://www.gnu.org/licenses/>
*/

#ifndef __DBUSMENU_CLIENT_H__
#define __DBUSMENU_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include "menuitem.h"

G_BEGIN_DECLS

#define DBUSMENU_TYPE_CLIENT            (dbusmenu_client_get_type ())
#define DBUSMENU_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DBUSMENU_TYPE_CLIENT, DbusmenuClient))
#define DBUSMENU_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DBUSMENU_TYPE_CLIENT, DbusmenuClientClass))
#define DBUSMENU_IS_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DBUSMENU_TYPE_CLIENT))
#define DBUSMENU_IS_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DBUSMENU_TYPE_CLIENT))
#define DBUSMENU_CLIENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DBUSMENU_TYPE_CLIENT, DbusmenuClientClass))

#define DBUSMENU_CLIENT_SIGNAL_LAYOUT_UPDATED  "layout-updated"
#define DBUSMENU_CLIENT_SIGNAL_ROOT_CHANGED    "root-changed"
#define DBUSMENU_CLIENT_SIGNAL_NEW_MENUITEM    "new-menuitem"
#define DBUSMENU_CLIENT_SIGNAL_ITEM_ACTIVATE   "item-activate"
#define DBUSMENU_CLIENT_SIGNAL_EVENT_RESULT    "event-result"

#define DBUSMENU_CLIENT_PROP_DBUS_NAME     "dbus-name"
#define DBUSMENU_CLIENT_PROP_DBUS_OBJECT   "dbus-object"

#define DBUSMENU_CLIENT_TYPES_DEFAULT      "standard"
#define DBUSMENU_CLIENT_TYPES_SEPARATOR    "separator"
#define DBUSMENU_CLIENT_TYPES_IMAGE        "standard"

typedef struct _DbusmenuClientPrivate DbusmenuClientPrivate;

/**
	DbusmenuClientClass:
	@parent_class: #GObjectClass
	@layout_updated: Slot for #DbusmenuClient::layout-updated.
	@new_menuitem: Slot for #DbusmenuClient::new-menuitem.
	@item_activate: Slot for #DbusmenuClient::item-activate.
	@event_result: Slot for #DbusmenuClient::event-error.
	@reserved1: Reserved for future use.
	@reserved2: Reserved for future use.
	@reserved3: Reserved for future use.
	@reserved4: Reserved for future use.
	@reserved5: Reserved for future use.
	@reserved6: Reserved for future use.

	A simple class that takes all of the information from a
	#DbusmenuServer over DBus and makes the same set of 
	#DbusmenuMenuitem objects appear on the other side.
*/
typedef struct _DbusmenuClientClass DbusmenuClientClass;
struct _DbusmenuClientClass {
	GObjectClass parent_class;

	void (*layout_updated)(void);
	void (*root_changed) (DbusmenuMenuitem * newroot);
	void (*new_menuitem) (DbusmenuMenuitem * newitem);
	void (*item_activate) (DbusmenuMenuitem * item, guint timestamp);
	void (*event_result) (DbusmenuMenuitem * item, gchar * event, GVariant * data, guint timestamp, GError * error);

	/*< Private >*/
	void (*reserved1) (void);
	void (*reserved2) (void);
	void (*reserved3) (void);
	void (*reserved4) (void);
	void (*reserved5) (void);
	void (*reserved6) (void);
};

/**
	DbusmenuClient:
	@parent: #GObject.

	The client for a #DbusmenuServer creating a shared
	object set of #DbusmenuMenuitem objects.
*/
typedef struct _DbusmenuClient      DbusmenuClient;
struct _DbusmenuClient {
	GObject parent;

	/*< Private >*/
	DbusmenuClientPrivate * priv;
};

/**
	DbusmenuClientTypeHandler:
	@newitem: The #DbusmenuMenuitem that was created
	@parent: The parent of @newitem or #NULL if none
	@client: A pointer to the #DbusmenuClient
	@user_data: The data you gave us

	The type handler is called when a dbusmenu item is created
	with a matching type as setup in #dbusmenu_client_add_type_handler
*/
typedef gboolean (*DbusmenuClientTypeHandler) (DbusmenuMenuitem * newitem, DbusmenuMenuitem * parent, DbusmenuClient * client, gpointer user_data);

/**
	DbusmenuClientTypeDestroyHandler:
	@client: A pointer to the #DbusmenuClient
	@type: The type that this handler was registered with
	@user_data: The data you gave us

	This handler is called when the type becomes unregistered by the
	client.  This is usally caused by the #DbusmenuClient being destroyed
	and should free memory or unref objects in @user_data.
*/
typedef void (*DbusmenuClientTypeDestroyHandler) (DbusmenuClient * client, const gchar * type, gpointer user_data);

GType                dbusmenu_client_get_type          (void);
DbusmenuClient *     dbusmenu_client_new               (const gchar * name,
                                                        const gchar * object);
DbusmenuMenuitem *   dbusmenu_client_get_root          (DbusmenuClient * client);
gboolean             dbusmenu_client_add_type_handler  (DbusmenuClient * client,
                                                        const gchar * type,
                                                        DbusmenuClientTypeHandler newfunc);
gboolean             dbusmenu_client_add_type_handler_full (DbusmenuClient * client,
                                                        const gchar * type,
                                                        DbusmenuClientTypeHandler newfunc,
                                                        gpointer user_data,
                                                        DbusmenuClientTypeDestroyHandler destroy_func);
void                 dbusmenu_client_send_event        (DbusmenuClient * client,
                                                        gint id,
                                                        const gchar * name,
                                                        GVariant * variant,
                                                        guint timestamp);
void                 dbusmenu_client_send_about_to_show(DbusmenuClient * client,
                                                        gint id,
                                                        void (*cb) (gpointer user_data),
                                                        gpointer cb_data);

/**
	SECTION:client
	@short_description: The catcher of all the server traffic
	@stability: Unstable
	@include: libdbusmenu-glib/client.h

	The client exists as a mirror to the server.  For most folks
	all they will do with a client is set it up to connect to
	a server and then watch as the menu items on their side
	of the bus change.  This is all they should need to know about
	the client, that it magically makes their menuitems dance.

	It does this by setting up signal watchers and adjusting
	the menuitems appropriately.  Most users should watch the
	menu items and the signal #DbusmenuClient::layout-changed for
	larger events so that they can be optimized.  It is possible
	with that signal that even the root node would change.  If
	that doesn't happen the normal signals on the individual
	nodes should be enough for most users.
*/

G_END_DECLS

#endif
