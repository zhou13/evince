/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-indent-level: 8; c-basic-offset: 8 -*- */
/* 
 *  Copyright (C) 2003 Remi Cohen-Scali
 *
 *  Author:
 *    Remi Cohen-Scali <Remi@Cohen-Scali.com>
 *
 * GPdf is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GPdf is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef THUMB_H
#define THUMB_H

#include <aconf.h>

class XRef;
class Gfx;

/*
 * Thumb
 */

class Thumb {
      public:
        Thumb(XRef *xrefA, Object *obj);
        ~Thumb();

        Object *getWidth(Object *obj) {return width.copy(obj); };
        Object *getHeight(Object *obj) {return height.copy(obj); };
        Object *getColorSpace(Object *obj) {return colorspace.copy(obj); };
        Object *getBitsPerComponent(Object *obj) {return bitspercomponent.copy(obj); };

        Dict *getDict() {return dict; };

      private:

        XRef *xref;
        Dict *dict;
        Object width, height, colorspace, bitspercomponent;
};

#endif

