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

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
#include <string.h>

#include "sample-dialogs.h"
#include "sample.h"
#include "setting.h"

/* the website url */
#define PLUGIN_WEBSITE "https://docs.xfce.org/panel-plugins/xfce4-sample-plugin"

static void setting_apply(SampleWithDialogs *swd)
{
    Setting *setting;
    setting = swd->sample->setting;
    //setting apply
    //pomodoro
    setting->pomodoro_time = gtk_adjustment_get_value(swd->pmdr_adj) * ONE_MINUTE;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(swd->colpick_pmdr_color), setting->pomodoro_color);
    //break
    setting->break_time = gtk_adjustment_get_value(swd->break_adj) * ONE_MINUTE;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(swd->colpick_break_color), setting->break_color);
    //longer break
    setting->longer_break = gtk_adjustment_get_value(swd->lonbre_adj) * ONE_MINUTE;
    //pause color
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(swd->colpick_pause_color), setting->pause_color);
    //scales
    setting->circle_scale = gtk_range_get_value(GTK_RANGE(swd->scale_circle_scale));
    setting->text_scale = gtk_range_get_value(GTK_RANGE(swd->scale_text_scale));
    //drawing area refresh
    gtk_widget_queue_draw(swd->sample->drawbox);
}

static gboolean sample_configure_scale_response(GtkRange *range,
                                                GtkScrollType scroll,
                                                double value,
                                                SampleWithDialogs *swd)
{
    setting_apply(swd);
    return FALSE;
}
static void sample_configure_color_response(GtkColorButton *widget,
                                            SampleWithDialogs *swd)
{
    setting_apply(swd);
}
static void sample_configure_dialog_response(GtkWidget *dialog,
                                             gint response,
                                             SampleWithDialogs *swd)
{
    gboolean result;

    if (response == GTK_RESPONSE_HELP)
    {
        /* show help */
        result = g_spawn_command_line_async("exo-open --launch WebBrowser " PLUGIN_WEBSITE, NULL);

        if (G_UNLIKELY(result == FALSE))
            g_warning(_("Unable to open the following url: %s"), PLUGIN_WEBSITE);
    }
    else
    {
        setting_apply(swd);

        /* remove the dialog data from the plugin */
        g_object_set_data(G_OBJECT(swd->sample->plugin), "dialog", NULL);

        /* unlock the panel menu */
        xfce_panel_plugin_unblock_menu(swd->sample->plugin);

        /* save the plugin */
        config_save(swd->sample->plugin, swd->sample->setting);

        /* destroy the properties dialog */
        gtk_widget_destroy(dialog);

        g_free(swd);
        DBG("ok");
    }
}

