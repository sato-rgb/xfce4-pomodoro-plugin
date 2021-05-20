// I put the POMODORO because SETTING_H seems common
#ifndef __POMODORO_SETTING_H__
#define __POMODORO_SETTING_H__
G_BEGIN_DECLS

#include <glib.h>

#define DEFAULT_POMODORO_COLOR "rgb(237,93,64)"
#define DEFAULT_POMODORO_TIME (25 * 60)
#define DEFAULT_BREAK_COLOR "rgb(0,127,255)"
#define DEFAULT_BREAK_TIME (5 * 60)
#define DEFAULT_PAUSE_COLOR "rgb(127,127,127)"
#define DEFAULT_LONGER_BREAK (15 * 60)
//(%)
#define DEFAULT_CIRCLE_SCALE (100)
#define DEFAULT_TEXT_SCALE (66)

#define ONE_MINUTE (60)
#define LONGER_BREAK_INTERVAL (4)
typedef struct
{
    guint pomodoro_time;
    GdkRGBA *pomodoro_color;

    guint break_time;
    GdkRGBA *break_color;

    guint longer_break;

    GdkRGBA *pause_color;
    //between 0 to 1
    guint circle_scale;
    guint text_scale;
} Setting;

//prototypes

void config_save(XfcePanelPlugin *plugin, Setting *s);
void config_read(XfcePanelPlugin *plugin, Setting *s);

G_END_DECLS
#endif /* !__POMODORO_SETTING_H__ */