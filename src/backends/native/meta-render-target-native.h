/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2025 ?
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
/*
 * This is not a subclass of MetaRenderTarget, but rather a set of
 * functions that makes sense in the context of the native backend.
 */

#pragma once

#include <glib-object.h>

#include "backends/meta-render-target.h"
#include "backends/native/meta-crtc-kms.h"
#include "backends/native/meta-output-kms.h"

MetaCrtcKms * meta_render_target_native_get_primary_crtc_kms (MetaRenderTarget *render_target);
MetaKmsCrtc * meta_render_target_native_get_primary_kms_crtc (MetaRenderTarget *render_target);
MetaOutputKms * meta_render_target_native_get_primary_output_kms (MetaRenderTarget *render_target);

MetaKmsDevice * meta_render_target_native_get_kms_device (MetaRenderTarget *render_target);

/* Caller takes ownership of return value */
GPtrArray *  /* of type MetaCrtcKms * */
meta_render_target_native_get_crtc_kmses (MetaRenderTarget *render_target);

/* Caller takes ownership of return value */
GPtrArray *  /* of type MetaKmsCrtc * */
meta_render_target_native_get_kms_crtcs (MetaRenderTarget *render_target);

int64_t meta_render_target_native_get_deadline_evasion (MetaRenderTarget *render_target);

#define meta_render_target_native_foreach_crtc_kms(crtc_kms_decl, render_target) \
  for (guint toggle__ = 1, \
       count__ = 0, \
       size__ = meta_render_target_get_crtcs (render_target)->len; \
       count__ < size__; \
       toggle__ = 1, count__++) \
    for (crtc_kms_decl = META_CRTC_KMS (g_ptr_array_index (meta_render_target_get_crtcs (render_target), count__)); toggle__; toggle__ = 0)

#define meta_render_target_native_foreach_output_kms(output_kms_decl, render_target) \
  for (guint toggle__ = 1, \
       count__ = 0, \
       size__ = meta_render_target_get_outputs (render_target)->len; \
       count__ < size__; \
       toggle__ = 1, count__++) \
    for (output_kms_decl = META_OUTPUT_KMS (g_ptr_array_index (meta_render_target_get_outputs (render_target), count__)); toggle__; toggle__ = 0)

#define meta_render_target_native_foreach_kms_crtc(kms_crtc_decl, render_target) \
  for (guint toggle__ = 1, \
       count__ = 0, \
       size__ = meta_render_target_get_crtcs (render_target)->len; \
       count__ < size__; \
       toggle__ = 1, count__++) \
    for (kms_crtc_decl = meta_crtc_kms_get_kms_crtc (META_CRTC_KMS (g_ptr_array_index (meta_render_target_get_crtcs (render_target), count__))); toggle__; toggle__ = 0)
