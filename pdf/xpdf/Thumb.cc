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
#include "GfxState.h"
#include "Thumb.h"

/*
 * ThumbColorMap
 */
ThumbColorMap::ThumbColorMap(int bitsA,
                             Object *obj,
                             GfxColorSpace *csA) :
  bits(bitsA),
  str(NULL),
  length(0), 
  cs(csA)
{
        Object obj1, obj2; 
        GfxIndexedColorSpace *iCS;
        GfxSeparationColorSpace *sepCS;
        int maxPixel, indexHigh;
        Dict *streamDict;
        int n;
        int baseNComps; 
        /* LZW params */
        int colors = 0, early = 0;
        /* CCITTFax params */
        int encoding = 0, rows = 0;
        GBool eol = gFalse, byteAlign = gFalse, eob = gFalse, black = gFalse;
        /* Common params */
        int pred = 0, cols = 0; 

        ok = gFalse;
        maxPixel = (1 << bits) - 1;

        do {
                if (!obj->isStream ()) {
                        printf ("Error: Invalid object of type %s\n",
                                obj->getTypeName ()); 
                        break; 
                }
                str = obj->getStream(); 

                streamDict = obj->streamGetDict ();

                streamDict->lookupNF ("Filter", &obj1);
                if (!obj1.isArray ()) {
                        printf ("Error: Invalid filter object of type %s\n",
                                obj1.getTypeName ()); 
                        break;                         
                }

                str = str->addFilters(obj);
#if 0
                for (n = 0; n < obj1.arrayGetLength (); n++) {
                        if (obj1.arrayGet (n, &obj2)->isName("ASCII85Decode")) {
                                str = new ASCII85Stream(str); 
                        }
                        else if (obj1.arrayGet (n, &obj2)->isName("ASCIIHexDecode")) {
                                str = new ASCIIHexStream(str); 
                        }
                        else if (obj1.arrayGet (n, &obj2)->isName("LZWDecode")) {
                                /* FIXME: Init other params by reading them */
                                str = new LZWStream(str, pred, cols, colors, bits, early); 
                        }
                        else if (obj1.arrayGet (n, &obj2)->isName("RunLengthDecode")) {
                                str = new RunLengthStream(str); 
                        }
                        else if (obj1.arrayGet (n, &obj2)->isName("CCITTFaxDecode")) {
                                /* FIXME: Init other params by reading them */
                                str = new CCITTFaxStream(str, encoding,
                                                         eol, byteAlign,
                                                         cols, rows,
                                                         eob, black); 
                        }
                        else if (obj1.arrayGet (n, &obj2)->isName("DCTDecode")) {
                                str = new DCTStream(str); 
                        }
                        else if (obj1.arrayGet (n, &obj2)->isName("FlateDecode")) {
                                /* FIXME: Init other params by reading them */
                                str = new FlateStream(str, pred, cols, colors, bits); 
                        }
                        else  {
                                printf("Error: Unknown Decoding filter %s", obj1.getName());
                                break; 
                        }
                        obj2.free (); 
                }
                obj1.free();
#endif

		streamDict->lookup ("Length", &obj1);
		if (obj1.isNull ())
		{
			printf ("Error: No Length object\n"); 
                        break; 
		}
		if (!obj1.isInt ()) {
			printf ("Error: Invalid Width object %s\n",
				obj1.getTypeName ());
			obj1.free ();
			break;
		}
		length = obj1.getInt ();
		obj1.free ();

                nComps = cs->getNComps();

                if (cs->getMode () == csIndexed) {
                        iCS = (GfxIndexedColorSpace *)cs;
                        baseNComps = iCS->getBase ()->getNComps ();
                        str->reset(); 
                        if (iCS->getBase ()->getMode () == csDeviceGray) {
                                gray = (double *)gmalloc(sizeof(double) * (iCS->getIndexHigh () + 1));
                                for (n = 0; n < iCS->getIndexHigh (); n++) {
                                        double comp = (double)str->getChar(); 
                                        printf ("Gray pixel [%03d] = %02x\n", n, (int)comp); 
                                        gray[n] = comp / 255.0; 
                                }
                        }
                        else if (iCS->getBase ()->getMode () == csDeviceRGB) {
                                rgb = (GfxRGB *)gmalloc(sizeof(GfxRGB) * (iCS->getIndexHigh () + 1));
                                for (n = 0; n < iCS->getIndexHigh (); n++) {
                                        double comp_r = (double)str->getChar(); 
                                        double comp_g = (double)str->getChar(); 
                                        double comp_b = (double)str->getChar(); 
                                        printf ("RGB pixel [%03d] = (%02x,%02x,%02x)\n",
                                                n, (int)comp_r, (int)comp_g, (int)comp_b); 
                                        rgb[n].r = comp_r / 255.0; 
                                        rgb[n].g = comp_g / 255.0; 
                                        rgb[n].b = comp_b / 255.0; 
                                }
                        }
                        else if (iCS->getBase ()->getMode () == csDeviceCMYK) {
                                cmyk = (GfxCMYK *)gmalloc(sizeof(GfxCMYK) * (iCS->getIndexHigh () + 1));
                                for (n = 0; n < iCS->getIndexHigh (); n++) {
                                        double comp_c = (double)str->getChar(); 
                                        double comp_m = (double)str->getChar(); 
                                        double comp_y = (double)str->getChar(); 
                                        double comp_k = (double)str->getChar(); 
                                        printf ("CMYK pixel [%03d] = (%02x,%02x,%02x,%02x)\n",
                                                n, (int)comp_c, (int)comp_m, (int)comp_y, (int)comp_k); 
                                        cmyk[n].c = comp_c / 255.0; 
                                        cmyk[n].m = comp_m / 255.0; 
                                        cmyk[n].y = comp_y / 255.0; 
                                        cmyk[n].k = comp_k / 255.0; 
                                }
                        }
                }
                else if (cs->getMode () == csSeparation) {
                        sepCS = (GfxSeparationColorSpace *)cs; 
                        /* FIXME: still to do */
                }

                ok = gTrue;
        }
        while (0); 
}