void sample_configure(XfcePanelPlugin *plugin,
                      SamplePlugin *sample)
{
    SampleWithDialogs *swd;
    Setting *setting;
    GtkWidget *dialog;
    GtkWidget *label, *vbox, *table;

    swd = g_new0(SampleWithDialogs, 1);
    swd->sample = sample;
    setting = sample->setting;
    /* block the plugin menu */
    xfce_panel_plugin_block_menu(plugin);

    /* create the dialog */
    dialog = xfce_titled_dialog_new_with_buttons(_("Sample Plugin"),
                                                 GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(plugin))),
                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 "gtk-help", GTK_RESPONSE_HELP,
                                                 "gtk-close", GTK_RESPONSE_CLOSE,
                                                 NULL);
    //configs UI
    swd->pmdr_adj = gtk_adjustment_new((setting->pomodoro_time / ONE_MINUTE), 1.0, 10000.0, 1.0, 5.0, 0.0);
    swd->break_adj = gtk_adjustment_new((setting->break_time / ONE_MINUTE), 1.0, 10000.0, 1.0, 5.0, 0.0);
    swd->lonbre_adj = gtk_adjustment_new((setting->longer_break / ONE_MINUTE), 1.0, 10000.0, 1.0, 5.0, 0.0);
    //box
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_show(vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), vbox, TRUE, TRUE, 0);
    //grid
    table = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(table), 12);
    gtk_grid_set_row_spacing(GTK_GRID(table), 6);
    //Settings
    label = gtk_label_new("work time:(m)");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 0, 1, 1);
    swd->spinb_pmdr_time = gtk_spin_button_new(swd->pmdr_adj, 1.0, 0);
    gtk_grid_attach(GTK_GRID(table), swd->spinb_pmdr_time, 1, 0, 1, 1);

    label = gtk_label_new("work color");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 1, 1, 1);
    swd->colpick_pmdr_color = gtk_color_button_new_with_rgba(setting->pomodoro_color);
    gtk_widget_set_hexpand(swd->colpick_pmdr_color, TRUE);
    gtk_grid_attach(GTK_GRID(table), swd->colpick_pmdr_color, 1, 1, 1, 1);

    label = gtk_label_new("break time:(m)");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 2, 1, 1);
    swd->spinb_break_time = gtk_spin_button_new(swd->break_adj, 1.0, 0);
    gtk_grid_attach(GTK_GRID(table), swd->spinb_break_time, 1, 2, 1, 1);

    label = gtk_label_new("break color");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 3, 1, 1);
    swd->colpick_break_color = gtk_color_button_new_with_rgba(setting->break_color);
    gtk_widget_set_hexpand(swd->colpick_break_color, TRUE);
    gtk_grid_attach(GTK_GRID(table), swd->colpick_break_color, 1, 3, 1, 1);

    label = gtk_label_new("longer break:(m)");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 4, 1, 1);
    swd->spinb_longer_break = gtk_spin_button_new(swd->lonbre_adj, 1.0, 0);
    gtk_grid_attach(GTK_GRID(table), swd->spinb_longer_break, 1, 4, 1, 1);

    label = gtk_label_new("pause color");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 5, 1, 1);
    swd->colpick_pause_color = gtk_color_button_new_with_rgba(setting->pause_color);
    gtk_widget_set_hexpand(swd->colpick_pause_color, TRUE);
    gtk_grid_attach(GTK_GRID(table), swd->colpick_pause_color, 1, 5, 1, 1);

    //scales
    label = gtk_label_new("Circle size (%)");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 6, 1, 1);
    swd->scale_circle_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
    gtk_range_set_value(GTK_RANGE(swd->scale_circle_scale), setting->circle_scale);
    gtk_widget_set_hexpand(swd->scale_circle_scale, TRUE);
    gtk_grid_attach(GTK_GRID(table), swd->scale_circle_scale, 1, 6, 1, 1);

    label = gtk_label_new("Work count size (%)");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(table), label, 0, 7, 1, 1);
    swd->scale_text_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
    gtk_range_set_value(GTK_RANGE(swd->scale_text_scale), setting->text_scale);
    gtk_widget_set_hexpand(swd->scale_text_scale, TRUE);
    gtk_grid_attach(GTK_GRID(table), swd->scale_text_scale, 1, 7, 1, 1);

    //put grid into the box
    gtk_widget_show_all(table);
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);
    //configs UI end

    /* center dialog on the screen */
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    /* set dialog icon */
    gtk_window_set_icon_name(GTK_WINDOW(dialog), "xfce4-settings");

    /* link the dialog to the plugin, so we can destroy it when the plugin
   * is closed, but the dialog is still open */
    g_object_set_data(G_OBJECT(plugin), "dialog", dialog);

    /* connect the response signal to the dialog */
    g_signal_connect(G_OBJECT(dialog), "response",
                     G_CALLBACK(sample_configure_dialog_response), swd);
    //reflesh drawbox when change the config
    g_signal_connect(G_OBJECT(swd->colpick_break_color), "color-set",
                     G_CALLBACK(sample_configure_color_response), swd);
    g_signal_connect(G_OBJECT(swd->colpick_pause_color), "color-set",
                     G_CALLBACK(sample_configure_color_response), swd);
    g_signal_connect(G_OBJECT(swd->colpick_pmdr_color), "color-set",
                     G_CALLBACK(sample_configure_color_response), swd);

    g_signal_connect(G_OBJECT(swd->scale_circle_scale), "change-value",
                     G_CALLBACK(sample_configure_scale_response), swd);

    g_signal_connect(G_OBJECT(swd->scale_text_scale), "change-value",
                     G_CALLBACK(sample_configure_scale_response), swd);

    /* show the entire dialog */
    gtk_widget_show(dialog);

    DBG("sample_configure success");
}

void sample_about(XfcePanelPlugin *plugin)
{
    /* about dialog code. you can use the GtkAboutDialog
   * or the XfceAboutInfo widget */
    GdkPixbuf *icon;

    const gchar *auth[] = {
        "Xfce development team <xfce4-dev@xfce.org>",
        NULL};

    icon = xfce_panel_pixbuf_from_source("xfce4-pomodoro-plugin", NULL, 32);
    gtk_show_about_dialog(NULL,
                          "logo", icon,
                          "license", xfce_get_license_text(XFCE_LICENSE_TEXT_GPL),
                          "version", PACKAGE_VERSION,
                          "program-name", PACKAGE_NAME,
                          "comments", _("This is a sample plugin"),
                          "website", PLUGIN_WEBSITE,
                          "copyright", _("Copyright \xc2\xa9 2006-2019 Xfce development team\n"),
                          "authors", auth,
                          NULL);

    if (icon)
        g_object_unref(G_OBJECT(icon));
}
