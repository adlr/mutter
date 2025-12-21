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
};

G_DEFINE_TYPE (MetaRenderTarget, meta_render_target, G_TYPE_OBJECT)

MetaRenderTarget *
meta_render_target_new (void)
{
  return g_object_new (META_TYPE_RENDER_TARGET, NULL);
}

void
meta_render_target_add (MetaRenderTarget *render_target,
                        MetaCrtc *crtc,
                        MetaOutput *output)
{
  g_ptr_array_add (render_target->crtcs, g_object_ref (crtc));
  g_ptr_array_add (render_target->outputs, g_object_ref (output));
}

MetaCrtc *
meta_render_target_get_primary_crtc (MetaRenderTarget *render_target)
{
  g_warn_if_fail (render_target->crtcs->len > 0);
  return g_ptr_array_index (render_target->crtcs, 0);
}

MetaOutput *
meta_render_target_get_primary_output (MetaRenderTarget *render_target)
{
  g_warn_if_fail (render_target->outputs->len > 0);
  return g_ptr_array_index (render_target->outputs, 0);
}

const char *
meta_render_target_get_name (MetaRenderTarget *render_target)
{
  MetaOutput *primary_output = meta_render_target_get_primary_output (render_target);
  return meta_output_get_name (primary_output);
}

gboolean
meta_render_target_has_crtc (MetaRenderTarget *render_target, MetaCrtc *crtc)
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

MetaGpu *
meta_render_target_get_gpu (MetaRenderTarget *render_target)
{
  return meta_crtc_get_gpu (meta_render_target_get_primary_crtc (render_target));
}

MtkRectangle
meta_render_target_get_output_frame (MetaRenderTarget *render_target)
{
  MtkRectangle output_frame;
  const MetaCrtcConfig *crtc_config = meta_crtc_get_config (g_ptr_array_index (render_target->crtcs, 0));
  const MetaCrtcModeInfo *crtc_mode_info = meta_crtc_mode_get_info (crtc_config->mode);
  meta_topic (META_DEBUG_KMS, "crtc config layout %f %f %f %f vs mode info %d %d",
              graphene_rect_get_x (&crtc_config->layout), graphene_rect_get_y (&crtc_config->layout),
              graphene_rect_get_width (&crtc_config->layout), graphene_rect_get_height (&crtc_config->layout),
              crtc_mode_info->width, crtc_mode_info->height);
  for (guint i = 0; i < render_target->outputs->len; i++)
    {
      MetaOutput *other_output = g_ptr_array_index (render_target->outputs, i);
      const MetaOutputInfo *other_output_info = meta_output_get_info (other_output);
      meta_topic (META_DEBUG_KMS, "Also Tile into %u/%u: size: %d %d loc: %d %d",
                  i+1, render_target->outputs->len,
                  other_output_info->tile_info.tile_w, other_output_info->tile_info.tile_h,
                  other_output_info->tile_info.loc_h_tile, other_output_info->tile_info.loc_v_tile);
    }
  // If only one, just use that size
  if (render_target->crtcs->len == 1) {
    output_frame = MTK_RECTANGLE_INIT (0, 0, crtc_mode_info->width, crtc_mode_info->height);
    return output_frame;
  }
  // Else get full size from tile info
  int max_h = 0;
  int max_v = 0;
  MetaOutput *max_output = NULL;
  for (guint i = 0; i < render_target->outputs->len; i++)
    {
      MetaOutput *output = g_ptr_array_index (render_target->outputs, i);
      const MetaOutputInfo *output_info = meta_output_get_info (output);
      if (output_info->tile_info.loc_h_tile > max_h || output_info->tile_info.loc_v_tile > max_v)
        {
          max_output = output;
          max_h = output_info->tile_info.loc_h_tile;
          max_v = output_info->tile_info.loc_v_tile;
        }
    }
  g_warn_if_fail (max_output != NULL);
  MtkRectangle tmp = meta_render_target_get_output_tile_frame (render_target, max_output);
  output_frame = MTK_RECTANGLE_INIT (0, 0, tmp.x + tmp.width, tmp.y + tmp.height);
  return output_frame;
}

