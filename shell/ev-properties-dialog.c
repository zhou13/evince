/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of evince, a gnome document viewer
 *
 *  Copyright (C) 2005 Red Hat, Inc
 *
 * Evince is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Evince is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#ifdef PLATFORM_HILDON
#include <hildon/hildon-gtk.h>
#include <hildon/hildon-pannable-area.h>
#include <hildon/hildon-stackable-window.h>
#endif

#include "ev-document-fonts.h"
#include "ev-page-cache.h"
#include "ev-properties-dialog.h"
#include "ev-properties-fonts.h"
#include "ev-properties-view.h"

struct _EvPropertiesDialog {
#ifndef PLATFORM_HILDON
	GtkDialog base_instance;
#else
        HildonStackableWindow base_instance;
#endif

	EvDocument *document;
	GtkWidget *notebook;
	GtkWidget *general_page;
	GtkWidget *fonts_page;
};

struct _EvPropertiesDialogClass {
#ifndef PLATFORM_HILDON
	GtkDialogClass base_class;
#else
        HildonStackableWindowClass base_class;
#endif
};

#ifndef PLATFORM_HILDON
G_DEFINE_TYPE (EvPropertiesDialog, ev_properties_dialog, GTK_TYPE_DIALOG)
#else
G_DEFINE_TYPE (EvPropertiesDialog, ev_properties_dialog, HILDON_TYPE_STACKABLE_WINDOW)
#endif

static void
ev_properties_dialog_class_init (EvPropertiesDialogClass *properties_class)
{
}

static void
ev_properties_dialog_init (EvPropertiesDialog *properties)
{
	gtk_window_set_title (GTK_WINDOW (properties), _("Properties"));
	gtk_window_set_destroy_with_parent (GTK_WINDOW (properties), TRUE);

#ifndef PLATFORM_HILDON
        gtk_dialog_set_has_separator (GTK_DIALOG (properties), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (properties), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (properties)->vbox), 2);

	gtk_dialog_add_button (GTK_DIALOG (properties), GTK_STOCK_CLOSE,
			       GTK_RESPONSE_CANCEL);
	gtk_dialog_set_default_response (GTK_DIALOG (properties), 
			                 GTK_RESPONSE_CANCEL);
#else

#endif /* !PLATFORM_HILDON */

	properties->notebook = gtk_notebook_new ();

#ifndef PLATFORM_HILDON
	gtk_container_set_border_width (GTK_CONTAINER (properties->notebook), 5);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (properties)->vbox),
			    properties->notebook, TRUE, TRUE, 0);
#else
        gtk_container_add (GTK_CONTAINER (properties), properties->notebook);
#endif /* !PLATFORM_HILDON */

	gtk_widget_show (properties->notebook);

	g_signal_connect (properties, "response",
			  G_CALLBACK (gtk_widget_destroy), NULL);
}

void
ev_properties_dialog_set_document (EvPropertiesDialog *properties,
				   const gchar        *uri,
			           EvDocument         *document)
{
	GtkWidget *label;
	const EvDocumentInfo *info;

	properties->document = document;

	info = ev_page_cache_get_info (ev_page_cache_get (document));

	if (properties->general_page == NULL) {
		label = gtk_label_new (_("General"));
		properties->general_page = ev_properties_view_new (uri);

#ifndef PLATFORM_HILDON
		gtk_notebook_append_page (GTK_NOTEBOOK (properties->notebook),
					  properties->general_page, label);
		gtk_widget_show (properties->general_page);
#else
        {
                GtkWidget *viewport, *pannable;

                viewport = gtk_viewport_new (NULL, NULL);
                gtk_container_add (GTK_CONTAINER (viewport), properties->general_page);
		gtk_widget_show (properties->general_page);

                pannable = hildon_pannable_area_new ();
                gtk_container_add (GTK_CONTAINER (pannable), viewport);
                gtk_widget_show (viewport);

		gtk_notebook_append_page (GTK_NOTEBOOK (properties->notebook),
					  pannable, label);
                gtk_widget_show (pannable);
        }
#endif /* !PLATFORM_HILDON */
	}
	ev_properties_view_set_info (EV_PROPERTIES_VIEW (properties->general_page), info);

	if (EV_IS_DOCUMENT_FONTS (document)) {
		if (properties->fonts_page == NULL) {
			label = gtk_label_new (_("Fonts"));
			properties->fonts_page = ev_properties_fonts_new ();
			gtk_notebook_append_page (GTK_NOTEBOOK (properties->notebook),
						  properties->fonts_page, label);
			gtk_widget_show (properties->fonts_page);
		}

		ev_properties_fonts_set_document
			(EV_PROPERTIES_FONTS (properties->fonts_page), document);
	}
}

GtkWidget *
ev_properties_dialog_new ()
{
	EvPropertiesDialog *properties;

	properties = g_object_new (EV_TYPE_PROPERTIES_DIALOG, NULL);

	return GTK_WIDGET (properties);
}
