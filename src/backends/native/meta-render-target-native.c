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

#include "config.h"

#include "backends/native/meta-render-target-native.h"
#include "backends/native/meta-crtc-kms.h"
#include "backends/native/meta-kms-crtc.h"

GPtrArray *
meta_render_target_native_get_crtc_kmses (MetaRenderTarget *render_target)
{
  GPtrArray *crtcs = meta_render_target_get_crtcs (render_target);
  GPtrArray *ret = g_ptr_array_new_full (crtcs->len, g_object_unref);

  for (guint i = 0; i < crtcs->len; i++)
    {
      g_ptr_array_add (ret, g_object_ref (META_CRTC_KMS (g_ptr_array_index (crtcs, i))));
    }
  return ret;
}

GPtrArray *
meta_render_target_native_get_kms_crtcs (MetaRenderTarget *render_target)
{
  GPtrArray *crtcs = meta_render_target_get_crtcs (render_target);
  GPtrArray *ret = g_ptr_array_new_full (crtcs->len, g_object_unref);

  for (guint i = 0; i < crtcs->len; i++)
    {
      g_ptr_array_add (ret, g_object_ref (meta_crtc_kms_get_kms_crtc (META_CRTC_KMS (g_ptr_array_index (crtcs, i)))));
    }
  return ret;
}
