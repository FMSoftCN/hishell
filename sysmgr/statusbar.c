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
 * \file statusbar.c
 * \author Gengyue <gengyue@minigui.org>
 * \date 2020/09/16
 *
 * \brief This file implements status bar in system manager process.
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

/*
 * $Id: statusbar.c 13674 2020-09-16 06:45:01Z Gengyue $
 *
 *      HybridOS for Linux, VxWorks, NuCleus, OSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgeff/mgeff.h>
#include <hibus.h>
#include <librsvg/rsvg.h>

#include "../include/sysconfig.h"
#include "config.h"

extern HWND m_hStatusBar;                       // handle of status bar
extern HWND m_hDockBar;                         // handle of dock bar
static int m_StatusBar_Height = 0;              // height of status bar
static int m_StatusBar_Y = 0;                   // the Y coordinate of top left corner
static MGEFF_ANIMATION m_animation = NULL;      // handle of animation
static int m_direction = DIRECTION_SHOW;        // the direction of animation
static int m_statusbar_visible_time = 200;      // status bar visible time
static int m_statusbar_show_time = 750;         // status bar animation show time
static int m_statusbar_hide_time = 400;         // status bar animation hide time

extern void * thread_hibus(void * arg);

static cairo_t *cr[4];
static cairo_surface_t *surface[4];
static RsvgStylePair button_color_pair[4];
static int wifi_strength = 0;
static int wifi_icon_index = 0;

// get current time
static char* mk_time(char* buff)
{
    time_t t;
    struct tm * tm;

    time (&t);
    tm = localtime (&t);
    sprintf(buff, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

    return buff;
}

// This code is copied from complexscripts from mg-sample
static void draw_title(HDC hdc, RECT rect, const char * text, PLOGFONT font)
{
    TEXTRUNS*   textruns;
    LAYOUT*     layout;
    LAYOUTLINE* line = NULL;
    int header_x = 0;
    int header_y = 0;
    int left_len_text = 0;
    Uchar32* ucs = NULL;
    Uint16* bos = NULL;

    // create title environment
    left_len_text = strlen(text);
    while(left_len_text > 0) 
    {
        int consumed = 0;
        int n = 0;
        int nr_ucs = 0;

        ucs = NULL;
        consumed = GetUCharsUntilParagraphBoundary(font, text, left_len_text, WSR_NOWRAP, &ucs, &n);
        if(consumed > 0) 
        {
            if(n > 0)
            {
                int len_bos;

                nr_ucs = n;
                bos = NULL;
                len_bos = UStrGetBreaks(LANGCODE_zh, CTR_NONE, WBR_NORMAL, LBP_NORMAL, ucs, n, &bos);

                if(len_bos > 0) 
                {
                    bos = bos;

                    // layout
                    textruns = CreateTextRuns(ucs, nr_ucs, LANGCODE_zh, BIDI_PGDIR_ON, 
                                "ttf-Source-ernnns-*-18-UTF-8", MakeRGB(0, 0, 0), 0, bos + 1);
                    if(textruns) 
                    {
                        if(!InitComplexShapingEngine(textruns)) 
                        {
                            DestroyTextRuns(textruns);
                            free(bos);
                            free(ucs);
                            return;
                        }

                        layout = CreateLayout(textruns, GRF_WRITING_MODE_HORIZONTAL_TB | GRF_TEXT_ORIENTATION_AUTO 
                                    | GRF_INDENT_NONE | GRF_LINE_EXTENT_FIXED | GRF_OVERFLOW_WRAP_NORMAL 
                                    | GRF_OVERFLOW_ELLIPSIZE_END | GRF_ALIGN_LEFT | GRF_TEXT_JUSTIFY_NONE 
                                    | GRF_HANGING_PUNC_NONE | GRF_SPACES_KEEP, bos + 1, TRUE, 
                                    RECTW(rect), 0, 0, 0, 100, NULL, 0);
                        if (layout == NULL) 
                        {
                            DestroyTextRuns(textruns);
                            free(bos);
                            free(ucs);
                            return;
                        }

                        LAYOUTLINE* line = NULL;
                        while ((line = LayoutNextLine(layout, line, 0, TRUE, NULL, 0))) 
                        {
                        }
                    }
                    else 
                        return;
                }
                else 
                    return;
            }
            else 
                return;

            break;
        }
        else
            return;

        left_len_text -= consumed;
        text += consumed;
    }
    
    // draw title
    header_x = rect.left;
    header_y = rect.top;

    SetTextColorInTextRuns(textruns, 0, 4096, MakeRGB(255, 255, 255));

    // always draw only one line
    if ((line = LayoutNextLine(layout, line, 0, 0, NULL, 0))) 
    {
        int x = header_x;
        int y = header_y + 2;
        RECT rc;

        GetLayoutLineRect(line, &x, &y, 0, &rc);
        if(RectVisible(hdc, &rc))
            DrawLayoutLine(hdc, line, header_x, header_y);
    }

    // destroy
    DestroyLayout(layout);
    DestroyTextRuns(textruns);
    free(bos);
    free(ucs);
}

// callback function of animation
static void animated_cb(MGEFF_ANIMATION handle, HWND hWnd, int id, int *value)
{
    if(m_StatusBar_Y != *value)
    {
        m_StatusBar_Y = *value;
        MoveWindow(hWnd, g_rcScr.right * (1.0 - 0.618) / 2.0, m_StatusBar_Y, g_rcScr.right * 0.618, m_StatusBar_Height, TRUE);
    }
}

// the function which will be invoked at the end of animation
static void animated_end(MGEFF_ANIMATION handle)
{
    HWND hWnd = (HWND)mGEffAnimationGetTarget(handle);
    mGEffAnimationDelete(m_animation);
    m_animation = NULL;

    if((m_direction == DIRECTION_SHOW) && hWnd)
        SetTimer(hWnd, ID_SHOW_TIMER, m_statusbar_visible_time);
}

// create an animation and start, it is asynchronous
static void create_animation(HWND hWnd)
{
    if(m_animation)
    {
        mGEffAnimationDelete(m_animation);
        m_animation = NULL;
    }

    m_animation = mGEffAnimationCreate((void *)hWnd, (void *)animated_cb, 1, MGEFF_INT);
    if (m_animation) 
    {
        int start = 0;
        int end = 0;
        int duration = 0;
        enum EffMotionType motionType = InCirc;

        start = m_StatusBar_Y;
        if(m_direction == DIRECTION_HIDE)
        {
            end = -1 * m_StatusBar_Height;
            motionType = OutCirc;
            duration = m_statusbar_hide_time * (m_StatusBar_Height + m_StatusBar_Y) / m_StatusBar_Height;
        }
        else
        {
            end = 0;
            motionType = OutCirc;
            duration = -1 * m_statusbar_show_time * m_StatusBar_Y / m_StatusBar_Height;
        }

        if(duration == 0)
            duration = m_statusbar_show_time;

        mGEffAnimationSetStartValue(m_animation, &start);
        mGEffAnimationSetEndValue(m_animation, &end);
        mGEffAnimationSetDuration(m_animation, duration);
        mGEffAnimationSetCurve(m_animation, motionType);
        mGEffAnimationSetFinishedCb(m_animation, animated_end);
        mGEffAnimationAsyncRun(m_animation);
    }
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

static void paintWiFiIcon(HDC hdc, int index)
{
    float r = SysPixelColor[IDX_COLOR_darkgray].r / 255.0;
    float g = SysPixelColor[IDX_COLOR_darkgray].g / 255.0;
    float b = SysPixelColor[IDX_COLOR_darkgray].b / 255.0;
    float alpha = 0xE0 / 255.0;

    if(surface[index] && cr[index])
    {
        HDC csdc = create_memdc_from_image_surface(surface[index]);
        if (csdc != HDC_SCREEN && csdc != HDC_INVALID)
        {
            SetMemDCColorKey(csdc, MEMDC_FLAG_SRCCOLORKEY,
                    MakeRGB(SysPixelColor[IDX_COLOR_darkgray].r * alpha,
                        SysPixelColor[IDX_COLOR_darkgray].g * alpha,
                        SysPixelColor[IDX_COLOR_darkgray].b * alpha));
            BitBlt(csdc, 0, 0, HEIGHT_STATUSBAR - 4 * MARGIN_STATUS, HEIGHT_STATUSBAR - 4 * MARGIN_STATUS, hdc, g_rcScr.right * 0.618 - TIME_INFO_X - HEIGHT_STATUSBAR + 2 * MARGIN_STATUS, 2 * MARGIN_STATUS, 0);
        }
        DeleteMemDC(csdc);
    }
    SyncUpdateDC(hdc);
}

static void loadSVGFromFile(const char* file, int index)
{
    RsvgHandle *handle;
    GError *error = NULL;
    RsvgDimensionData dimensions;
    double factor_width = 0.0f;
    double factor_height = 0.0f;

    // read file from svg file
    handle = rsvg_handle_new_from_file(file, &error);
    if(error)
    {
        surface[index] = NULL;
        cr[index] = NULL;
        return;
    }
    rsvg_handle_get_dimensions(handle, &dimensions);

    // create cairo_surface_t and cairo_t for one picture
    surface[index] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, HEIGHT_STATUSBAR - 4 * MARGIN_STATUS, HEIGHT_STATUSBAR - 4 * MARGIN_STATUS);
    cr[index] = cairo_create(surface[index]);

    cairo_save(cr[index]);

    factor_width = (double)(HEIGHT_STATUSBAR - 4 * MARGIN_STATUS) / (double)dimensions.width;
    factor_height = (double)(HEIGHT_STATUSBAR - 4 * MARGIN_STATUS) / (double)dimensions.height;
    factor_width = (factor_width > factor_height) ? factor_width : factor_height;

    cairo_scale(cr[index], factor_width, factor_width);

    float r = SysPixelColor[IDX_COLOR_darkgray].r / 255.0;
    float g = SysPixelColor[IDX_COLOR_darkgray].g / 255.0;
    float b = SysPixelColor[IDX_COLOR_darkgray].b / 255.0;
    float alpha = 0xE0 / 255.0;
    cairo_set_source_rgb (cr[index],  r*alpha, g*alpha, b*alpha);
    cairo_paint (cr[index]);
    rsvg_handle_render_cairo_style (handle, cr[index], &button_color_pair[index], 1);
    cairo_restore (cr[index]);

    g_object_unref (handle);
}

// the window proc of status bar
static LRESULT StatusBarWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static PLOGFONT font_title;
    static PLOGFONT font_time;
    char buff [20];
    int length = 0;
    RECT rect[2] = {{10 * MARGIN_STATUS, MARGIN_STATUS + 3, g_rcScr.right * 0.618 - TIME_INFO_X - HEIGHT_STATUSBAR,  m_StatusBar_Height - MARGIN_STATUS}, \
                    {g_rcScr.right * 0.618 - TIME_INFO_X, MARGIN_STATUS, g_rcScr.right * 0.618 - 10 * MARGIN_STATUS, m_StatusBar_Height - MARGIN_STATUS}};
    char config_path[MAX_PATH + 1];
    char* etc_value = NULL;

    switch (message) 
    {
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            SetTextColor (hdc, DWORD2Pixel (hdc, 0xFFFFFFFF));
            SetBkMode (hdc, BM_TRANSPARENT);
            length = GetWindowTextLength(hWnd);
            {
                char title[length + 1];
                memset(title, 0, length + 1);
                GetWindowText(hWnd, title, length);
                draw_title(hdc, *(rect + 0), title, font_title);
            }

            mk_time(buff);
            SelectFont(hdc, font_time);
            DrawText (hdc, buff, strlen(buff), rect + 1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            paintWiFiIcon(hdc, wifi_icon_index);
            EndPaint (hWnd, hdc);
            return 0;

        case MSG_MAINWINDOW_CHANGE:
            if((wParam & 0xFFFF) == 0)          // It is Desk Top
            {
                m_direction = DIRECTION_SHOW;
                create_animation(hWnd);

                SetWindowText(hWnd, STRING_OS_NAME);
                InvalidateRect(hWnd, rect + 0, TRUE);
            }
            else
            {
                length = (int)lParam;
                if(length == 0)            // no title for main window
                {
                    m_direction = DIRECTION_SHOW;
                    create_animation(hWnd);

                    SetWindowText(hWnd, STRING_OS_NAME);
                    InvalidateRect(hWnd, rect + 0, TRUE);
                }
                else                            // get the title
                {
                    REQUEST request;
                    RequestInfo requestinfo;
                    char title[length + 1];

                    memset(title, 0, length + 1);

                    requestinfo.id = REQ_GET_TOPMOST_TITLE;
                    requestinfo.hWnd = m_hStatusBar;
                    requestinfo.iData0 = wParam;
                    requestinfo.iData1 = lParam;

                    request.id = UNFIXED_FORMAT_REQID;
                    request.data = (void *)&requestinfo;
                    request.len_data = sizeof(requestinfo);

                    ClientRequest(&request, title, length + 1);
                    if(title[0])
                    {
                        m_direction = DIRECTION_SHOW;
                        create_animation(hWnd);

                        SetWindowText(hWnd, title);
                        InvalidateRect(hWnd, rect + 0, TRUE);
                    }
                }
            }
            break;

        case MSG_WIFI_CHANGED:
            if(wParam == 0)
            {
                wifi_icon_index = 0;
                if(wifi_strength)
                {
                    m_direction = DIRECTION_SHOW;
                    create_animation(hWnd);
                }
            }
            else if(wParam < 33)
            {
                wifi_icon_index = 1;
                if(wifi_strength == 0)
                {
                    m_direction = DIRECTION_SHOW;
                    create_animation(hWnd);
                }
            }
            else if(wParam < 66)
            {
                wifi_icon_index = 2;
                if(wifi_strength == 0)
                {
                    m_direction = DIRECTION_SHOW;
                    create_animation(hWnd);
                }
            }
            else
            {
                wifi_icon_index = 3;
                if(wifi_strength == 0)
                {
                    m_direction = DIRECTION_SHOW;
                    create_animation(hWnd);
                }
            }
            wifi_strength = (int)wParam;
            break;

        case MSG_CREATE:
            if ((etc_value = getenv ("HISHELL_CFG_PATH")))
            {
                int len = strlen(etc_value);
                if (etc_value[len-1] == '/')
                    sprintf(config_path, "%s%s", etc_value, SYSTEM_CONFIG_FILE);
                else
                    sprintf(config_path, "%s/%s", etc_value, SYSTEM_CONFIG_FILE);
            }
            else
                sprintf(config_path, "%s", SYSTEM_CONFIG_FILE);
            GetIntValueFromEtcFile(config_path, "system", "statusbar_time", &m_statusbar_visible_time);
            GetIntValueFromEtcFile(config_path, "system", "statusbar_animation_show_time", &m_statusbar_show_time);
            GetIntValueFromEtcFile(config_path, "system", "statusbar_animation_hide_time", &m_statusbar_hide_time);

            font_title = CreateLogFont (FONT_TYPE_NAME_SCALE_TTF, "ttf-Source", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        m_StatusBar_Height * 0.5, 0);

            font_time = CreateLogFont (FONT_TYPE_NAME_SCALE_TTF, "ttf-Source", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, FONT_FLIP_NIL,
                        FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        m_StatusBar_Height * 0.4, 0);

            // load wifi icon
            for(int i = 0; i < 4; i++)
            {
                button_color_pair[i].name = "color";
                //button_color_pair[i].value = "#FA9C38";
                button_color_pair[i].value = "#FFFFFF";
                button_color_pair[i].important = 0;
            }
            loadSVGFromFile("res/wifi-0.svg", 0);
            loadSVGFromFile("res/wifi-1.svg", 1);
            loadSVGFromFile("res/wifi-2.svg", 2);
            loadSVGFromFile("res/wifi-3.svg", 3);

            SetTimer(hWnd, ID_TIMER, 100);
            SetTimer(hWnd, ID_SHOW_TIMER, m_statusbar_visible_time);
            m_direction = DIRECTION_HIDE;
            m_StatusBar_Y = 0;
            break;

        case MSG_TIMER:
            if(wParam == ID_TIMER)
                InvalidateRect(hWnd, rect + 1, TRUE);
            else if(wParam == ID_SHOW_TIMER)
            {
                m_direction = DIRECTION_HIDE;
                create_animation(hWnd);
                KillTimer(hWnd, ID_SHOW_TIMER);
            }
            break;

        case MSG_DESTROY:
            DestroyLogFont(font_title);
            DestroyLogFont(font_time);
        case MSG_CLOSE:
            KillTimer (hWnd, ID_TIMER);
            KillTimer (hWnd, ID_SHOW_TIMER);
            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

// create status bar
HWND create_status_bar (void)
{
    MAINWINCREATE CreateInfo;
    HWND hStatusBar;
    REQUEST request;
    const WINDOWINFO *pWindowInfo = NULL;
    RequestInfo requestinfo;
    ReplyInfo replyInfo;
    int ret = 0;

    m_StatusBar_Height = GetGDCapability(HDC_SCREEN, GDCAP_DPI);
    m_StatusBar_Height = HEIGHT_STATUSBAR * m_StatusBar_Height / 96;

    // create a main window
    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_WINTYPE_DOCKER | WS_EX_TROUNDCNS | WS_EX_BROUNDCNS;
    CreateInfo.spCaption = STRING_OS_NAME ;
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = StatusBarWinProc;
    CreateInfo.lx = g_rcScr.right * (1 - 0.618) / 2.0; 
    CreateInfo.ty = 0;
    CreateInfo.rx = CreateInfo.lx + g_rcScr.right * 0.618;
    CreateInfo.by = m_StatusBar_Height;
    CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xFF, 0xFF, 0xFF, 0xFF); 
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

//    hStatusBar = CreateMainWindowEx2 (&CreateInfo, 0L, NULL, NULL, ST_PIXEL_ARGB8888,
//                                MakeRGBA (SysPixelColor[IDX_COLOR_darkgray].r,
//                                          SysPixelColor[IDX_COLOR_darkgray].g,
//                                          SysPixelColor[IDX_COLOR_darkgray].b,
//                                          0xE0),
//                                CT_ALPHAPIXEL, 0xFF);

    hStatusBar = CreateMainWindowEx2 (&CreateInfo, 0L, NULL, NULL, ST_PIXEL_ARGB8888,
                                MakeRGBA (140, 140, 140, 0xF0),
                                CT_ALPHAPIXEL, 0xFF);
    if (hStatusBar == HWND_INVALID)
        return HWND_INVALID;

    // send status bar zNode index
    pWindowInfo = GetWindowInfo(hStatusBar);

    // submit zNode index to server
    requestinfo.id = REQ_SUBMIT_STATUSBAR_ZNODE;
    requestinfo.hWnd = hStatusBar;
    requestinfo.iData0 = pWindowInfo->idx_znode;
    request.id = FIXED_FORMAT_REQID;
    request.data = (void *)&requestinfo;
    request.len_data = sizeof(requestinfo);

    memset(&replyInfo, 0, sizeof(ReplyInfo));
    ClientRequest(&request, &replyInfo, sizeof(ReplyInfo));
    if((replyInfo.id == REQ_SUBMIT_STATUSBAR_ZNODE) && (replyInfo.iData0))
    {
        // create thread for hiBus
        pthread_attr_t thread_attr_hibus;           // attribute for hibus thread
        pthread_t a_thread_hibus;                   // thread id for hibus thread

        ret = pthread_attr_init(&thread_attr_hibus);
        if(ret != 0)
        {
            printf("hibus thread: Create thread atrribute failed.\n");
            return HWND_INVALID;
        }
        else
        {
            ret = pthread_attr_setdetachstate(&thread_attr_hibus, PTHREAD_CREATE_DETACHED);
            if(ret != 0)
            {
                printf("hibus thread: Detach thread attribute failed.\n");
                return HWND_INVALID;
            }
            else
            {
                ret = pthread_create(&a_thread_hibus, &thread_attr_hibus, thread_hibus, NULL);
                if(ret != 0)
                {
                    printf("hibus thread: Start thread failed.\n");
                    return HWND_INVALID;
                }
            }
            (void)pthread_attr_destroy(&thread_attr_hibus);
        }
    }
    else
        return HWND_INVALID;

    return hStatusBar;
}

