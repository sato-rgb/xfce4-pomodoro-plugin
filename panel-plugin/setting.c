#include <cairo.h>
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4util/libxfce4util.h>

#include "setting.h"

static void save(XfceRc *rc, Setting *s)
{
    gchar *p_rgba, *b_rgba, *pause_rgba;

    //pomodoro time
    xfce_rc_write_int_entry(rc, "pomodoro_time", s->pomodoro_time);
    //color
    p_rgba = gdk_rgba_to_string(s->pomodoro_color);
    xfce_rc_write_entry(rc, "pomodoro_color", p_rgba);

    //break time
    xfce_rc_write_int_entry(rc, "break_time", s->break_time);
    //color
    b_rgba = gdk_rgba_to_string(s->break_color);
    xfce_rc_write_entry(rc, "break_color", b_rgba);

    //pause color
    pause_rgba = gdk_rgba_to_string(s->pause_color);
    xfce_rc_write_entry(rc, "pause_color", pause_rgba);

    //longer break time
    xfce_rc_write_int_entry(rc, "longer_break", s->longer_break);

    //scales
    xfce_rc_write_int_entry(rc, "circle_scale", s->circle_scale);
    xfce_rc_write_int_entry(rc, "text_scale", s->text_scale);
}
//you can call it even rc == NULL
static void read_with_fallback(XfceRc *rc, Setting *s)
{
    if (G_LIKELY(rc != NULL))
    {
        gchar
            *p_rgba_spec,
            *b_rgba_spec,
            *pause_rgba_spec;

        //pomodoro time
        s->pomodoro_time = xfce_rc_read_int_entry(rc, "pomodoro_time", DEFAULT_POMODORO_TIME);

        //color
        p_rgba_spec = g_strdup(xfce_rc_read_entry(rc, "pomodoro_color", DEFAULT_POMODORO_COLOR));
        gdk_rgba_parse(s->pomodoro_color, p_rgba_spec);

        //break time
        s->break_time = xfce_rc_read_int_entry(rc, "break_time", DEFAULT_BREAK_TIME);

        //color
        b_rgba_spec = g_strdup(xfce_rc_read_entry(rc, "break_color", DEFAULT_BREAK_COLOR));
        gdk_rgba_parse(s->break_color, b_rgba_spec);

        //pause color
        pause_rgba_spec = g_strdup(xfce_rc_read_entry(rc, "pause_color", DEFAULT_PAUSE_COLOR));
        gdk_rgba_parse(s->pause_color, pause_rgba_spec);

        //longer break time
        s->longer_break = xfce_rc_read_int_entry(rc, "longer_break", DEFAULT_LONGER_BREAK);

        //scales
        s->circle_scale = xfce_rc_read_int_entry(rc, "circle_scale", DEFAULT_CIRCLE_SCALE);
        s->text_scale = xfce_rc_read_int_entry(rc, "text_scale", DEFAULT_TEXT_SCALE);

        //free
        g_free(p_rgba_spec);
        g_free(b_rgba_spec);
        g_free(pause_rgba_spec);
    }
    else
    {
        //fallback
        DBG("Fallback!");
        //pomodoro time
        s->pomodoro_time = DEFAULT_POMODORO_TIME;
        //color
        gdk_rgba_parse(s->pomodoro_color, DEFAULT_POMODORO_COLOR);

        //break time
        s->break_time = DEFAULT_BREAK_TIME;
        //color
        gdk_rgba_parse(s->break_color, DEFAULT_BREAK_COLOR);

        //pause color
        gdk_rgba_parse(s->pause_color, DEFAULT_PAUSE_COLOR);

        //longer break time
        s->longer_break = DEFAULT_LONGER_BREAK;
        //scales
        s->circle_scale = DEFAULT_CIRCLE_SCALE;
        s->text_scale = DEFAULT_TEXT_SCALE;
    }
}

void config_save(XfcePanelPlugin *plugin,
                 Setting *s)
{
    //return;
    XfceRc *rc;
    gchar *file;

    /* get the config file location */
    file = xfce_panel_plugin_save_location(plugin, TRUE);

    if (G_UNLIKELY(file == NULL))
    {
        DBG("Failed to open config file");
        return;
    }

    /* open the config file, read/write */
    rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);

    if (G_LIKELY(rc != NULL))
    {
        /* save the settings */
        DBG(".");
        save(rc, s);
        xfce_rc_flush(rc);
        /* close the rc file */
        xfce_rc_close(rc);
    }
}
void config_read(XfcePanelPlugin *plugin, Setting *s)
{
    XfceRc *rc;
    gchar *file;
    /* get the plugin config file location */
    file = xfce_panel_plugin_save_location(plugin, TRUE);

    if (G_LIKELY(file != NULL))
    {
        /* open the config file, readonly */
        rc = xfce_rc_simple_open(file, TRUE);

        /* cleanup */
        g_free(file);

        if (G_LIKELY(rc != NULL))
        {
            /* read the settings */
            read_with_fallback(rc, s);
            /* cleanup */
            xfce_rc_close(rc);
            /* leave the function, everything went well */
            DBG("read config successfully");
            return;
        }
    }

    /* something went wrong, apply default values */
    read_with_fallback(NULL, s);
}
