#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>

#include "menu-items.h"

void add_separator_and_label_with_markup(XfcePanelPlugin* plugin, gchar* lbl)
{
    GtkWidget *separator, *menuitem, *label;
    separator = gtk_separator_menu_item_new();
    menuitem = gtk_menu_item_new_with_label(lbl);
    gtk_widget_set_sensitive(menuitem, FALSE);
    label = gtk_bin_get_child(GTK_BIN(menuitem));
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(separator));
    xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(menuitem));
    gtk_widget_show(separator);
    gtk_widget_show(menuitem);
}