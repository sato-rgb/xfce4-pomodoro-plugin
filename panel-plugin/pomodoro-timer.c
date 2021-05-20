#include <cairo.h>
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>
#include <stdio.h>

#include "notifycation.h"
#include "pomodoro-timer.h"
#include "sample.h"

//prototypes
static void forward_pmdr(Timer *t);
static void set_mode(Timer *t, TimerModes tm);
static void stop(Timer *timer);

static void PRINT_MODE(TimerModes tm)
{
    switch (tm)
    {
    case WORK:
        DBG("WORK");
        break;
    case USUAL_BREAK:
        DBG("USUAL_BREAK");
        break;
    case LONGER_BREAK:
        DBG("LONGER_BREAK");
        break;
    }
}

//emit draw signal
static void refresh(GtkWidget *drawbox)
{
    gtk_widget_queue_draw(drawbox);
}

//MAIN LOGIC BEGIN

//menu callbacks
void to_work_mode(GtkWidget *widget, Timer *timer)
{
    set_mode(timer, WORK);
    stop(timer);
    refresh(timer->drawbox);
    DBG("Work");
}

void to_break_mode(GtkWidget *widget, Timer *timer)
{
    set_mode(timer, get_break_mode(timer));
    stop(timer);
    refresh(timer->drawbox);
    PRINT_MODE(timer->mode);
}
//probably add confirmation screen
void pmdr_count_reset(GtkWidget *widget, Timer *timer)
{
    timer->pmdr_count = 0;
    refresh(timer->drawbox);
    DBG("count reset");
}

//handlers
static gboolean countdown(Timer *timer)
{
    if ((--timer->count) > 0)
    {
        //still running
        DBG("timer->count: %d", timer->count);
        refresh(timer->drawbox);
        return TRUE;
    }
    else
    {
        DBG("timer end");
        clean_id(timer);
        PRINT_MODE(timer->mode);

        forward_pmdr(timer);
        t_notify_send(timer->notify);

        PRINT_MODE(timer->mode);
        DBG("pmdr_count: %d", timer->pmdr_count);
        refresh(timer->drawbox);
        return FALSE;
    }
}

//stop timer
static void stop(Timer *timer)
{
    if (timer->id != 0)
    {
        g_source_remove(timer->id);
        clean_id(timer);
    }
    timer_set(timer);
}

void click_handler(GtkWidget *widget, GdkEventButton *event, Timer *timer)
{
    PRINT_MODE(timer->mode);
    //if middle button clicked, stop timer
    if (event->button == GDK_BUTTON_MIDDLE)
    {
        DBG("timer stop");
        stop(timer);
        refresh(timer->drawbox);
        return;
    }

    //if timer running
    if (timer->id > 0)
    {
        g_source_remove(timer->id);
        clean_id(timer);
        if (event->button == GDK_BUTTON_PRIMARY)
        {
            DBG("timer pause");
            pause_timer_color(timer);
        }
    }
    else //timer isn't running
    {
        if (timer->set == timer->count)
        {
            DBG("timer start");
            DBG("pmdr_count: %d", timer->pmdr_count);
            timer_set(timer);
            timer->id = g_timeout_add_seconds(1, G_SOURCE_FUNC(countdown), timer);
            t_notify_close(timer->notify);
        }
        else
        {
            DBG("timer resume");
            resume_timer_color(timer);
            timer->id = g_timeout_add_seconds(1, G_SOURCE_FUNC(countdown), timer);
        }
    }
    refresh(timer->drawbox);
}

//MAIN LOGIC END

void pause_timer_color(Timer *t)
{
    t->color = t->setting->pause_color;
}
void resume_timer_color(Timer *t)
{
    if (t->mode == WORK)
    {
        t->color = t->setting->pomodoro_color;
    }
    else
    {
        t->color = t->setting->break_color;
    }
}

//delete glib event source id
void clean_id(Timer *t)
{
    t->id = 0;
}

//set timer->set/count/color by timer->mode
//called when ended work or break
static void forward_pmdr(Timer *t)
{
    //when end work
    if (t->mode == WORK)
    {
        //give a tomato
        t->pmdr_count += 1;
        set_mode(t, get_break_mode(t));
    }
    else if (t->mode == USUAL_BREAK)
    { //when end break
        set_mode(t, WORK);
    }
    else
    {
        t->pmdr_count = 0;
        set_mode(t, WORK);
    }
    timer_set(t);
}

static void set_mode(Timer *t, TimerModes tm)
{
    t->mode = tm;
}

//get break mode by pmdr_count
TimerModes get_break_mode(Timer *t)
{
    if (t->pmdr_count >= LONGER_BREAK_INTERVAL)
    {
        return LONGER_BREAK;
    }
    else
    {
        return USUAL_BREAK;
    }
}

void timer_set(Timer *t)
{
    if (t->mode == WORK)
    {
        t->set = t->setting->pomodoro_time;
        t->count = t->setting->pomodoro_time;
        t->color = t->setting->pomodoro_color;
    }
    else if (t->mode == USUAL_BREAK)
    {
        t->set = t->setting->break_time;
        t->count = t->setting->break_time;
        t->color = t->setting->break_color;
    }
    else //longer break
    {
        t->set = t->setting->longer_break;
        t->count = t->setting->longer_break;
        t->color = t->setting->break_color;
    }
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, Timer *t)
{
    guint width, height;
    GdkRGBA white;
    gdouble mi = t->set;
    gdouble i = t->count;
    gdouble fi = t->set - t->count;
    gdouble circle_radius;
    white.red = 1.0;
    white.green = 1.0;
    white.blue = 1.0;
    white.alpha = 1.0;
    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);
    circle_radius = (MIN(width, height) / 2.0) * (t->setting->circle_scale / 100.0);
    gdk_cairo_set_source_rgba(cr, t->color);
    if (i < mi && i > 0)
    {
        //draw circle
        cairo_arc_negative(cr,
                           width / 2.0, height / 2.0,
                           circle_radius,
                           0 - 0.5 * G_PI, ((fi / mi) * 2 - 0.5) * G_PI);
        cairo_line_to(cr, width / 2.0, height / 2.0);
        cairo_fill(cr);
    }
    else
    {
        ///*circle
        cairo_arc(cr, width / 2.0, height / 2.0, circle_radius, 0, 2 * G_PI);
        cairo_fill(cr);

        { //draw pomodoro count number
            gchar pmdr_char[3];
            cairo_text_extents_t extents;
            double x, y;
            sprintf(pmdr_char, "%d", t->pmdr_count);
            gdk_cairo_set_source_rgba(cr, &white);
            cairo_select_font_face(cr, "Sans",
                                   CAIRO_FONT_SLANT_NORMAL,
                                   CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, height * (t->setting->text_scale / 100.0));

            cairo_text_extents(cr, pmdr_char, &extents);
            x = (height / 2.0) - (extents.width / 2 + extents.x_bearing);
            y = (height / 2.0) - (extents.height / 2 + extents.y_bearing);

            cairo_move_to(cr, x, y);
            cairo_show_text(cr, pmdr_char);
        }
    }

    return TRUE;
}