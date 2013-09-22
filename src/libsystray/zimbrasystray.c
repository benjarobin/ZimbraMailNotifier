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

#include "img/iconnewmail.h"
#include "img/iconnomail.h"
#include "img/iconnotconnected.h"

static GtkStatusIcon *statusIcon = NULL;
static GdkPixbuf *bufIconNewMail = NULL;
static GdkPixbuf *bufIconNoMail = NULL;
static GdkPixbuf *bufIconNotConnected = NULL;

uint32_t ZIMBRA_SYSTRAY_init(void)
{
    // Create systray icon
    if (statusIcon == NULL)
    {
        statusIcon = gtk_status_icon_new();
    }
    // Load images from memory
    if (bufIconNewMail == NULL)
    {
        bufIconNewMail = gdk_pixbuf_new_from_inline (-1, iconnewmail_inline, FALSE, NULL);
    }
    if (bufIconNoMail == NULL)
    {
        bufIconNoMail = gdk_pixbuf_new_from_inline (-1, iconnomail_inline, FALSE, NULL);
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

    if (bufIconNewMail)
    {
        g_object_unref(bufIconNewMail);
        bufIconNewMail = NULL;
    }
    else
    {
        ret++;
    }

    if (bufIconNoMail)
    {
        g_object_unref(bufIconNoMail);
        bufIconNoMail = NULL;
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
            if (bufIconNoMail)
            {
                gtk_status_icon_set_from_pixbuf(statusIcon, bufIconNoMail);
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
    char buf[128];
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
        if (bufIconNoMail)
        {
            gtk_status_icon_set_from_pixbuf(statusIcon, bufIconNoMail);
        }
    }
    else
    {
        gtk_status_icon_set_blinking(statusIcon, 1);
        if (bufIconNewMail)
        {
            gtk_status_icon_set_from_pixbuf(statusIcon, bufIconNewMail);
        }
    }
}
