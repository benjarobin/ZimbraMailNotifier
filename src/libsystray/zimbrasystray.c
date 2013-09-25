/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Zimbra Mail Notifier.
 *
 * The Initial Developer of the Original Code is
 * David GUEHENNEC.
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "zimbrasystray.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "img/iconnomail.h"
#include "img/iconnotconnected.h"


/********************** macro ***********************************/

#define UNUSED(v) (void)(v)

/********************** static var ***********************************/

static GtkStatusIcon *statusIcon = NULL;
static unsigned int previousNbUnreadMail = 0;
static GdkPixbuf *bufIconUnreadMail = NULL;
static GdkPixbuf *bufIconConnected = NULL;
static GdkPixbuf *bufIconNotConnected = NULL;

/****************** static function **********************************/

static cairo_surface_t* gdkPixbuf2CairoSurface(const GdkPixbuf *pixbuf);
static GdkPixbuf* cairoSurface2gdkPixbuf(cairo_surface_t *surface);
static void drawNumberBottomLeft(cairo_surface_t *surface, unsigned int v);

/****************** public function **********************************/

uint32_t ZIMBRA_SYSTRAY_init(void)
{
    // Create systray icon
    if (statusIcon == NULL)
    {
        statusIcon = gtk_status_icon_new();
    }
    // Load images from memory
    if (bufIconConnected == NULL)
    {
        bufIconConnected = gdk_pixbuf_new_from_inline (-1, iconnomail_inline, FALSE, NULL);
    }
    if (bufIconNotConnected == NULL)
    {
        bufIconNotConnected = gdk_pixbuf_new_from_inline (-1, iconnotconnected_inline, FALSE, NULL);
    }

    ZIMBRA_SYSTRAY_setConnected(0);
    return (statusIcon == NULL);
}

uint32_t ZIMBRA_SYSTRAY_unload(void)
{
    int ret = 0;
    if (statusIcon)
    {
        gtk_status_icon_set_visible(statusIcon, 0);
        g_object_unref(statusIcon);
        statusIcon = NULL;
    }
    else
    {
        ret++;
    }

    previousNbUnreadMail = 0;
    if (bufIconUnreadMail)
    {
        g_object_unref(bufIconUnreadMail);
        bufIconUnreadMail = NULL;
    }

    if (bufIconConnected)
    {
        g_object_unref(bufIconConnected);
        bufIconConnected = NULL;
    }
    else
    {
        ret++;
    }

    if (bufIconNotConnected)
    {
        g_object_unref(bufIconNotConnected);
        bufIconNotConnected = NULL;
    }
    else
    {
        ret++;
    }

    return ret;
}

void ZIMBRA_SYSTRAY_setConnected(uint32_t connected)
{
    if (statusIcon)
    {
        if (connected)
        {
            gtk_status_icon_set_tooltip_text(statusIcon, "Connecté");
            if (bufIconConnected)
            {
                gtk_status_icon_set_from_pixbuf(statusIcon, bufIconConnected);
            }
        }
        else
        {
            gtk_status_icon_set_tooltip_text(statusIcon, "Non connecté");
            if (bufIconNotConnected)
            {
                gtk_status_icon_set_from_pixbuf(statusIcon, bufIconNotConnected);
            }
        }

        gtk_status_icon_set_visible(statusIcon, 1);
    }
}

