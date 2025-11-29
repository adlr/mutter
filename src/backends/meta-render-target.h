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

void meta_render_target_set_transform (MetaRenderTarget   *render_target,
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

MtkRectangle meta_render_target_get_view_layout (MetaRenderTarget *render_target);

MetaGpu * meta_render_target_get_gpu (MetaRenderTarget *render_target);

META_EXPORT_TEST
MetaBackend * meta_render_target_get_backend (MetaRenderTarget *render_target);

/* Clobbers `crtc`, which must be of type `MetaCrtc *`. */
#define META_RENDER_TARGET_FOREACH_CRTC(render_target, crtc)    \
  for (unsigned int G_PASTE (__i, __LINE__) = 0; \
       G_PASTE (__i, __LINE__) < \
         meta_render_target_get_crtcs (render_target)->len && \
       ((crtc) = g_ptr_array_index (meta_render_target_get_crtcs (render_target), \
                                    G_PASTE (__i, __LINE__)), TRUE); \
       G_PASTE (__i, __LINE__)++)

/* Clobbers `output`, which must be of type `MetaOutput *`. */
#define META_RENDER_TARGET_FOREACH_OUTPUT(render_target, output) \
  for (unsigned int G_PASTE (__i, __LINE__) = 0; \
       G_PASTE (__i, __LINE__) < \
         meta_render_target_get_outputs (render_target)->len && \
       ((output) = g_ptr_array_index (meta_render_target_get_outputs (render_target), \
                                      G_PASTE (__i, __LINE__)), TRUE); \
       G_PASTE (__i, __LINE__)++)

/* Clobbers `output`, which must be of type `MetaOutput *`, and
 * `crtc`, which must be of type `MetaCrtc *`.
 */
#define META_RENDER_TARGET_FOREACH_FULL(render_target, output, crtc) \
  for (unsigned int G_PASTE (__i, __LINE__) = 0; \
       G_PASTE (__i, __LINE__) < \
         meta_render_target_get_crtcs (render_target)->len && \
       ((output) = g_ptr_array_index (meta_render_target_get_outputs (render_target), \
                                      G_PASTE (__i, __LINE__)), \
        (crtc) = g_ptr_array_index (meta_render_target_get_crtcs (render_target), \
                                    G_PASTE (__i, __LINE__)), TRUE); \
       G_PASTE (__i, __LINE__)++)
