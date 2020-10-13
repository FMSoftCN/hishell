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

#define PLATFORM(HBD)   1

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <WebKit/WKGeometry.h>
#include <WebKit/WKView.h>
#include <WebKit/WKURL.h>
#include <WebKit/WKPage.h>

#include <glib.h>
#include "WebKitBrowserWindow.h"

#ifdef _LANG_ZHCN
#include "browser_res_cn.h"
#elif defined _LANG_ZHTW
#include "browser_res_tw.h"
#else
#include "browser_res_en.h"
#endif

#include "../include/sysconfig.h"

RECT m_ScreenRect;

extern WebKitBrowserWindow& toWebKitBrowserWindow(const void *clientInfo);
extern std::string createString(WKStringRef wkString);
extern std::string createString(WKURLRef wkURL);

//static const char *gUrl = "https://hybridos.fmsoft.cn/";
//static const char *gUrl = "http://www.sina.com.cn/";
static const char *gUrl = "file://localhost/home/projects/hiwebkit/Websites/fmsoft.webkit.org/e.html";
struct Window_Info window_info[MAX_TARGET_NUMBER];
int m_target_blank_index = -1;

bool g_has_page_configuration = false;
WKPageConfigurationRef g_page_configuration;

static LRESULT MainFrameProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static void create_control(HWND hwnd, int index)
{
    BrowserWindow * browserWindow = NULL;
    int i = 0;
    RECT rect = m_ScreenRect;
    
    if (g_has_page_configuration)
    {
        browserWindow = WebKitBrowserWindow::create(IDC_BROWSER, rect, hwnd, HWND_INVALID, g_page_configuration);
    }
    else
    {
        browserWindow = WebKitBrowserWindow::create(IDC_BROWSER, rect, hwnd, HWND_INVALID);
    }
    if (browserWindow)
    {
        if (!g_has_page_configuration)
        {
            g_page_configuration = ((WebKitBrowserWindow*)browserWindow)->getPageConfiguration();
            g_has_page_configuration = true;
        }
        window_info[index].hWnd = hwnd;
        window_info[index].view = browserWindow;
        ShowWindow(browserWindow->hwnd(), SW_SHOW);

        if(window_info[index].target_url)
        {
            browserWindow->loadURL(window_info[index].target_url);
        }
        else
        {
            browserWindow->loadURL(gUrl);
        }
    }
}

static HWND CreateAPPAgentMaindow(int index)
{
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    
    m_ScreenRect = GetScreenRect();

    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = HL_ST_CAP;
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MainFrameProc;
    CreateInfo.lx = m_ScreenRect.left;
    CreateInfo.ty = m_ScreenRect.top;
    CreateInfo.rx = m_ScreenRect.right;
    CreateInfo.by = m_ScreenRect.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = (DWORD)index;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return HWND_INVALID;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    return hMainWnd;
}

