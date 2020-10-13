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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <wtf/RefCounted.h>

#include "resource.h"

class BrowserWindow : public RefCounted<BrowserWindow> {
public:
    virtual ~BrowserWindow() { };

    virtual LRESULT init() = 0;
    virtual HWND hwnd() = 0;

    virtual LRESULT loadURL(const char* url) = 0;
    virtual void navigateForward() = 0;
    virtual void navigateBackward() = 0;
    virtual void navigateStopLoading() = 0;
    virtual void navigateReload() = 0;
    virtual void navigateToHistory(UINT menuID) = 0;
    virtual double getEstimatedProgress() = 0;
    virtual void setPreference(UINT menuID, bool enable) = 0;
    virtual bool usesLayeredWebView() const { return false; }

    virtual void print() = 0;
    virtual void launchInspector() = 0;
    virtual void openProxySettings() = 0;

    virtual std::string userAgent() = 0;
    void setUserAgent(UINT menuID);
    virtual void setUserAgent(const char* ua) = 0;

    virtual void showLayerTree() = 0;
    virtual void updateStatistics(HWND dialog) = 0;

    virtual void resetZoom() = 0;
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
};