ThumbColorMap::~ThumbColorMap()
{
        delete str;
        gfree((void *)gray); 
}

/*
 * Thumb
 */

Thumb::Thumb(XRef *xrefA, Object *obj) :
  xref(xrefA),
  str(NULL)
{
	Object obj1, obj2;
	Dict *dict;

	do {
		/* Get stream dict */
		dict = obj->streamGetDict ();
                str = obj->getStream(); 
		
		/* Get width */
		dict->lookup ("Width", &obj1);
		if (obj1.isNull ())
		{
			obj1.free ();
			dict->lookup ("W", &obj1);
		}
		if (!obj1.isInt ()) {
			printf ("Error: Invalid Width object %s\n",
				obj1.getTypeName ());
			obj1.free ();
			break;
		}
		width = obj1.getInt ();
		obj1.free ();
		
		/* Get heigth */
		dict->lookup ("Height", &obj1);
		if (obj1.isNull ()) 
		{
			obj1.free ();
			dict->lookup ("H", &obj1);
		}
		if (!obj1.isInt ()) {
			printf ("Error: Invalid Height object %s\n",
				obj1.getTypeName ());
			obj1.free ();
			break;
		}
		height = obj1.getInt ();
		obj1.free ();
		
		/* bit depth */
		dict->lookup ("BitsPerComponent", &obj1);
		if (obj1.isNull ())
		{
			obj1.free ();
			dict->lookup ("BPC", &obj1);
		}
		if (!obj1.isInt ()) {
			printf ("Error: Invalid BitsPerComponent object %s\n",
				obj1.getTypeName ());
			obj1.free ();
			break;
		}
		bits = obj1.getInt ();
		obj1.free ();
		
		/* Get color space */
		dict->lookup ("ColorSpace", &obj1);
		if (obj1.isNull ()) 
		{
			obj1.free ();
			dict->lookup ("CS", &obj1);
		}
		if (!(gfxCS = GfxColorSpace::parse (&obj1)))
		{
			printf ("Error: Cannot parse color space\n");
			obj1.free ();
			break;
		}
		
		dict->lookup ("Length", &obj1);
		if (!obj1.isInt ()) {
			printf ("Error: Invalid Length Object %s\n",
				obj1.getTypeName ());
			obj1.free ();
			break;			
		}
		length = obj1.getInt ();
		obj1.free ();

                thumbCM = new ThumbColorMap (bits, obj, gfxCS);
	}
	while (0);	
}

Thumb::~Thumb() {
        delete thumbCM;
        delete str; 
}

