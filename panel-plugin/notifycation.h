
#ifndef __NOTIFYCATION_H__
#define __NOTIFYCATION_H__

G_BEGIN_DECLS
#include <libnotify/notify.h>

NotifyNotification *t_notify_new(void);
void t_notify_send(NotifyNotification *n);
void t_notify_close(NotifyNotification *n);
void t_notify_destroy(NotifyNotification *n);

G_END_DECLS

#endif /* !__NOTIFYCATION_H__ */
