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

#include "BrowserWindow.h"
#include "Common.h"

#include <WebKit/WKRetainPtr.h>
#include <WebKit/WebKit2_C.h>
#include <webkit2/webkit2.h> 

#include <unordered_map>

class WebKitBrowserWindow : public BrowserWindow {
public:
    static WebKitBrowserWindow* create(LINT id, RECT rect, HWND mainWnd, HWND urlBarWnd);
    static WebKitBrowserWindow* create(LINT id, RECT rect, HWND mainWnd, HWND urlBarWnd, WKPageConfigurationRef conf);

    HWND hwnd() override;
    HWND mainHwnd() { return m_hMainWnd; }
    bool canTrustServerCertificate(WKProtectionSpaceRef);
    double getEstimatedProgress() override;
    WKRetainPtr<WKViewRef> getView()    { return m_view; }
    WKPageConfigurationRef getPageConfiguration() { return m_pageConfiguration; }

private:
    WebKitBrowserWindow(LINT id, RECT rect, HWND mainWnd, HWND urlBarWnd, WKPageConfigurationRef conf);

    LRESULT init() override;

    LRESULT loadURL(const char* url) override;
    void navigateForward() override;
    void navigateBackward() override;
    void navigateStopLoading() override;
    void navigateReload() override;
    void navigateToHistory(UINT menuID) override;
    void setPreference(UINT menuID, bool enable) override;

    void print() override;
    void launchInspector() override;
    void openProxySettings() override;

    std::string userAgent() override;
    void setUserAgent(const char* agent) override;

    void showLayerTree() override;
    void updateStatistics(HWND dialog) override;

    void resetZoom() override;
    void zoomIn() override;
    void zoomOut() override;

    void updateProxySettings();

    WKRetainPtr<WKViewRef> m_view;
    HWND m_hMainWnd { nullptr };
    HWND m_urlBarWnd { nullptr };
    ProxySettings m_proxy { };
    std::unordered_map<std::string, std::string> m_acceptedServerTrustCerts;
    WKPageConfigurationRef m_pageConfiguration;
};
