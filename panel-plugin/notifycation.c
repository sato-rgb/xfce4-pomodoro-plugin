#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>

#include "notifycation.h"

NotifyNotification *t_notify_new(void)
{
    return notify_notification_new("The timer has ended",
                                   "This notification will disappear when timer newly started.",
                                   "xfce4-pomodoro-plugin");
}

void t_notify_send(NotifyNotification *n)
{
    notify_notification_set_timeout(n, NOTIFY_EXPIRES_NEVER);
    notify_notification_show(n, NULL);
}

void t_notify_close(NotifyNotification *n)
{
    notify_notification_close(n, NULL);
}

void t_notify_destroy(NotifyNotification *n)
{
    g_object_unref(G_OBJECT(n));
}