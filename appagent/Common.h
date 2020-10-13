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

#pragma once

#include <wtf/Optional.h>
#include <string>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "BrowserWindow.h"
#include "../include/sysconfig.h"

// Control ID
#define IDC_BROWSER     140

// user define message
#define MSG_USER_NEW_VIEW       (MSG_USER + 100 + 0)
#define MSG_USER_CLOSE_VIEW       (MSG_USER + 100 + 1)

// define contrl
#define IDC_ADDRESS_LEFT    5
#define IDC_ADDRESS_TOP     3
#define IDC_ADDRESS_HEIGHT  20   

#define MAX_TARGET_URL_LEN  512

struct Window_Info {
    HWND    hWnd;
    char    target_name[MAX_TARGET_NAME_LEN];
    char    target_url[MAX_TARGET_URL_LEN];
    BrowserWindow * view;
};

struct Credential {
    std::string username;
    std::string password;
};

struct ProxySettings {
    bool enable { true };
    bool custom { false };
    std::string url;
    std::string excludeHosts;
};

Optional<Credential> askCredential(HWND, const std::string& realm);
bool askProxySettings(HWND, ProxySettings&);

bool askServerTrustEvaluation(HWND, const std::string& pems);
std::string replaceString(std::string src, const std::string& oldValue, const std::string& newValue);