MtkRectangle
meta_render_target_get_view_layout (MetaRenderTarget *render_target)
{
  MtkRectangle output_frame;
  const MetaCrtcConfig *crtc_config = meta_crtc_get_config (g_ptr_array_index (render_target->crtcs, 0));
  const MetaCrtcModeInfo *crtc_mode_info = meta_crtc_mode_get_info (crtc_config->mode);
  meta_topic (META_DEBUG_KMS, "crtc config layout %f %f %f %f vs mode info %d %d",
              graphene_rect_get_x (&crtc_config->layout), graphene_rect_get_y (&crtc_config->layout),
              graphene_rect_get_width (&crtc_config->layout), graphene_rect_get_height (&crtc_config->layout),
              crtc_mode_info->width, crtc_mode_info->height);
  for (guint i = 0; i < render_target->outputs->len; i++)
    {
      MetaOutput *other_output = g_ptr_array_index (render_target->outputs, i);
      const MetaOutputInfo *other_output_info = meta_output_get_info (other_output);
      meta_topic (META_DEBUG_KMS, "Also Tile into %u/%u: size: %d %d loc: %d %d",
                  i+1, render_target->outputs->len,
                  other_output_info->tile_info.tile_w, other_output_info->tile_info.tile_h,
                  other_output_info->tile_info.loc_h_tile, other_output_info->tile_info.loc_v_tile);
    }
  mtk_rectangle_from_graphene_rect (&crtc_config->layout,
                                    MTK_ROUNDING_STRATEGY_ROUND,
                                    &output_frame);
  // Handle all crtcs after the first by unioning them together
  for (guint i = 1; i < render_target->crtcs->len; i++)
    {
      MetaCrtc *crtc = g_ptr_array_index (render_target->crtcs, i);
      MtkRectangle other_view_layout;
      crtc_config = meta_crtc_get_config (crtc);
      mtk_rectangle_from_graphene_rect (&crtc_config->layout,
                                        MTK_ROUNDING_STRATEGY_ROUND,
                                        &other_view_layout);
      mtk_rectangle_union (&output_frame, &other_view_layout, &output_frame);
    }
  return output_frame;
}

MtkRectangle
meta_render_target_get_output_tile_frame (MetaRenderTarget *render_target, MetaOutput *output)
{
  const MetaOutputInfo *output_info = meta_output_get_info (output);
  MtkRectangle tile_frame = MTK_RECTANGLE_INIT (0, 0, output_info->tile_info.tile_w, output_info->tile_info.tile_h);
  for (guint i = 0; i < render_target->outputs->len; i++)
    {
      MetaOutput *other_output = g_ptr_array_index (render_target->outputs, i);
      const MetaOutputInfo *other_output_info = meta_output_get_info (other_output);
      meta_topic (META_DEBUG_KMS, "Tile into %u/%u: size: %d %d loc: %d %d",
                  i+1, render_target->outputs->len,
                  other_output_info->tile_info.tile_w, other_output_info->tile_info.tile_h,
                  other_output_info->tile_info.loc_h_tile, other_output_info->tile_info.loc_v_tile);
      if (output_info->tile_info.loc_v_tile == other_output_info->tile_info.loc_v_tile &&
          output_info->tile_info.loc_h_tile > other_output_info->tile_info.loc_h_tile)
        {
          tile_frame.x += other_output_info->tile_info.tile_w;
        }
      if (output_info->tile_info.loc_h_tile == other_output_info->tile_info.loc_h_tile &&
          output_info->tile_info.loc_v_tile > other_output_info->tile_info.loc_v_tile)
        {
          tile_frame.y += other_output_info->tile_info.tile_h;
        }
    }
  meta_topic (META_DEBUG_KMS, "Final frame: %d %d %d %d", tile_frame.x, tile_frame.y, tile_frame.width, tile_frame.height);
  return tile_frame;
}

MetaBackend *
meta_render_target_get_backend (MetaRenderTarget *render_target)
{
  return meta_crtc_get_backend (meta_render_target_get_primary_crtc (render_target));
}

static void
meta_render_target_dispose (GObject *object)
{
  MetaRenderTarget *render_target = META_RENDER_TARGET (object);

  g_ptr_array_foreach (render_target->crtcs, (GFunc) g_object_unref, NULL);
  g_ptr_array_unref (render_target->crtcs);
  g_ptr_array_foreach (render_target->outputs, (GFunc) g_object_unref, NULL);
  g_ptr_array_unref (render_target->outputs);

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

  object_class->dispose = meta_render_target_dispose;
}
