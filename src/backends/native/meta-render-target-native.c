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

#include "config.h"

#include "backends/native/meta-render-target-native.h"
#include "backends/native/meta-crtc-kms.h"
#include "backends/native/meta-kms-crtc.h"

MetaCrtcKms *
meta_render_target_native_get_primary_crtc_kms (MetaRenderTarget *render_target)
{
  return META_CRTC_KMS (meta_render_target_get_primary_crtc (render_target));
}

MetaKmsCrtc *
meta_render_target_native_get_primary_kms_crtc (MetaRenderTarget *render_target)
{
  return meta_crtc_kms_get_kms_crtc (meta_render_target_native_get_primary_crtc_kms (render_target));
}

MetaKmsDevice *
meta_render_target_native_get_kms_device (MetaRenderTarget *render_target)
{
  return meta_kms_crtc_get_device (meta_render_target_native_get_primary_kms_crtc (render_target));
}

MetaOutputKms *
meta_render_target_native_get_primary_output_kms (MetaRenderTarget *render_target)
{
  return META_OUTPUT_KMS (meta_render_target_get_primary_output (render_target));
}

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

int64_t
meta_render_target_native_get_deadline_evasion (MetaRenderTarget *render_target)
{
  int64_t deadline_evasion_us = INT64_MIN;
  GPtrArray *crtcs = meta_render_target_get_crtcs (render_target);
  for (guint i = 0; i < crtcs->len; i++)
    {
      MetaCrtc *crtc = g_ptr_array_index (crtcs, i);
      MetaCrtcNative *crtc_native = META_CRTC_NATIVE (crtc);
      deadline_evasion_us = MAX (deadline_evasion_us,
                                 meta_crtc_native_get_deadline_evasion (crtc_native));
    }
  return deadline_evasion_us;
}