/*
 * Sweep, a sound wave editor.
 *
 * Copyright (C) 2000 Conrad Parker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

#include "sweep.h"
#include "edit.h"
#include "format.h"
#include "filter_ops.h"
#include "sample.h"
#include "undo.h"

#define NR_PARAMS 5

#ifndef __GNUC__
#error GCCisms used here. Please report this error to \
sweep-devel@lists.sourceforge.net stating your versions of sweep \
and your operating system and compiler.
#endif


#if 0

/* OTHER WAY OF DOING IT:
 *
 * specify ranges as a list of values, where each is a parameter
 * tagged by what type of constraint it is.
 *
 * This couples the idea of "which constraints are valid" with their
 * actual values, as opposed to the current method which is
 * decoupled (flags + values elsewhere);
 *
 * This introduces possible inconsistencies (eg. multiple lower bounds)
 * which can then be checked for consistency at plugin_init time, or
 * given an interpretation (eg. MIN/MAX values for bounds and steps)
 *
 */

static sw_param_range delay_range = {
  {i: 1},
  { SW_CONSTRAINT_BELOW, {i: 0} }
};

/* OR perhaps, to make it a consistent array */

static sw_param_range delay_range = {
  { SW_CONSTRAINT_NR, {i: 1} },  /* <-- This can be nicely macroed */
  { SW_CONSTRAINT_BELOW, {i: 0} }
};

/* Deal with multiple ranges? Uggh, no! */

#endif

static sw_param_range delay_range = {
  {i: 0},
  {i: 0},
  {i: 0}
};

static sw_param_range gain_range = {
  {f: 0.0},
  {f: 1.0},
  {f: 0}
};

static sw_param stix_list[] = {
  {i: 4},
  {s: "With a fork"},
  {s: "With a spoon"},
  {s: "With false teeth"},
  {s: "With Nigel's bum"}
};

static sw_param pants_list[] = {
  {i: 7},
  {i: 0},
  {i: 1},
  {i: 2},
  {i: 7},
  {i: 42},
  {i: 44100},
  {i: 1000000}
};

static sw_param_spec param_specs[] = {
  {
    "Delay",
    "Number of frames to delay by",
    SWEEP_TYPE_INT,
    SW_PARAM_CONSTRAINED_BELOW,
    {range: &delay_range}
  },
  {
    "Gain",
    "Gain with which to mix in delayed signal",
    SWEEP_TYPE_FLOAT,
    SW_PARAM_CONSTRAINED_BELOW | SW_PARAM_CONSTRAINED_ABOVE,
    {range: &gain_range}
  },
  {
    "Flim",
    "Should you manage your flim?",
    SWEEP_TYPE_BOOL,
    0,
    {NULL}
  },
  {
    "Stix",
    "Method of eating beans",
    SWEEP_TYPE_STRING,
    SW_PARAM_CONSTRAINED_LIST,
    {list: &stix_list}
  },
  {
    "Pants methodology",
    "How many pants should you wear per day?",
    SWEEP_TYPE_INT,
    SW_PARAM_CONSTRAINED_LIST,
    {list: &pants_list}
  }
};

static void
echo_suggest (sw_sample * sample, sw_param_set pset)
{
  pset[0].i = 2000;
  pset[1].f = 0.4;
  pset[2].b = TRUE;
  pset[3].s = "With a fork";
}

static void
region_echo (gpointer data, sw_format * format, gint nr_frames,
	     sw_param_set pset)
{
  glong i, sw;
  sw_audio_t * d, * e;
  gpointer ep;
  gint dlen;
  gint delay = pset[0].i;
  gfloat gain = pset[1].f;

  sw = frames_to_bytes (format, 1);

  d = (sw_audio_t *)data;
  ep = data + frames_to_bytes (format, delay);
  e = (sw_audio_t *)ep;

  if (delay > nr_frames) return;

  dlen = frames_to_samples (format, nr_frames - delay);

  for (i = 0; i < dlen; i++) {
    e[i] += (sw_audio_t)((gfloat)(d[i]) * gain);
  }
}

static sw_op_instance *
echo_apply (sw_sample * sample, sw_param_set pset)
{
  return
    register_filter_region_op (sample, _("Echo"),
			       (SweepFilterRegion)region_echo, pset);
}


static sw_proc proc_echo = {
  _("Echo"),
  _("Apply an echo to selected regions of a sample"),
  "Conrad Parker",
  "Copyright (C) 2000",
  "http://sweep.sourceforge.net/plugins/echo",
  "Filters/Echo", /* category */
  GDK_e, /* accel_key */
  GDK_SHIFT_MASK, /* accel_mods */
  NR_PARAMS, /* nr_params */
  param_specs, /* param_specs */
  echo_suggest, /* suggests() */
  echo_apply,
};

static void
echo_init (gint * nr_procs, sw_proc **procs)
{
  *nr_procs = 1;
  *procs = &proc_echo;
}


sw_plugin plugin = {
  echo_init, /* plugin_init */
  NULL, /* plugin_cleanup */
};
