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

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "Object.h"
#include "Gfx.h"
#include "Thumb.h"

/*
 * Thumb
 */

Thumb::Thumb(XRef *xrefA, Object *obj) :
  xref(xrefA)
{
        dict = new Dict(xref); 

        dict->add("Width", obj->dictLookupNF("Width", &width));
        dict->add("Height", obj->dictLookupNF("Height", &height));
        dict->add("ColorSpace", obj->dictLookupNF("ColorSpace", &colorspace));
        dict->add("BitsPerComponent", obj->dictLookupNF("BitsPerComponent", &bitspercomponent));
}

Thumb::~Thumb() {
  width.free();
  height.free();
  colorspace.free();
  bitspercomponent.free(); 
}

