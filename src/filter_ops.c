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

#include "sweep.h"
#include "format.h"
#include "filter_ops.h"
#include "edit.h"
#include "sample.h"
#include "undo.h"


static sw_operation filter_op = {
  (SweepCallback)undo_by_paste_over,
  (SweepFunction)paste_over_data_destroy,
  (SweepCallback)redo_by_paste_over,
  (SweepFunction)paste_over_data_destroy
};

static sw_sdata *
do_filter_regions (sw_sdata * sdata, SweepFilterRegion func,
		   sw_param_set pset, gpointer custom_data)
{
  sw_format * f = sdata->format;
  GList * gl;
  sw_sel * sel;
  gpointer d;

  for (gl = sdata->sels; gl; gl = gl->next) {
    sel = (sw_sel *)gl->data;

    d = sdata->data + frames_to_bytes (f, sel->sel_start);
    func (d, sdata->format, sel->sel_end - sel->sel_start,
	  pset, custom_data);
  }

  return sdata;
}

sw_op_instance *
register_filter_region_op (sw_sample * sample, char * desc,
			   SweepFilterRegion func,
			   sw_param_set pset, gpointer custom_data)
{
  sw_op_instance * inst;
  edit_buffer * old_eb, * new_eb;

  inst = sw_op_instance_new (desc, &filter_op);
  old_eb = edit_buffer_from_sample (sample);

  do_filter_regions (sample->sdata, func, pset, custom_data);
  new_eb = edit_buffer_from_sample (sample);

  inst->redo_data = inst->undo_data =
    paste_over_data_new (old_eb, new_eb);

  sample_refresh_views (sample);

  register_operation (sample, inst);

  return inst;
}

sw_op_instance *
register_filter_op (sw_sample * sample, char * desc, SweepFilter func,
		    sw_param_set pset, gpointer custom_data)
{
  sw_op_instance * inst;
  edit_buffer * old_eb, * new_eb;
  sw_sdata * out;

  inst = sw_op_instance_new (desc, &filter_op);
  old_eb = edit_buffer_from_sample (sample);

  out = func (sample->sdata, pset, custom_data);

  /* XXX: Not for inplace edits!!
  sdata_destroy (sample->sdata);
  sample->sdata = out;
  */

  new_eb = edit_buffer_from_sample (sample);

  inst->redo_data = inst->undo_data =
    paste_over_data_new (old_eb, new_eb);

  sample_refresh_views (sample);

  register_operation (sample, inst);

  return inst;
}
