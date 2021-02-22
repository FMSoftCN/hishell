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

#include "WebKitBrowserWindow.h"

#include <vector>
#include <WebKit/WKCredentialTypes.h>
#include <WebKit/WKAuthenticationDecisionListener.h>
#include <WebKit/WKInspector.h>
#include <WebKit/WKAuthenticationChallenge.h>
#include <WebKit/WKProtectionSpace.h>
#include <WebKit/WKProtectionSpaceTypes.h>
#include <WebKit/WKCredential.h>


#include "Common.h"
#include "../include/sysconfig.h"

extern struct Window_Info window_info[MAX_TARGET_NUMBER];
extern int m_target_blank_index;


std::string createString(WKStringRef wkString)
{
    size_t maxSize = WKStringGetMaximumUTF8CStringSize(wkString);

    std::vector<char> wkCharBuffer(maxSize);
    size_t actualLength = WKStringGetUTF8CString(wkString, wkCharBuffer.data(), maxSize);
    return std::string(wkCharBuffer.data(), actualLength);
}

std::string createString(WKURLRef wkURL)
{
    WKRetainPtr<WKStringRef> url = adoptWK(WKURLCopyString(wkURL));
    return createString(url.get());
}

WKRetainPtr<WKStringRef> createWKString(std::string str)
{
    return adoptWK(WKStringCreateWithUTF8CString(str.c_str()));
}

WKRetainPtr<WKURLRef> createWKURL(std::string url)
{
    return adoptWK(WKURLCreateWithUTF8CString(url.c_str()));
}

WebKitBrowserWindow& toWebKitBrowserWindow(const void *clientInfo)
{
    return *const_cast<WebKitBrowserWindow*>(static_cast<const WebKitBrowserWindow*>(clientInfo));
}


// it is WKPageNavigationClientV0 callback
static void didFinishNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo)
{
    WKRetainPtr<WKStringRef> title = adoptWK(WKPageCopyTitle(page));
    std::string titleString = createString(title.get()) + " [WebKit]";
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    SetWindowText(thisWindow.mainHwnd(), titleString.c_str());
}

static void didCommitNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);

    WKRetainPtr<WKURLRef> wkurl = adoptWK(WKPageCopyCommittedURL(page));
    std::string urlString = createString(wkurl.get());
    SetWindowText(thisWindow.mainHwnd(), urlString.c_str());
}

static void didReceiveAuthenticationChallenge(WKPageRef page, WKAuthenticationChallengeRef challenge, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    auto protectionSpace = WKAuthenticationChallengeGetProtectionSpace(challenge);
    auto decisionListener = WKAuthenticationChallengeGetDecisionListener(challenge);
    auto authenticationScheme = WKProtectionSpaceGetAuthenticationScheme(protectionSpace);

    if (authenticationScheme == kWKProtectionSpaceAuthenticationSchemeServerTrustEvaluationRequested) {
        if (thisWindow.canTrustServerCertificate(protectionSpace)) {
            WKRetainPtr<WKStringRef> username = createWKString("accept server trust");
            WKRetainPtr<WKStringRef> password = createWKString("");
            WKRetainPtr<WKCredentialRef> wkCredential = adoptWK(WKCredentialCreate(username.get(), password.get(), kWKCredentialPersistenceForSession));
            WKAuthenticationDecisionListenerUseCredential(decisionListener, wkCredential.get());
            return;
        }
    } else {
        WKRetainPtr<WKStringRef> realm(WKProtectionSpaceCopyRealm(protectionSpace));

        if (auto credential = askCredential(thisWindow.hwnd(), createString(realm.get()))) {
            WKRetainPtr<WKStringRef> username = createWKString(credential->username);
            WKRetainPtr<WKStringRef> password = createWKString(credential->password);
            WKRetainPtr<WKCredentialRef> wkCredential = adoptWK(WKCredentialCreate(username.get(), password.get(), kWKCredentialPersistenceForSession));
            WKAuthenticationDecisionListenerUseCredential(decisionListener, wkCredential.get());
            return;
        }
    }

    WKAuthenticationDecisionListenerUseCredential(decisionListener, nullptr);
}
 
