///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/**
 * \file dybkgnd.c
 * \author Xueshuming <xueshuming@minigui.org>
 * \date 2020/09/03
 *
 * \brief This file implements dynamic background.
 *
 \verbatim

    This file is part of hishell, a developing operating system based on
    MiniGUI. HybridOs will support embedded systems and smart IoT devices.

    Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>

#include <glib.h>

#include "librsvg/rsvg.h"

#include "../include/sysconfig.h"

#define WALLPAPER_FILE_TOP          "wallpaper-top.jpg"
#define WALLPAPER_FILE_BOTTOM       "wallpaper-bottom.jpg"

BITMAP bmpTop;
BITMAP bmpBottom;

int start = 100;
int end = 0;
int duration = 600;
enum EffMotionType motionType = OutQuart;
//enum EffMotionType motionType = OutQuint;

void loadBitmap(void)
{
    char topRes [MAX_PATH + 1];
    char bottomRes [MAX_PATH + 1];
    char* etc_value = NULL;

    bmpTop.bmWidth = 0;
    bmpBottom.bmWidth = 0;

    if ((etc_value = getenv ("HISHELL_RES_PATH")))
    {
        int len = strlen(etc_value);
        if (etc_value[len-1] == '/')
        {
            sprintf(topRes, "%s%s", etc_value, WALLPAPER_FILE_TOP);
            sprintf(bottomRes, "%s%s", etc_value, WALLPAPER_FILE_BOTTOM);
        }
        else
        {
            sprintf(topRes, "%s/%s", etc_value, WALLPAPER_FILE_TOP);
            sprintf(bottomRes, "%s/%s", etc_value, WALLPAPER_FILE_BOTTOM);
        }
    }
    else
    {
        sprintf(topRes, "res/%s", WALLPAPER_FILE_TOP);
        sprintf(bottomRes, "res/%s", WALLPAPER_FILE_BOTTOM);
    }

    LoadBitmapFromFile(HDC_SCREEN, &bmpTop, topRes);
    LoadBitmapFromFile(HDC_SCREEN, &bmpBottom, bottomRes);
}

void clearBitmap(void)
{
    if (bmpTop.bmWidth > 0)
    {
        UnloadBitmap(&bmpTop);
    }

    if (bmpBottom.bmWidth > 0)
    {
        UnloadBitmap(&bmpBottom);
    }
}

int paintWallpaper (HDC hdc, int space)
{
    static int lastSpace = -1;
    int ret = 0;

    if (lastSpace == space)
    {
        return ret;
    }
    lastSpace = space;

    if (bmpTop.bmWidth > 0 && bmpTop.bmHeight > 0 && bmpBottom.bmWidth > 0 && bmpBottom.bmHeight > 0) {
        int x, y, w, h, y2;
        static int wp_w = 0, wp_h = 0, wp_half_h = 0;

        if (wp_w == 0) {
            wp_w = (int)GetGDCapability (hdc, GDCAP_HPIXEL);
            wp_h = (int)GetGDCapability (hdc, GDCAP_VPIXEL);
            wp_half_h = wp_h >> 1;
            start = wp_half_h - bmpTop.bmHeight;;
        }

        SetBrushColor(hdc, RGBA2Pixel(hdc, 0x00, 0x00, 0x00, 0xFF));
        FillBox(hdc, 0, 0, wp_w, wp_h);

        w = bmpTop.bmWidth;
        h = bmpTop.bmHeight;

        x = (wp_w - w) >> 1;
        y = wp_half_h - h;
        y2 = wp_half_h;

        FillBoxWithBitmap(hdc, x, y - space, bmpTop.bmWidth, bmpTop.bmHeight, &bmpTop);
        FillBoxWithBitmap(hdc, x, y2 + space, bmpBottom.bmWidth, bmpBottom.bmHeight, &bmpBottom);

        SyncUpdateDC (hdc);
    }
    else {
        _ERR_PRINTF("Failed to get the size of wallpaper bitmap\n");
        ret = -2;
        goto ret;
    }

ret:
    return ret;
}

static void animated_cb(MGEFF_ANIMATION handle, HWND hwnd, int id, int *value)
{
    paintWallpaper(HDC_SCREEN, *value);
}

static void animated_end(MGEFF_ANIMATION handle)
{
}