void ZIMBRA_SYSTRAY_setUnreadMail(uint32_t nb)
{
    char buf[64];
    if (nb > 1)
    {
        sprintf(buf, "%u messages non lus", nb);
    }
    else
    {
        sprintf(buf, "%u message non lu", nb);
    }

    gtk_status_icon_set_tooltip_text(statusIcon, buf);

    if (nb == 0)
    {
        gtk_status_icon_set_blinking(statusIcon, 0);
        if (bufIconConnected)
        {
            gtk_status_icon_set_from_pixbuf(statusIcon, bufIconConnected);
        }
    }
    else
    {
        gtk_status_icon_set_blinking(statusIcon, 1);
        if (bufIconUnreadMail && previousNbUnreadMail != nb)
        {
            g_object_unref(bufIconUnreadMail);
            bufIconUnreadMail = NULL;
        }
        if (bufIconUnreadMail == NULL)
        {
            cairo_surface_t *surface = gdkPixbuf2CairoSurface(bufIconConnected);
            drawNumberBottomLeft(surface, nb);
            bufIconUnreadMail = cairoSurface2gdkPixbuf(surface);
            cairo_surface_destroy(surface);
        }
        if (bufIconUnreadMail)
        {
            gtk_status_icon_set_from_pixbuf(statusIcon, bufIconUnreadMail);
        }
    }
}

/****************** static function **********************************/

static cairo_surface_t* gdkPixbuf2CairoSurface(const GdkPixbuf *pixbuf)
{
    cairo_surface_t *surface = NULL;
    cairo_t *cr;

    g_assert(pixbuf != NULL);
    g_assert(gdk_pixbuf_get_colorspace(pixbuf) == GDK_COLORSPACE_RGB);
    g_assert(gdk_pixbuf_get_bits_per_sample(pixbuf) == 8);
    g_assert(gdk_pixbuf_get_has_alpha(pixbuf));
    g_assert(gdk_pixbuf_get_n_channels(pixbuf) == 4);

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, gdk_pixbuf_get_width(pixbuf),
                                         gdk_pixbuf_get_height(pixbuf));
    if (surface)
    {
        cr = cairo_create(surface);
        gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
    }
    return surface;
}

static void gdkPixbufFree(guchar *pixels, gpointer data)
{
    UNUSED(data);

    if (pixels != NULL)
    {
        free(pixels);
    }
}

// function only valid for little endian processors...
static inline u_int32_t cairoPixel2gdkPixel(u_int32_t p)
{
    return ( p & 0xFF00FF00)        |
           ((p & 0x00FF0000) >> 16) |
           ((p & 0x000000FF) << 16);
}

static GdkPixbuf* cairoSurface2gdkPixbuf(cairo_surface_t *surface)
{
    const guchar *dst;
    u_int32_t *src, *p;
    int rstride, w, h, i, size;

    g_assert(surface != NULL);

    // read info from cairo surface
    rstride = cairo_image_surface_get_stride(surface);
    w = cairo_image_surface_get_width(surface);
    h = cairo_image_surface_get_height(surface);
    src = (u_int32_t*)cairo_image_surface_get_data(surface);

    g_assert((rstride & 0x03) == 0);

    // Alloc internal buffer for pixbuf
    size = rstride * h;
    p = (u_int32_t*)malloc(size);
    dst = (const guchar*)p;
    size /= 4;

    // Convert cairo pixels to gdk pixbuf pixels
    for (i = 0; i < size; ++i)
    {
        *p++ = cairoPixel2gdkPixel(*src++);
    }

    // Create pixbuf
    return gdk_pixbuf_new_from_data(dst, GDK_COLORSPACE_RGB, 1, 8, w, h,
                                    rstride, gdkPixbufFree, NULL);
}

static void drawNumberBottomLeft(cairo_surface_t *surface, unsigned int v)
{
    cairo_text_extents_t te;
    cairo_t *cr;
    char buf[4] = "99+";

    if (v < 100)
    {
        sprintf(buf, "%u", v);
    }

    cr = cairo_create(surface);

    cairo_select_font_face (cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 20);
    cairo_text_extents(cr, buf, &te);

    cairo_move_to(cr, 2 - te.x_bearing,
                  cairo_image_surface_get_height(surface) - te.height - 2 - te.y_bearing);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_line_width(cr, 3);
    cairo_text_path(cr, buf);
    cairo_stroke_preserve(cr);

    cairo_set_source_rgb(cr, 0.2, 0.0, 1.0);
    cairo_fill(cr);

    cairo_destroy(cr);
}
