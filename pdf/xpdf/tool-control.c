/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* Tool selection menu button
 *
 * Copyright (C) 2004 Remi Cohen-Scali
 *
 * Author:
 *   Remi Cohen-Scali <remi@cohen-scali.com>
 *
 * GPdf is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GPdf is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * This is the implementation for the tool control class that
 * propose the user with one menu button with an icon for each
 * available tool mode for gpdf.
 * This class is specialized from GtkButton. It has an arrow and
 * a popup menu.
 * Interface provide one signal and two functions:
 *
 * signal:
 *   tool_changed - emited when tool is selected and set. Has one
 *                  argument being the new tool value.
 */

#include <aconf.h>
#include <stdlib.h>
#include <libgnome/gnome-macros.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gpdf-stock-icons.h"
#include "gpdf-util.h"
#include "gpdf-marshal.h"
#include "tool-control.h"

/*
 * Declare & register class
 */
#define PARENT_TYPE GTK_TYPE_BUTTON
GPDF_CLASS_BOILERPLATE (GPdfToolControl, gpdf_tool_control,
			GtkButton, PARENT_TYPE)

/*
 * Private structure
 */
struct _GPdfToolControlPrivate
{
        GtkMenu *popup_menu;		/* Popup menu */
	GtkTooltips *tooltips;		

        GtkWidget *drag_tool_image;	/* Drag tool image for item & button */
        GtkWidget *select_tool_image;	/* Select tool image for item & button */

        GtkWidget *hbox;		/* Button HBox */
        GtkWidget *arrow;		/* Button arrow */

        GPdfTool current_tool;		/* Current selected tool */
};

/* Signals */
enum {
        TOOL_CHANGED_SIGNAL,
        LAST_SIGNAL
};

static guint gpdf_tool_control_signals [LAST_SIGNAL];



/*
 * Dispose method overiding GObject::dispose
 */
static void
gpdf_tool_control_dispose (GObject *object)
{
        GPdfToolControl *control = GPDF_TOOL_CONTROL (object);
        GPdfToolControlPrivate *priv = control->priv;

        if (priv->popup_menu) {
            gtk_widget_destroy ((GtkWidget *)priv->popup_menu);
            priv->popup_menu = NULL;
        }

        GNOME_CALL_PARENT (G_OBJECT_CLASS, dispose, (object));
}

/*
 * Finalize method overiding GObject::finalize
 */
static void
gpdf_tool_control_finalize (GObject *object)
{
        GPdfToolControl *control = GPDF_TOOL_CONTROL (object);

        if (control->priv) {
                g_free (control->priv);
                control->priv = NULL;
        }

        GNOME_CALL_PARENT (G_OBJECT_CLASS, finalize, (object));
}

/*
 * Destroy method overiding GtkWidget::destroy
 */
static void
gpdf_tool_control_destroy (GtkObject *object)
{
	GPdfToolControlPrivate *priv = GPDF_TOOL_CONTROL (object)->priv;

	if (priv->tooltips) {
		g_object_unref (priv->tooltips);
		priv->tooltips = NULL;
	}

        /* Destroy all children etc ... */
	GNOME_CALL_PARENT (GTK_OBJECT_CLASS, destroy, (object));
}

/*
 * Init class and register signals
 */
void
gpdf_tool_control_class_init (GPdfToolControlClass *klass)
{
        GObjectClass *object_class;
	GtkObjectClass *gtk_object_class;
	GtkWidgetClass *gtk_widget_class;

        object_class = G_OBJECT_CLASS (klass);
	gtk_object_class = GTK_OBJECT_CLASS (klass);
	gtk_widget_class = GTK_WIDGET_CLASS (klass);

        object_class->dispose = gpdf_tool_control_dispose;
        object_class->finalize = gpdf_tool_control_finalize;
	gtk_object_class->destroy = gpdf_tool_control_destroy;

        gpdf_tool_control_signals [TOOL_CHANGED_SIGNAL] = g_signal_new (
                "tool-changed",
                G_TYPE_FROM_CLASS (object_class),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GPdfToolControlClass, tool_changed),
                NULL, NULL,
                gpdf_marshal_VOID__INT_INT,
                G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
}

/*
 * Toggle button image when a popup menu item have been selected
 */
static void
gpdf_tool_control_set_button_image (GPdfToolControl *control)
{
    GPdfToolControlPrivate *priv = control->priv;
    
    switch (priv->current_tool)
    {
      case GPDF_TOOL_CTRL_DRAG:
        gtk_widget_hide (GTK_WIDGET (priv->select_tool_image));
        gtk_widget_show (GTK_WIDGET (priv->drag_tool_image));
        break;
        
      case GPDF_TOOL_CTRL_SELECT:
        gtk_widget_hide (GTK_WIDGET (priv->drag_tool_image));
        gtk_widget_show (GTK_WIDGET (priv->select_tool_image));
        break;
    }
}

/*
 * Emit TOOL_CHANGED signal
 */