static void renderingProgressDidChange(WKPageRef page, WKPageRenderingProgressEvents progressEvents, WKTypeRef userData, const void* clientInfo)
{
}

static void didChangeEstimatedProgress(WKPageRef page, const void* clientInfo)
{
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    _WRN_PRINTF(" ############# progress=%f\n", thisWindow.getEstimatedProgress());
}

static void decidePolicyForNavigationResponse(WKPageRef page, WKNavigationResponseRef navigationResponse, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
}

static void decidePolicyForNavigationActionHBD(const void * decisionTypePointer, const void * decisionPointer)
{
    WebKitPolicyDecision * decision = (WebKitPolicyDecision *)decisionPointer;
    WebKitPolicyDecisionType * decisionType = (WebKitPolicyDecisionType *)decisionTypePointer;

    if (*decisionType != WEBKIT_POLICY_DECISION_TYPE_RESPONSE)
        return;

    WebKitResponsePolicyDecision *responseDecision = WEBKIT_RESPONSE_POLICY_DECISION(decision);
    if (webkit_response_policy_decision_is_mime_type_supported(responseDecision))
        return;
    WebKitURIRequest *request = webkit_response_policy_decision_get_request(responseDecision);
    const char *requestURI = webkit_uri_request_get_uri(request);
//    if (g_strcmp0(webkit_web_resource_get_uri(mainResource), requestURI))
//        return;

    webkit_policy_decision_download(decision);
    MessageBox(NULL, "Do not Support Download", "Warning", MB_OK);
}
/*
    The rest is in WKPageNavigationClient.h 
*/



// it is WKPageUIClientV13 callback
static WKPageRef createNewPage(WKPageRef page, WKPageConfigurationRef configuration, WKNavigationActionRef navigationAction, WKWindowFeaturesRef windowFeatures, const void *clientInfo)
{
/*
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    BrowserWindow * browserWindow = NULL;
    RECT rect = GetScreenRect();
    rect.top += IDC_ADDRESS_HEIGHT + IDC_ADDRESS_TOP;

    browserWindow = WebKitBrowserWindow::create(IDC_BROWSER, rect, thisWindow.mainHwnd(), HWND_INVALID);

    if(browserWindow)
    {
        auto page = WKViewGetPage(((WebKitBrowserWindow *)browserWindow)->getView().get());
//        return WKViewGetPage(((WebKitBrowserWindow *)browserWindow)->getView().get());
        return page;
    }
    else
        return nullptr;
*/
#if 0 
    auto& newWindow = MainWindow::create().leakRef();
    auto factory = [configuration](HWND mainWnd, HWND urlBarWnd, bool, bool) -> auto {
        return adoptRef(*new WebKitBrowserWindow(configuration, mainWnd, urlBarWnd));
    };
    bool ok = newWindow.init(factory, hInst);
    if (!ok)
        return nullptr;
    ShowWindow(newWindow.hwnd(), SW_SHOW);
    auto& newBrowserWindow = *static_cast<WebKitBrowserWindow*>(newWindow.browserWindow());
    WKRetainPtr<WKPageRef> newPage = WKViewGetPage(newBrowserWindow.m_view.get());
    return newPage.leakRef();
#endif
    return nullptr;
}

void close(WKPageRef page, const void* clientInfo)
{
    fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, __func__);
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);
    SendMessage(thisWindow.mainHwnd(), MSG_USER_CLOSE_VIEW, 0, (LPARAM)clientInfo);
    DestroyWindow(thisWindow.hwnd());
}

/*
    The rest is in WKPageUIClient.h
*/



