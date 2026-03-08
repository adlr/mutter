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

/**
 * MetaRendererTarget:
 *
 * Holds a collection of MetaCrtc and MetaOutput pointers that represent
 * a single output. The common case is 1 of each. In the case of a tiled
 * display, objects of this class will hold 1 crtc and output per tile.
 * The first element of each member array is the "primary" one.
 * There must be at least one crtc and output, and the number of
 * crtcs and outputs must be identical.
 */

#include "config.h"

#include "backends/meta-render-target.h"

#include <glib-object.h>

struct _MetaRenderTarget
{
  GObject parent_instance;

  GPtrArray *crtcs;  /* of type MetaCrtc * */
  GPtrArray *outputs;  /* of type MetaOutput * */
  MtkMonitorTransform transform;
};

G_DEFINE_TYPE (MetaRenderTarget, meta_render_target, G_TYPE_OBJECT)

MetaRenderTarget *
meta_render_target_new (void)
{
  return g_object_new (META_TYPE_RENDER_TARGET, NULL);
}

void
meta_render_target_set_transform (MetaRenderTarget *render_target,
                                  MtkMonitorTransform transform)
{
  render_target->transform = transform;
}

void
meta_render_target_add (MetaRenderTarget *render_target,
                        MetaCrtc         *crtc,
                        MetaOutput       *output)
{
  g_ptr_array_add (render_target->crtcs, g_object_ref (crtc));
  g_ptr_array_add (render_target->outputs, g_object_ref (output));
}

MetaCrtc *
meta_render_target_get_primary_crtc (MetaRenderTarget *render_target)
{
  g_return_val_if_fail (render_target->crtcs->len > 0, NULL);
  return g_ptr_array_index (render_target->crtcs, 0);
}

MetaOutput *
meta_render_target_get_primary_output (MetaRenderTarget *render_target)
{
  g_return_val_if_fail (render_target->outputs->len > 0, NULL);
  return g_ptr_array_index (render_target->outputs, 0);
}

const char *
meta_render_target_get_name (MetaRenderTarget *render_target)
{
  MetaOutput *primary_output = meta_render_target_get_primary_output (render_target);

  return meta_output_get_name (primary_output);
}

gboolean
meta_render_target_has_crtc (MetaRenderTarget *render_target,
                             MetaCrtc         *crtc)
{
  return g_ptr_array_find (render_target->crtcs, crtc, NULL);
}

GPtrArray *
meta_render_target_get_crtcs (MetaRenderTarget *render_target)
{
  return render_target->crtcs;
}

GPtrArray *
meta_render_target_get_outputs (MetaRenderTarget *render_target)
{
  return render_target->outputs;
}

MtkRectangle
meta_render_target_get_output_frame (MetaRenderTarget *render_target)
{
  MtkRectangle output_frame;
  const MetaCrtcConfig *crtc_config = meta_crtc_get_config (g_ptr_array_index (render_target->crtcs, 0));
  const MetaCrtcModeInfo *crtc_mode_info = meta_crtc_mode_get_info (crtc_config->mode);
  /* If only one, just use that size */
  if (render_target->crtcs->len == 1)
    {
      output_frame = MTK_RECTANGLE_INIT (0, 0, crtc_mode_info->width, crtc_mode_info->height);
      return output_frame;
    }
  /* Else get full size from tile info */
  int width = 0;
  int height = 0;
  for (guint i = 0; i < render_target->outputs->len; i++)
    {
      MetaOutput *output = g_ptr_array_index (render_target->outputs, i);
      const MetaOutputInfo *output_info = meta_output_get_info (output);
      if (output_info->tile_info.loc_h_tile == 0)
        height += output_info->tile_info.tile_h;
      if (output_info->tile_info.loc_v_tile == 0)
        width += output_info->tile_info.tile_w;
    }
  output_frame = MTK_RECTANGLE_INIT (0, 0, width, height);
  return output_frame;
}