static void
gpdf_tool_control_emit_tool_changed (GPdfToolControl *control, GPdfTool new, GPdfTool old)
{
    g_signal_emit (G_OBJECT (control),
                   gpdf_tool_control_signals [TOOL_CHANGED_SIGNAL],
                   0, new, old);
}

/*
 * Callback for Drag tool popup menu item
 */
static void
gpdf_tool_control_drag_tool_selected_cb (GtkMenuItem *item, GPdfToolControl *control)
{
    GPdfToolControlPrivate *priv = control->priv;
    GPdfTool old = priv->current_tool;

    if (priv->current_tool != GPDF_TOOL_CTRL_DRAG)
    {
        priv->current_tool = GPDF_TOOL_CTRL_DRAG;
        gpdf_tool_control_set_button_image (control);
        gpdf_tool_control_emit_tool_changed (control, GPDF_TOOL_CTRL_DRAG, old);
    }
}

/*
 * Callback for Select tool popup menu item
 */
static void
gpdf_tool_control_select_tool_selected_cb (GtkMenuItem *item, GPdfToolControl *control)
{
    GPdfToolControlPrivate *priv = control->priv;
    GPdfTool old = priv->current_tool;

    if (priv->current_tool != GPDF_TOOL_CTRL_SELECT)
    {
        priv->current_tool = GPDF_TOOL_CTRL_SELECT;
        gpdf_tool_control_set_button_image (control);
        gpdf_tool_control_emit_tool_changed (control, GPDF_TOOL_CTRL_SELECT, old);
    }
}

/*
 * Compute popup menu position according to its attached widget.
 * Take care of root window borders to avoid the menu being outside.
 */
static void
gpdf_tool_control_popup_position_func (GtkMenu *menu,
                                       gint    *x,
                                       gint    *y,
                                       gboolean *push_in,
                                       gpointer user_data)
{
    GPdfToolControl *control = GPDF_TOOL_CONTROL (user_data);
    GPdfToolControlPrivate *priv = control->priv;
    GtkWidget *widget = GTK_WIDGET (control);
    GdkScreen *screen = gtk_widget_get_screen (widget);
    GtkRequisition req;

    gdk_window_get_origin (widget->window, x, y);      

    gtk_widget_size_request (GTK_WIDGET (priv->popup_menu),
                             &req);
  
    *x += widget->allocation.width / 2;
    *y += widget->allocation.height;

    *x = CLAMP (*x, 0, MAX (0, gdk_screen_get_width (screen) - req.width));
    *y = CLAMP (*y, 0, MAX (0, gdk_screen_get_height (screen) - req.height));
}

/*
 * Callback for button clicked event. Displays the popup menu
 */
static void
gpdf_tool_control_popup_menu(GPdfToolControl *control, gpointer dum)
{
    GPdfToolControlPrivate *priv = control->priv;

    gtk_menu_popup (priv->popup_menu,
                    NULL, NULL,
                    gpdf_tool_control_popup_position_func, control,
                    0,
                    gtk_get_current_event_time ());
}

/*
 * Create tool menu item with icon and set it up
 */
GtkWidget *
gpdf_tool_control_setup_tool_item (GtkWidget *w,
                                   const gchar *stock_id, 
                                   GtkMenu *menu,
                                   gchar *mnemonic,
                                   GCallback cb)
{
    GtkWidget *item;
    GtkIconSet *icon_set;
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    GtkWidget *button_image;

    /* Create item */
    item = gtk_image_menu_item_new_with_mnemonic (mnemonic);
    gtk_widget_show (item);
    gtk_container_add (GTK_CONTAINER (menu), item);

    /* Get icon set for requested icon */
    icon_set = gtk_style_lookup_icon_set (gtk_widget_get_style (w),
                                          stock_id);
    /* If the icon is not a stock icon or is not in gpdf-stock-icons */
    g_assert (icon_set);

    /* Render the icon menu size */
    pixbuf = gtk_icon_set_render_icon (icon_set,
                                       gtk_widget_get_style (w),
                                       gtk_widget_get_direction (w),
                                       GTK_STATE_NORMAL,
                                       GTK_ICON_SIZE_MENU,
                                       NULL, NULL);
    image = gtk_image_new_from_pixbuf (pixbuf);
    gtk_widget_show (image);
    gdk_pixbuf_unref (pixbuf);	

    /* Render the icon button size */
    pixbuf = gtk_icon_set_render_icon (icon_set,
                                       gtk_widget_get_style (w),
                                       gtk_widget_get_direction (w),
                                       GTK_STATE_NORMAL,
                                       GTK_ICON_SIZE_BUTTON,
                                       NULL, NULL);
    button_image = gtk_image_new_from_pixbuf (pixbuf);
    gtk_widget_show (button_image);
    gdk_pixbuf_unref (pixbuf);	

    /* Set image in item */
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);

    /* Connect callback on item activate signal */
    g_signal_connect (G_OBJECT (item), "activate", cb, (gpointer)w);

    /* Return image used in button */
    return (button_image);
}

/*
 * Setup control widgets.
 */