void startAnimation (HWND hwnd)
{
    MGEFF_ANIMATION animation;
    animation = mGEffAnimationCreate((void *)hwnd, (void *)animated_cb, 1, MGEFF_INT);
    if (animation) {
        mGEffAnimationSetStartValue(animation, &start);
        mGEffAnimationSetEndValue(animation, &end);
        mGEffAnimationSetDuration(animation, duration);
        mGEffAnimationSetCurve(animation, motionType);
        mGEffAnimationSetFinishedCb(animation, animated_end);
        mGEffAnimationSyncRun(animation);
        mGEffAnimationDelete(animation);
    }
}

void doAnimationBack(HWND hwnd)
{
    MGEFF_ANIMATION animation;
    animation = mGEffAnimationCreate((void *)hwnd, (void *)animated_cb, 1, MGEFF_INT);
    if (animation) {
        mGEffAnimationSetStartValue(animation, &end);
        mGEffAnimationSetEndValue(animation, &start);
        mGEffAnimationSetDuration(animation, motionType);
        mGEffAnimationSetCurve(animation, motionType);
        mGEffAnimationSyncRun(animation);
        mGEffAnimationDelete(animation);
    }
}


static char *
get_output_file (const char *test_file,
                 const char *extension)
{
  const char *output_dir = g_get_tmp_dir ();
  char *result, *base;

  base = g_path_get_basename (test_file);

  if (g_str_has_suffix (base, ".svg"))
    base[strlen (base) - strlen (".svg")] = '\0';

  result = g_strconcat (output_dir, G_DIR_SEPARATOR_S, base, extension, NULL);
  g_free (base);

  return result;
}

static void
save_image (cairo_surface_t *surface,
            const char      *test_name,
            const char      *extension)
{
  char *filename = get_output_file (test_name, extension);

  fprintf(stderr, "=======================output filename=%s\n", filename);
  g_test_message ("Storing test result image at %s", filename);
  g_assert (cairo_surface_write_to_png (surface, filename) == CAIRO_STATUS_SUCCESS);

  g_free (filename);
}

static HDC create_memdc_from_image_surface (cairo_surface_t* image_surface)
{
    MYBITMAP my_bmp = {
        flags: MYBMP_TYPE_RGB | MYBMP_FLOW_DOWN,
        frames: 1,
        depth: 32,
    };

    my_bmp.w = cairo_image_surface_get_width (image_surface);
    my_bmp.h = cairo_image_surface_get_height (image_surface);
    my_bmp.pitch = cairo_image_surface_get_stride (image_surface);
    my_bmp.bits = cairo_image_surface_get_data (image_surface);
    my_bmp.size = my_bmp.pitch * my_bmp.h;

    return CreateMemDCFromMyBitmap(&my_bmp, NULL);
}

void loadSVG(const char* file)
{
    RsvgHandle *handle;
    GError *error = NULL;

    cairo_t *cr;
    cairo_surface_t *surface_a;
    RsvgDimensionData dimensions;
    
    handle = rsvg_handle_new_from_file (file, &error);
    rsvg_handle_get_dimensions (handle, &dimensions);

    int width = 100;// dimensions.width;
    int height = 100; //dimensions.height;

    surface_a = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create (surface_a);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

#if 1
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_paint (cr);
#endif

    cairo_save(cr);
    cairo_scale(cr, 5.0, 5.0);

    cairo_push_group (cr);
    rsvg_handle_render_cairo (handle, cr);
    cairo_pattern_t *p = cairo_pop_group (cr);

    cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
    cairo_mask(cr, p);

    cairo_restore (cr);

    cairo_surface_type_t cst = cairo_surface_get_type (surface_a);

    HDC csdc = create_memdc_from_image_surface (surface_a);
    if (csdc != HDC_SCREEN && csdc != HDC_INVALID) {
        BitBlt(csdc, 0, 0, width, height,  HDC_SCREEN, 100, 100, 0);
    }
    DeleteMemDC (csdc);
    SyncUpdateDC (HDC_SCREEN);

    cairo_surface_destroy (surface_a);
    cairo_pattern_destroy (p);
    cairo_destroy (cr);
    g_object_unref (handle);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG msg;
    JoinLayer(NAME_DEF_LAYER , "wallpaper" , 0 , 0);

    loadBitmap();
    if (paintWallpaper(HDC_SCREEN, 0) < 0)
    {
        clearBitmap();
        exit (1);
    }

    mGEffInit();
    startAnimation(NULL);

    while (GetMessage (&msg, HWND_DESKTOP)) {
        if (msg.message == MSG_DYBKGND_DO_ANIMATION)
        {
            startAnimation(NULL);
        }
        DispatchMessage (&msg);
    }

    clearBitmap();
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