MtkRectangle
meta_render_target_get_view_layout (MetaRenderTarget *render_target)
{
  MtkRectangle view_layout;
  const MetaCrtcConfig *crtc_config = meta_crtc_get_config (g_ptr_array_index (render_target->crtcs, 0));

  mtk_rectangle_from_graphene_rect (&crtc_config->layout,
                                    MTK_ROUNDING_STRATEGY_ROUND,
                                    &view_layout);
  /* Handle all crtcs after the first by unioning them together */
  for (guint i = 1; i < render_target->crtcs->len; i++)
    {
      MetaCrtc *crtc = g_ptr_array_index (render_target->crtcs, i);
      MtkRectangle other_view_layout;

      crtc_config = meta_crtc_get_config (crtc);
      mtk_rectangle_from_graphene_rect (&crtc_config->layout,
                                        MTK_ROUNDING_STRATEGY_ROUND,
                                        &other_view_layout);
      mtk_rectangle_union (&view_layout, &other_view_layout, &view_layout);
    }
  return view_layout;
}

MetaGpu *
meta_render_target_get_gpu (MetaRenderTarget *render_target)
{
  return meta_crtc_get_gpu (meta_render_target_get_primary_crtc (render_target));
}

MtkRectangle
meta_render_target_get_output_tile_frame (MetaRenderTarget *render_target,
                                          MetaOutput       *output,
                                          gboolean          transform)
{
  const MetaOutputInfo *output_info = meta_output_get_info (output);
  MtkRectangle tile_frame = MTK_RECTANGLE_INIT (0, 0, output_info->tile_info.tile_w, output_info->tile_info.tile_h);
  MtkMonitorTransform use_transform =
    transform ? render_target->transform : MTK_MONITOR_TRANSFORM_NORMAL;

  /* In order to share the tiled and non-tiled display cases, we
   * fallback to the current CRTC mode size when there's only one
   * output. We can't use the tile_info in this case because it's
   * generally all 0 for a non-tiled display. */
  if (render_target->outputs->len == 1)
    {
      const MetaCrtcConfig *crtc_config;
      MetaCrtcMode *crtc_mode;
      const MetaCrtcModeInfo *mode_info;

      tile_frame = MTK_RECTANGLE_INIT (0, 0, 0, 0);
      g_return_val_if_fail (render_target->crtcs->len == 1, tile_frame);
      crtc_config = meta_crtc_get_config (g_ptr_array_index (render_target->crtcs, 0));
      g_return_val_if_fail (crtc_config, tile_frame);
      crtc_mode = crtc_config->mode;
      g_return_val_if_fail (crtc_mode, tile_frame);
      mode_info = meta_crtc_mode_get_info (crtc_mode);
      g_return_val_if_fail (mode_info, tile_frame);
      tile_frame.width = mode_info->width;
      tile_frame.height = mode_info->height;
    }
  else
    {
      meta_output_info_calculate_tile_coordinate (output_info, render_target->outputs,
                                                  use_transform,
                                                  &tile_frame.x,
                                                  &tile_frame.y);
    }
  switch (use_transform)
    {
    case MTK_MONITOR_TRANSFORM_270:
    case MTK_MONITOR_TRANSFORM_FLIPPED_270:
    case MTK_MONITOR_TRANSFORM_90:
    case MTK_MONITOR_TRANSFORM_FLIPPED_90:
      /* swap width and height for these */
      int width = tile_frame.width;
      tile_frame.width = tile_frame.height;
      tile_frame.height = width;
      break;
    default:  /* appease compiler */
    }
  return tile_frame;
}

MetaBackend *
meta_render_target_get_backend (MetaRenderTarget *render_target)
{
  return meta_crtc_get_backend (meta_render_target_get_primary_crtc (render_target));
}

static void
meta_render_target_finalize (GObject *object)
{
  MetaRenderTarget *render_target = META_RENDER_TARGET (object);

  g_ptr_array_foreach (render_target->crtcs, (GFunc) g_object_unref, NULL);
  g_clear_pointer (&render_target->crtcs, g_ptr_array_unref);
  g_ptr_array_foreach (render_target->outputs, (GFunc) g_object_unref, NULL);
  g_clear_pointer (&render_target->outputs, g_ptr_array_unref);

  G_OBJECT_CLASS (meta_render_target_parent_class)->dispose (object);
}

static void
meta_render_target_init (MetaRenderTarget *render_target)
{
  render_target->crtcs = g_ptr_array_new ();
  render_target->outputs = g_ptr_array_new ();
}

static void
meta_render_target_class_init (MetaRenderTargetClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = meta_render_target_finalize;
}