// it is WKPagePolicyClientV1 callback
static void decidePolicyForNewWindowAction(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKURLRequestRef request, WKStringRef frameName, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    int i = 0;
    BOOL find = FALSE;

    WKURLRef url = WKURLRequestCopyURL(request);
    std::string urlString = createString(url);
    auto& thisWindow = toWebKitBrowserWindow(clientInfo);

    std::string framename = createString(frameName);
    for(i = 0; i < MAX_TARGET_NUMBER; i++)
    {
        if(strcmp(framename.c_str(), window_info[i].target_name) == 0)
        {
            find = TRUE;
            break;
        }
    }

    if(find)
    {
        if(i < 2)
            i = MAX_TARGET_NUMBER;
        else
        {
            memset(window_info[i].target_url, 0, MAX_TARGET_URL_LEN);
            sprintf(window_info[i].target_url, "%s", urlString.c_str());
        }
    }

    if(i == MAX_TARGET_NUMBER)
    {
        i = m_target_blank_index;
        memset(window_info[i].target_url, 0, MAX_TARGET_URL_LEN);
        sprintf(window_info[i].target_url, "%s", urlString.c_str());
    }

    SendMessage(thisWindow.mainHwnd(), MSG_USER_NEW_VIEW, (WPARAM)i, NULL); 
    return;
}


static void decidePolicyForResponse(WKPageRef page, WKFrameRef frame, WKURLResponseRef response, WKURLRequestRef request, bool canShowMIMEType, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    return;
}


/*
static void decidePolicyForNavigationAction_deprecatedForUseWithV0(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    return;
}

static void decidePolicyForResponse_deprecatedForUseWithV0(WKPageRef page, WKFrameRef frame, WKURLResponseRef response, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    return;
}

static void unableToImplementPolicy(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef userData, const void* clientInfo)
{
    return;
}
*/



// it is WKPageStateClientV0 callback
/*
static void willChangeIsLoading(const void* clientInfo)
{
    return;
}
 
static void didChangeIsLoading(const void* clientInfo)
{
    return;
}

static void willChangeTitle(const void* clientInfo)
{
    return;
}

static void didChangeTitle(const void* clientInfo)
{
    return;
}

static void willChangeActiveURL(const void* clientInfo)
{
    return;
}

static void didChangeActiveURL(const void* clientInfo)
{
    return;
}

static void willChangeHasOnlySecureContent(const void* clientInfo)
{
    return;
}

static void didChangeHasOnlySecureContent(const void* clientInfo)
{
    return;
}

static void willChangeEstimatedProgress(const void* clientInfo)
{
    return;
}

static void didChangeEstimatedProgress(const void* clientInfo)
{
    return;
}

static void willChangeCanGoBack(const void* clientInfo)
{
    return;
}

static void didChangeCanGoBack(const void* clientInfo)
{
    return;
}

static void willChangeCanGoForward(const void* clientInfo)
{
    return;
}

static void didChangeCanGoForward(const void* clientInfo)
{
    return;
}

static void willChangeNetworkRequestsInProgress(const void* clientInfo)
{
    return;
}

static void didChangeNetworkRequestsInProgress(const void* clientInfo)
{
    return;
}

static void willChangeCertificateInfo(const void* clientInfo)
{
    return;
}

static void didChangeCertificateInfo(const void* clientInfo)
{
    return;
}

static void willChangeWebProcessIsResponsive(const void* clientInfo)
{
    return;
}

static void didChangeWebProcessIsResponsive(const void* clientInfo)
{
    return;
}

static void didSwapWebProcesses(const void* clientInfo)
{
    return;
}
*/


BrowserWindow* WebKitBrowserWindow::create(LINT id, RECT rect, HWND mainWnd, HWND urlBarWnd)
{
    auto conf = adoptWK(WKPageConfigurationCreate());

    auto prefs = adoptWK(WKPreferencesCreate());

    auto pageGroup = adoptWK(WKPageGroupCreateWithIdentifier(createWKString("HbdMiniBrowser").get()));
    WKPageConfigurationSetPageGroup(conf.get(), pageGroup.get());
    WKPageGroupSetPreferences(pageGroup.get(), prefs.get());

    WKPreferencesSetMediaCapabilitiesEnabled(prefs.get(), false);
    WKPreferencesSetDeveloperExtrasEnabled(prefs.get(), true);
    WKPageConfigurationSetPreferences(conf.get(), prefs.get());

    auto context =adoptWK(WKContextCreateWithConfiguration(nullptr));
    WKPageConfigurationSetContext(conf.get(), context.get());

    return create(id, rect, mainWnd, urlBarWnd, conf.get());
}

