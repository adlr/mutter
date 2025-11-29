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

/* Caller takes ownership of return value */
GPtrArray *  /* of type MetaCrtcKms * */
meta_render_target_native_get_crtc_kmses (MetaRenderTarget *render_target);

/* Caller takes ownership of return value */
GPtrArray *  /* of type MetaKmsCrtc * */
meta_render_target_native_get_kms_crtcs (MetaRenderTarget *render_target);