static LRESULT MainFrameProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    int index = 0;
    BrowserWindow * view = NULL;
    int i = 0;

    switch (message) {
        case MSG_CREATE:
            index = (int) GetWindowAdditionalData(hWnd);
            create_control(hWnd, index);
            break;

        case MSG_USER_NEW_VIEW:
            i = (int)wParam;
            if(window_info[i].hWnd)
            {
                SetActiveWindow(window_info[i].hWnd);
                window_info[i].view->loadURL(window_info[i].target_url);
            }
            else
                CreateAPPAgentMaindow(i);
            break;

        case MSG_USER_CLOSE_VIEW:
            for(i == 0; i < MAX_TARGET_NUMBER; i++)
            {
                if(window_info[i].hWnd == hWnd)
                    break;
            }
            if(i < MAX_TARGET_NUMBER)
            {
                window_info[i].hWnd = NULL;
                window_info[i].view = NULL;
                if(i > 2)
                    window_info[i].target_url[0] = 0;
            }

            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            return 0;

        case MSG_CLOSE:
            for(i == 0; i < MAX_TARGET_NUMBER; i++)
            {
                if(window_info[i].hWnd == hWnd)
                    break;
            }
            if(i < MAX_TARGET_NUMBER)
            {
                SendMessage(window_info[i].view->hwnd(), MSG_CLOSE, 0, 0);
            }
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

void performMessageLoopTasks()
{
    g_main_context_iteration(0, false);
}

static void filter_browser_message(MSG * msg)
{
    int id = -1;

    if(msg->message == MSG_BROWSER_SHOW)
    {
        id = msg->wParam;


        if(id == -1)
            id = m_target_blank_index;

        if(window_info[id].hWnd)
        {
            REQUEST request;
            RequestInfo requestinfo;
            ReplyInfo replyInfo;

            SetActiveWindow(window_info[id].hWnd);
            SetFocus(window_info[id].hWnd);
            window_info[id].view->loadURL(window_info[id].target_url);

            requestinfo.id = REQ_SUBMIT_TOPMOST;
            requestinfo.hWnd = window_info[id].hWnd;
            requestinfo.iData0 = 0;
            request.id = FIXED_FORMAT_REQID;
            request.data = (void *)&requestinfo;
            request.len_data = sizeof(requestinfo);

            memset(&replyInfo, 0, sizeof(ReplyInfo));
            ClientRequest(&request, &replyInfo, sizeof(ReplyInfo));
            if((replyInfo.id == REQ_SUBMIT_TOGGLE) && (replyInfo.iData0))
            {
            }
            else
            {
            }
        }
        else
            CreateAPPAgentMaindow(id);
    }
}

static void initial_window_info()
{
    char config_path[MAX_PATH + 1];
    char* etc_value = NULL;
    char unknown_target[MAX_TARGET_NAME_LEN];
    int i = 0;

    if ((etc_value = getenv ("HISHELL_CFG_PATH")))
    {
        int len = strlen(etc_value);
        if (etc_value[len-1] == '/')
        {
            sprintf(config_path, "%s%s", etc_value, SYSTEM_CONFIG_FILE);
        }
        else
        {
            sprintf(config_path, "%s/%s", etc_value, SYSTEM_CONFIG_FILE);
        }
    }
    else
    {
        sprintf(config_path, "%s", SYSTEM_CONFIG_FILE);
    }

    memset(&window_info, 0, sizeof(struct Window_Info) * MAX_TARGET_NUMBER);
    memset(unknown_target, 0, MAX_TARGET_NAME_LEN);

    GetValueFromEtcFile(config_path, "system", "target0", window_info[0].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url0", window_info[0].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target1", window_info[1].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url1", window_info[1].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target2", window_info[2].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url2", window_info[2].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target3", window_info[3].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url3", window_info[3].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target4", window_info[4].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url4", window_info[4].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target5", window_info[5].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url5", window_info[5].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target6", window_info[6].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url6", window_info[6].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target7", window_info[7].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url7", window_info[7].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "target8", window_info[8].target_name, MAX_TARGET_NAME_LEN);
    GetValueFromEtcFile(config_path, "system", "url8", window_info[8].target_url, MAX_TARGET_URL_LEN);

    GetValueFromEtcFile(config_path, "system", "unknown_target", unknown_target, MAX_TARGET_NAME_LEN);

    for(i = 0; i < MAX_TARGET_NUMBER; i++)
        if(window_info[i].target_name[0] && (strcmp(window_info[i].target_name, unknown_target) == 0))
            break;

    if(i < MAX_TARGET_NUMBER)
        m_target_blank_index = i;
    else
        m_target_blank_index = -1;

}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;

#if 0
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
#endif
    
    m_ScreenRect = GetScreenRect();

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "HybridOS" , 0 , 0);
#endif

    initial_window_info();

#if 0
    CreateInfo.dwStyle = 
        WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = HL_ST_CAP;
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MainFrameProc;
    CreateInfo.lx = m_ScreenRect.left;
    CreateInfo.ty = m_ScreenRect.top;
    CreateInfo.rx = m_ScreenRect.right;
    CreateInfo.by = m_ScreenRect.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
#endif
    while (GetMessage(&Msg, HWND_DESKTOP)) {
        performMessageLoopTasks();
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
        filter_browser_message(&Msg);
        performMessageLoopTasks();
    }

#if 0
    MainWindowThreadCleanup (hMainWnd);
#endif

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