BrowserWindow* WebKitBrowserWindow::create(LINT id, RECT rect, HWND mainWnd, HWND urlBarWnd, WKPageConfigurationRef conf)
{
    return new WebKitBrowserWindow(id, rect,mainWnd, urlBarWnd, conf);
}

WebKitBrowserWindow::WebKitBrowserWindow(LINT id, RECT rect, HWND mainWnd, HWND urlBarWnd, WKPageConfigurationRef conf)
    : m_hMainWnd(mainWnd)
    , m_urlBarWnd(urlBarWnd)
{
    m_view = adoptWK(WKViewCreateWithConfiguration(rect, mainWnd, id, conf));
    WKViewSetIsInWindow(m_view.get(), true);
    // enable / disable touch support
    WKViewSetTouchEnabled(m_view.get(), true);

    auto page = WKViewGetPage(m_view.get());

    // it is WKPageNavigationClientV0 callback
    WKPageNavigationClientV0 navigationClient = { };
    navigationClient.base.version = 0;
    navigationClient.base.clientInfo = this;
    navigationClient.didFinishNavigation = didFinishNavigation;
    navigationClient.didCommitNavigation = didCommitNavigation;
    navigationClient.didReceiveAuthenticationChallenge = didReceiveAuthenticationChallenge;
    navigationClient.renderingProgressDidChange = renderingProgressDidChange;
    navigationClient.didChangeEstimatedProgress = didChangeEstimatedProgress;
    navigationClient.decidePolicyForNavigationActionHBD = decidePolicyForNavigationActionHBD;
    navigationClient.decidePolicyForNavigationResponse = decidePolicyForNavigationResponse;
    WKPageSetPageNavigationClient(page, &navigationClient.base);


    // it is WKPageUIClientV13 callback
    WKPageUIClientV13 uiClient = { };
    uiClient.base.version = 13;
    uiClient.base.clientInfo = this;
    uiClient.createNewPage = createNewPage;
    uiClient.close = close;
    WKPageSetPageUIClient(page, &uiClient.base);


    // it is WKPagePolicyClientV0 callback 
    WKPagePolicyClientV1 policyClient = { };
    policyClient.base.version = 1;
    policyClient.base.clientInfo = this;
    policyClient.decidePolicyForNewWindowAction = decidePolicyForNewWindowAction;
    policyClient.decidePolicyForResponse = decidePolicyForResponse;
    /*
    policyClient.decidePolicyForNavigationAction_deprecatedForUseWithV0 = decidePolicyForNavigationAction_deprecatedForUseWithV0;
    policyClient.decidePolicyForResponse_deprecatedForUseWithV0 = decidePolicyForResponse_deprecatedForUseWithV0;
    policyClient.unableToImplementPolicy = unableToImplementPolicy;
    */
    WKPageSetPagePolicyClient(page, &policyClient.base);


    // it is WKPageStateClientV0 callback
    WKPageStateClientV0 stateClient = { };
    stateClient.base.version = 0;
    stateClient.base.clientInfo = this;
    /*
    stateClient.willChangeIsLoading = willChangeIsLoading;
    stateClient.didChangeIsLoading = didChangeIsLoading;
    stateClient.willChangeTitle = willChangeTitle;
    stateClient.didChangeTitle = didChangeTitle;
    stateClient.willChangeActiveURL = willChangeActiveURL;
    stateClient.didChangeActiveURL = didChangeActiveURL;
    stateClient.willChangeHasOnlySecureContent = willChangeHasOnlySecureContent;
    stateClient.didChangeHasOnlySecureContent = didChangeHasOnlySecureContent;
    stateClient.willChangeEstimatedProgress = willChangeEstimatedProgress;
    stateClient.didChangeEstimatedProgress = didChangeEstimatedProgress;
    stateClient.willChangeCanGoBack = willChangeCanGoBack;
    stateClient.didChangeCanGoBack = didChangeCanGoBack;
    stateClient.willChangeCanGoForward = willChangeCanGoForward;
    stateClient.didChangeCanGoForward = didChangeCanGoForward;
    stateClient.willChangeNetworkRequestsInProgress = willChangeNetworkRequestsInProgress;
    stateClient.didChangeNetworkRequestsInProgress = didChangeNetworkRequestsInProgress;
    stateClient.willChangeCertificateInfo = willChangeCertificateInfo;
    stateClient.didChangeCertificateInfo = didChangeCertificateInfo;
    stateClient.willChangeWebProcessIsResponsive = willChangeWebProcessIsResponsive;
    stateClient.didChangeWebProcessIsResponsive = didChangeWebProcessIsResponsive;
    stateClient.didSwapWebProcesses = didSwapWebProcesses;
    */
    WKPageSetPageStateClient(page, &stateClient.base);
    m_pageConfiguration = WKPageCopyPageConfiguration(page);

    updateProxySettings();
    resetZoom();

}

