/*  $Id$
 *
 *  Copyright (C) 2019 John Doo <john@foo.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>

#include "menu-items.h"
#include "pomodoro-timer.h"
#include "sample-dialogs.h"
#include "sample.h"
#include "setting.h"
#include "notifycation.h"

/* default settings */

/* prototypes */
static void sample_construct(XfcePanelPlugin *plugin);
/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER(sample_construct);

static SamplePlugin *
sample_new(XfcePanelPlugin *plugin)
{
    SamplePlugin *sample;
    GtkOrientation orientation;
    GtkWidget *drawing_area;
    /* allocate memory for the plugin structure */
    sample = g_slice_new0(SamplePlugin);
    sample->timer = g_slice_new0(Timer);
    sample->setting = g_slice_new0(Setting);
    sample->setting->pomodoro_color = g_slice_new0(GdkRGBA);
    sample->setting->break_color = g_slice_new0(GdkRGBA);
    sample->setting->pause_color = g_slice_new0(GdkRGBA);
    sample->timer->notify = t_notify_new();
    sample->timer->color = sample->setting->pomodoro_color;
    sample->timer->setting = sample->setting;

    /* pointer to plugin */
    sample->plugin = plugin;

    /* read the user settings */
    config_read(plugin, sample->setting);
    DBG("%d", sample->setting->pomodoro_time);
    DBG("%f", sample->setting->pomodoro_color->alpha);
    DBG("%d", sample->setting->break_time);
    DBG("%f", sample->setting->break_color->alpha);
    DBG("%d", sample->setting->longer_break);
    DBG("%f", sample->setting->pause_color->alpha);
    //then set
    timer_set(sample->timer);
    /* get the current orientation */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* create some panel widgets */
    sample->ebox = gtk_event_box_new();
    gtk_widget_show(sample->ebox);

    sample->hvbox = gtk_box_new(orientation, 0);
    gtk_widget_show(sample->hvbox);
    gtk_container_add(GTK_CONTAINER(sample->ebox), sample->hvbox);

    /* some sample widgets */
    //drawbox
    sample->drawbox = gtk_box_new(orientation, 10);
    gtk_widget_show(sample->drawbox);
    gtk_container_add(GTK_CONTAINER(sample->hvbox), sample->drawbox);

    sample->timer->drawbox = sample->drawbox;

    //drawarea
    drawing_area = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(sample->drawbox), drawing_area, FALSE, FALSE, 0);
    gtk_widget_add_events(sample->drawbox, GDK_BUTTON_PRESS_MASK);
    DBG("sample_new success");
    return sample;
}

static void
sample_free(XfcePanelPlugin *plugin,
            SamplePlugin *sample)
{
    GtkWidget *dialog;

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data(G_OBJECT(plugin), "dialog");
    if (G_UNLIKELY(dialog != NULL))
        gtk_widget_destroy(dialog);

    /* destroy the panel widgets */
    gtk_widget_destroy(sample->hvbox);
    gtk_widget_destroy(sample->drawbox);

    //uninit notify
    notify_uninit();
    t_notify_destroy(sample->timer->notify);
    /* cleanup the settings */

    /* free the plugin structure */
    g_slice_free(SamplePlugin, sample);
    g_slice_free(Timer, sample->timer);
    g_slice_free(Setting, sample->setting);
    g_slice_free(GdkRGBA, sample->setting->pomodoro_color);
    g_slice_free(GdkRGBA, sample->setting->break_color);
    g_slice_free(GdkRGBA, sample->setting->pause_color);
}

static void
sample_orientation_changed(XfcePanelPlugin *plugin,
                           GtkOrientation orientation,
                           SamplePlugin *sample)
{
    /* change the orientation of the box */
    gtk_orientable_set_orientation(GTK_ORIENTABLE(sample->hvbox), orientation);
}

static gboolean
sample_size_changed(XfcePanelPlugin *plugin,
                    gint size,
                    SamplePlugin *sample)
{
    GtkOrientation orientation;

    /* get the orientation of the plugin */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* set the widget size */
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        gtk_widget_set_size_request(GTK_WIDGET(plugin), -1, size);
    else
        gtk_widget_set_size_request(GTK_WIDGET(plugin), size, -1);
    //set drawbox size
    gtk_widget_set_size_request(GTK_WIDGET(sample->drawbox), size, size);

    /* we handled the orientation */
    return TRUE;
}

static void add_menu(XfcePanelPlugin *plugin, SamplePlugin *sample)
{
    GtkMenuItem *work_menu, *break_menu, *reset;
    work_menu = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Work mode"));
    break_menu = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Break mode"));
    reset = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Reset work count"));
    //add to xfce menu
    add_separator_and_label_with_markup(plugin, "<b>Settings</b>");
    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(work_menu));
    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(break_menu));
    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(reset));
    add_separator_and_label_with_markup(plugin, "");

    g_signal_connect(G_OBJECT(work_menu), "activate", G_CALLBACK(to_work_mode), sample->timer);
    g_signal_connect(G_OBJECT(break_menu), "activate", G_CALLBACK(to_break_mode), sample->timer);
    g_signal_connect(G_OBJECT(reset), "activate", G_CALLBACK(pmdr_count_reset), sample->timer);

    gtk_widget_show(GTK_WIDGET(work_menu));
    gtk_widget_show(GTK_WIDGET(break_menu));
    gtk_widget_show(GTK_WIDGET(reset));
}

static void
sample_construct(XfcePanelPlugin *plugin)
{
    static SamplePlugin *sample;
    /* setup transation domain */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    //init notify
    notify_init(PACKAGE_NAME);

    /* create the plugin */
    sample = sample_new(plugin);

    //DEBUG SPACE
    //sample->timer->pmdr_count = 3;

    //DEBUG SPACE

    /* add the ebox to the panel */
    gtk_container_add(GTK_CONTAINER(plugin), sample->ebox);
    /* show the panel's right-click menu on this ebox */
    xfce_panel_plugin_add_action_widget(plugin, sample->ebox);
    add_menu(plugin, sample);

    /* connect plugin signals */
    g_signal_connect(G_OBJECT(plugin), "free-data",
                     G_CALLBACK(sample_free), sample);

    g_signal_connect(G_OBJECT(plugin), "save",
                     G_CALLBACK(config_save), sample->setting);

    g_signal_connect(G_OBJECT(plugin), "size-changed",
                     G_CALLBACK(sample_size_changed), sample);

    g_signal_connect(G_OBJECT(plugin), "orientation-changed",
                     G_CALLBACK(sample_orientation_changed), sample);

    g_signal_connect(G_OBJECT(sample->drawbox), "draw",
                     G_CALLBACK(draw_callback), sample->timer);

    g_signal_connect(G_OBJECT(sample->ebox), "button_press_event",
                     G_CALLBACK(click_handler), sample->timer);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure(plugin);
    g_signal_connect(G_OBJECT(plugin), "configure-plugin",
                     G_CALLBACK(sample_configure), sample);

    /* show the about menu item and connect signal */
    xfce_panel_plugin_menu_show_about(plugin);
    g_signal_connect(G_OBJECT(plugin), "about",
                     G_CALLBACK(sample_about), NULL);

    DBG("sample_construct success");
}
