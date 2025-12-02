/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2026 the Mutter authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <glib-object.h>

#include "backends/meta-crtc.h"
#include "backends/meta-output.h"

#define META_TYPE_RENDER_TARGET (meta_render_target_get_type ())
G_DECLARE_FINAL_TYPE (MetaRenderTarget, meta_render_target, META, RENDER_TARGET, GObject)

MetaRenderTarget *meta_render_target_new (void);
void meta_render_target_set_transform (MetaRenderTarget *render_target,
                                       MtkMonitorTransform transform);
void meta_render_target_add (MetaRenderTarget *render_target,
                             MetaCrtc         *crtc,
                             MetaOutput       *output);
META_EXPORT_TEST
MetaCrtc * meta_render_target_get_primary_crtc (MetaRenderTarget *render_target);
MetaOutput * meta_render_target_get_primary_output (MetaRenderTarget *render_target);
const char * meta_render_target_get_name (MetaRenderTarget *render_target);
gboolean meta_render_target_has_crtc (MetaRenderTarget *render_target,
                                      MetaCrtc         *crtc);
GPtrArray * meta_render_target_get_crtcs (MetaRenderTarget *render_target);
GPtrArray * meta_render_target_get_outputs (MetaRenderTarget *render_target);
MetaGpu * meta_render_target_get_gpu (MetaRenderTarget *render_target);

/* Returns the output tile frame. That is, the rectangle in physical
 * pixels for this particular `output` in the `render_target`. This
 * function does not look at the bigger picture of how this
 * `render_target` may fit into a multi-display layout: the upper left
 * `output` will have `x`, `y` as 0.
 *
 * In order to handle non-tiled displays in the same code paths, in
 * the case of just one `output` in the `render_target`, it will
 * ignore any tile info and look at the current crtc mode to get the
 * size in physical pixels.
 */
MtkRectangle meta_render_target_get_output_frame (MetaRenderTarget *render_target);
MtkRectangle meta_render_target_get_view_layout (MetaRenderTarget *render_target);
MtkRectangle meta_render_target_get_output_tile_frame (MetaRenderTarget *render_target,
                                                       MetaOutput       *output,
                                                       gboolean          transform);

META_EXPORT_TEST
MetaBackend * meta_render_target_get_backend (MetaRenderTarget *render_target);

#define meta_render_target_foreach_crtc_output(crtc_decl, output_decl, render_target) \
  for (guint toggle__ = 1, \
       count__ = 0, \
       size__ = meta_render_target_get_crtcs (render_target)->len; \
       count__ < size__; \
       toggle__ = 1, count__++) \
  for (crtc_decl = g_ptr_array_index (meta_render_target_get_crtcs (render_target), count__); toggle__; ) \
  for (output_decl = g_ptr_array_index (meta_render_target_get_outputs (render_target), count__); toggle__; toggle__ = 0)