void WebKitBrowserWindow::updateProxySettings()
{
}

LRESULT WebKitBrowserWindow::init()
{
    return ERR_OK;
}

HWND WebKitBrowserWindow::hwnd()
{
    return WKViewGetWindow(m_view.get());
}

LRESULT WebKitBrowserWindow::loadURL(const char* url)
{
    auto page = WKViewGetPage(m_view.get());
    WKPageLoadURL(page, createWKURL(url).get());
    return true;
}

void WebKitBrowserWindow::navigateForward()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageGoForward(page);
}

void WebKitBrowserWindow::navigateBackward()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageGoBack(page);
}

void WebKitBrowserWindow::navigateStopLoading()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageStopLoading(page);
}

void WebKitBrowserWindow::navigateReload()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageReload(page);
}

void WebKitBrowserWindow::navigateToHistory(UINT menuID)
{
    // Not implemented
}

double WebKitBrowserWindow::getEstimatedProgress()
{
    auto page = WKViewGetPage(m_view.get());
    return WKPageGetEstimatedProgress(page);
}

void WebKitBrowserWindow::setPreference(UINT menuID, bool enable)
{
    auto page = WKViewGetPage(m_view.get());
    auto pgroup = WKPageGetPageGroup(page);
    auto pref = WKPageGroupGetPreferences(pgroup);
    switch (menuID) {
    case IDM_DISABLE_IMAGES:
        WKPreferencesSetLoadsImagesAutomatically(pref, !enable);
        break;
    case IDM_DISABLE_JAVASCRIPT:
        WKPreferencesSetJavaScriptEnabled(pref, !enable);
        break;
    }
}

void WebKitBrowserWindow::print()
{
    // Not implemented
}

void WebKitBrowserWindow::launchInspector()
{
    auto page = WKViewGetPage(m_view.get());
    auto inspector = WKPageGetInspector(page);
    WKInspectorShow(inspector);
}

void WebKitBrowserWindow::openProxySettings()
{
    if (askProxySettings(m_hMainWnd, m_proxy))
        updateProxySettings();
}

void WebKitBrowserWindow::setUserAgent(const char* agent)
{
    auto page = WKViewGetPage(m_view.get());
    auto ua = createWKString(agent);
    WKPageSetCustomUserAgent(page, ua.get());
}

std::string WebKitBrowserWindow::userAgent()
{
    auto page = WKViewGetPage(m_view.get());
    auto ua = adoptWK(WKPageCopyUserAgent(page));
    return createString(ua.get()).c_str();
}

void WebKitBrowserWindow::showLayerTree()
{
    // Not implemented
}

void WebKitBrowserWindow::updateStatistics(HWND hDlg)
{
    // Not implemented
}


void WebKitBrowserWindow::resetZoom()
{
    auto page = WKViewGetPage(m_view.get());
    WKPageSetPageZoomFactor(page, 1.0);
}

void WebKitBrowserWindow::zoomIn()
{
    auto page = WKViewGetPage(m_view.get());
    double s = WKPageGetPageZoomFactor(page);
    WKPageSetPageZoomFactor(page, s * 1.25);
}

void WebKitBrowserWindow::zoomOut()
{
    auto page = WKViewGetPage(m_view.get());
    double s = WKPageGetPageZoomFactor(page);
    WKPageSetPageZoomFactor(page, s * 0.8);
}

bool WebKitBrowserWindow::canTrustServerCertificate(WKProtectionSpaceRef protectionSpace)
{
    return false;
}
