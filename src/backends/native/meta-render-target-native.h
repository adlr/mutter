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
/*
 * This is not a subclass of MetaRenderTarget, but rather a set of
 * functions that makes sense in the context of the native backend.
 */

#pragma once

#include <glib-object.h>

#include "backends/meta-render-target.h"
#include "backends/native/meta-crtc-kms.h"
#include "backends/native/meta-output-kms.h"

MetaKmsDevice * meta_render_target_native_get_kms_device (MetaRenderTarget *render_target);

typedef GPtrArray MetaKmsCrtcPtrArray;  /* GPtrArray of MetaKmsCrtc *, must have at least 1 element */

/* Returns a GPtrArray of MetaKmsCrtc *. Caller takes ownership of the
 * returned array, and the array has a ref on each element.
 */
GPtrArray * meta_render_target_native_get_kms_crtc_array (MetaRenderTarget *render_target);

#define meta_crtc_kms_ptr_array_get_primary(arr) \
  ((MetaKmsCrtc *) g_ptr_array_index (arr, 0))
#define meta_crtc_kms_ptr_array_get_device(arr) \
  ((MetaKmsDevice *) meta_kms_crtc_get_device (meta_crtc_kms_ptr_array_get_primary (arr)))
#define meta_crtc_kms_ptr_array_get_deadline_evasion(arr) \
  meta_kms_crtc_get_deadline_evasion (meta_crtc_kms_ptr_array_get_primary (arr))

int64_t meta_render_target_native_get_deadline_evasion (MetaRenderTarget *render_target);

#define meta_kms_crtc_ptr_array_foreach(kms_crtc_decl, arr) \
  for (guint toggle__ = 1, \
       count__ = 0, \
       size__ = arr->len; \
       count__ < size__; \
       toggle__ = 1, count__++) \
  for (kms_crtc_decl = g_ptr_array_index (arr, count__); toggle__; toggle__ = 0)
