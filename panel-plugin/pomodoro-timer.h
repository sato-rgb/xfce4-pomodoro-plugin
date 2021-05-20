#ifndef __COUNTDOWN_H__
#define __COUNTDOWN_H__
G_BEGIN_DECLS

#include <glib.h>
#include <libnotify/notify.h>

#include "setting.h"

//it means just kinds, doesn't mean actual time
typedef enum
{
    WORK = 0,
    USUAL_BREAK = 1,
    LONGER_BREAK = 2
} TimerModes;

typedef struct
{
    //time
    guint set;
    //time count
    guint count;

    //color
    GdkRGBA *color;

    //state(s)
    //current mode
    TimerModes mode;
    //determine break time
    guint pmdr_count;
    //id of glib event source
    guint id;

    //Same pointer as the sample struct
    Setting *setting;
    //Same pointer as the sample struct
    GtkWidget *drawbox;
    //notifycation
    NotifyNotification *notify;
} Timer;
// prototype(s)

//menu callbacks
void to_work_mode(GtkWidget *widget, Timer *timer);
void to_break_mode(GtkWidget *widget, Timer *timer);
void pmdr_count_reset(GtkWidget *widget, Timer *timer);
//handlers
void click_handler(GtkWidget *widget, GdkEventButton *event, Timer *timer);

void timer_set(Timer *t);
TimerModes get_break_mode(Timer *t);

void pause_timer_color(Timer *t);
void resume_timer_color(Timer *timer);
void clean_id(Timer *t);

gboolean count_down(Timer *timer);
gboolean draw_callback(GtkWidget *widget, cairo_t *cr, Timer *t);

//notifycation

//statics

G_END_DECLS
#endif /* !__COUNTDOWN_H__ */