static void
gpdf_tool_control_setup_widgets (GPdfToolControl *control)
{
    GPdfToolControlPrivate *priv;
    GtkWidget *w;
    GtkWidget *align;
    
    priv = control->priv;
    w = GTK_WIDGET (control);

    /* Popup menu */
    priv->popup_menu = GTK_MENU (gtk_menu_new());
    
    /* Setup arrow */
    priv->arrow = gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
    gtk_widget_show (priv->arrow);    
    
    /*
     * Drag tool & drag tool button image
     */
    priv->drag_tool_image =
      gpdf_tool_control_setup_tool_item
        (w, GPDF_TOOL_OPEN_HAND, priv->popup_menu, _("Drag Tool"),
         G_CALLBACK (gpdf_tool_control_drag_tool_selected_cb));
    
    /*
     * Select tool & select tool button image
     */
    priv->select_tool_image =
      gpdf_tool_control_setup_tool_item
        (w, GPDF_TOOL_SELECT, priv->popup_menu, _("Select Tool"),
         G_CALLBACK (gpdf_tool_control_select_tool_selected_cb));


    /* Alignment */
    align = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_widget_show (align);
    gtk_container_add (GTK_CONTAINER (control), align);

    /* HBox containing image & arrow */
    priv->hbox = gtk_hbox_new (FALSE, 2);
    gtk_widget_show (priv->hbox);
    gtk_container_add (GTK_CONTAINER (align), priv->hbox);

    /* packing images and arrow */
    gtk_box_pack_start (GTK_BOX (priv->hbox), GTK_WIDGET (priv->drag_tool_image),
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (priv->hbox), GTK_WIDGET (priv->select_tool_image),
                        FALSE, FALSE, 0);
    gtk_widget_show (GTK_WIDGET (priv->drag_tool_image));
    gtk_widget_hide (GTK_WIDGET (priv->select_tool_image));
    gtk_box_pack_end (GTK_BOX (priv->hbox), priv->arrow, FALSE, FALSE, 0);

    /* Hide all images but the one of the current selected tool */
    gpdf_tool_control_set_button_image (control);

    /* Connect clicked button signal to popup displayer */
    g_signal_connect (G_OBJECT (control), "clicked",
                      G_CALLBACK (gpdf_tool_control_popup_menu),
                      NULL);
}

/*
 * Setup tooltips
 */
static void
gpdf_tool_control_setup_tooltips (GPdfToolControl *control)
{
	GPdfToolControlPrivate *priv;

	priv = control->priv;

        /* Create tooltips manager */
	priv->tooltips = gtk_tooltips_new ();
	g_object_ref (G_OBJECT (priv->tooltips));
	gtk_object_sink (GTK_OBJECT (priv->tooltips));

        /* Set tips for button */
	gtk_tooltips_set_tip (GTK_TOOLTIPS (priv->tooltips), GTK_WIDGET (control),
			      _("Current tool"),
			      _("This shows the current tool in use on the document."));

        /* and arrow */
        gtk_tooltips_set_tip (GTK_TOOLTIPS (priv->tooltips), GTK_WIDGET (priv->arrow),
			      _("Current tool"),
			      _("This shows the current tool in use on the document."));
}

/*
 * Setup control accessibility 
 */
static void
gpdf_tool_control_setup_at (GPdfToolControl *control)
{
	AtkObject *accessible;

	accessible = gtk_widget_get_accessible (GTK_WIDGET (control));
	atk_object_set_name (accessible, _("Current Tool"));
}

/*
 * Allocate private struct and setup control widgets
 */
void
gpdf_tool_control_instance_init (GPdfToolControl *control)
{
        GPdfToolControlPrivate *priv = g_new0 (GPdfToolControlPrivate, 1);
        control->priv = priv;

	gpdf_tool_control_setup_widgets (control);
	gpdf_tool_control_setup_tooltips (control);
	gpdf_tool_control_setup_at (control);
}



/**
 * gpdf_tool_control_set_tool:
 * @control: the #GPdfToolControl control
 * @tool: the #GPdfTool tool to activate
 *
 * Set the tool @tool as the current tool
 */
void
gpdf_tool_control_set_tool (GPdfToolControl *control, GPdfTool tool)
{
    GPdfToolControlPrivate *priv;
    GPdfTool old;

    g_return_if_fail (GPDF_IS_TOOL_CONTROL (control));

    priv = control->priv;

    if (priv->current_tool != tool)
    {
        old = priv->current_tool;
        priv->current_tool = tool;
        gpdf_tool_control_emit_tool_changed (control, tool, old);
    }
}

/**
 * gpdf_tool_control_get_tool:
 * @control: the #GPdfToolControl control
 *
 * Get the current tool
 */
GPdfTool
gpdf_tool_control_get_tool (GPdfToolControl *control)
{
    GPdfToolControlPrivate *priv;

    g_return_val_if_fail (GPDF_IS_TOOL_CONTROL (control),
                          (GPdfTool)NULL);

    priv = control->priv;

    return priv->current_tool;
}

