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

MetaKmsDevice *
meta_render_target_native_get_kms_device (MetaRenderTarget *render_target)
{
  MetaCrtc *primary_crtc = meta_render_target_get_primary_crtc (render_target);
  MetaKmsCrtc *kms_crtc =
    meta_crtc_kms_get_kms_crtc (META_CRTC_KMS (primary_crtc));

  return meta_kms_crtc_get_device (kms_crtc);
}

static MetaKmsCrtc *
dup_kms_crtc_from_crtc (MetaCrtc *crtc)
{
  return g_object_ref (meta_crtc_kms_get_kms_crtc (META_CRTC_KMS (crtc)));
}

GPtrArray *
meta_render_target_native_get_kms_crtc_array (MetaRenderTarget *render_target)
{
  GPtrArray *crtcs = meta_render_target_get_crtcs (render_target);

  return g_ptr_array_copy (crtcs, (GCopyFunc) dup_kms_crtc_from_crtc, NULL);
}

int64_t
meta_render_target_native_get_deadline_evasion (MetaRenderTarget *render_target)
{
  MetaCrtc *primary_crtc = meta_render_target_get_primary_crtc (render_target);

  return meta_crtc_native_get_deadline_evasion (META_CRTC_NATIVE (primary_crtc));
}
