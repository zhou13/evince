/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* Tool number entry
 *
 * Copyright (C) 2003 Martin Kretzschmar
 *
 * Author:
 *   Martin Kretzschmar <Martin.Kretzschmar@inf.tu-dresden.de>
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

#ifndef TOOL_CONTROL_H
#define TOOL_CONTROL_H

#include <gtk/gtkhbox.h>

G_BEGIN_DECLS

#define GPDF_TYPE_TOOL_CONTROL            (gpdf_tool_control_get_type ())
#define GPDF_TOOL_CONTROL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GPDF_TYPE_TOOL_CONTROL, GPdfToolControl))
#define GPDF_TOOL_CONTROL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GPDF_TYPE_TOOL_CONTROL, GPdfToolControlClass))
#define GPDF_IS_TOOL_CONTROL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GPDF_TYPE_TOOL_CONTROL))
#define GPDF_IS_TOOL_CONTROL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GPDF_TYPE_TOOL_CONTROL))

typedef struct _GPdfToolControl        GPdfToolControl;
typedef struct _GPdfToolControlClass   GPdfToolControlClass;
typedef struct _GPdfToolControlPrivate GPdfToolControlPrivate;

typedef enum _GPdfTool {
    GPDF_TOOL_CTRL_DRAG = 1,
    GPDF_TOOL_CTRL_SELECT,
    GPDF_NB_TOOLS
} GPdfTool;

struct _GPdfToolControl {
	GtkButton parent;
        
        GPdfToolControlPrivate *priv;
};

struct _GPdfToolControlClass {
        GtkButtonClass parent_class;
        
        void (*tool_changed) (GPdfToolControl *control, GPdfTool newtool, GPdfTool oldtool);
};

GType 	 gpdf_tool_control_get_type (void);
void  	 gpdf_tool_control_set_tool (GPdfToolControl *control, GPdfTool tool);
GPdfTool gpdf_tool_control_get_tool (GPdfToolControl *control);

G_END_DECLS

#endif /* TOOL_CONTROL_H */
