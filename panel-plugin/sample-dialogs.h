/*  $Id$
 *
 *  Copyright (C) 2012 John Doo <john@foo.org>
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

#ifndef __SAMPLE_DIALOGS_H__
#define __SAMPLE_DIALOGS_H__

G_BEGIN_DECLS

#include "sample.h"

typedef struct
{
    SamplePlugin *sample;

    GtkWidget *spinb_pmdr_time;
    GtkAdjustment *pmdr_adj;
    GtkWidget *colpick_pmdr_color;

    GtkWidget *spinb_break_time;
    GtkAdjustment *break_adj;
    GtkWidget *colpick_break_color;

    GtkWidget *spinb_longer_break;
    GtkAdjustment *lonbre_adj;

    GtkWidget *colpick_pause_color;
    //scales
    GtkWidget *scale_circle_scale;
    GtkRange *circle_scale_range;

    GtkWidget *scale_text_scale;
    GtkRange *text_scale_range;

} SampleWithDialogs;

void sample_configure(XfcePanelPlugin *plugin,
                      SamplePlugin *sample);

void sample_about(XfcePanelPlugin *plugin);

G_END_DECLS

#endif
