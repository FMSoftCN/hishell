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
#include <hibox/json.h>

#include "../include/sysconfig.h"
#include "config.h"
#include "wifi.h"

extern HWND m_hStatusBar;                       // handle of status bar
static char device_name[64];                    // wifi device name
static char connect_bssid[128];                 // bssid of connected wifi
static char command_device[128];                // parameter for hibus command
static char wifi_ssid[128];
static char wifi_password[64];

static int wifi_connect_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    int error_code = 0;

//printf("========================================connect, %s\n", ret_value);
    jo = hibus_json_object_from_string(ret_value, strlen(ret_value), 10);
        return 0;

    // get error code
    if(json_object_object_get_ex(jo, "errCode", &jo_tmp) == 0)
    {
        if(jo)
            json_object_put (jo);
        return 0;
    }

    error_code = json_object_get_int(jo_tmp); 

    if(error_code == 0)
    {
        if(m_hStatusBar)
            PostMessage(m_hStatusBar, MSG_WIFI_CHANGED, 200, 0);
    }

    if(jo)
        json_object_put (jo);

    return 0;
}

static void wifi_signal_changed_handler(hibus_conn* conn, const char* from_endpoint, const char* bubble_name, const char* bubble_data)
{
    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    int strength = 0;
    const char * ssid = NULL;

//    printf("========================================get signal changed, %s\n", bubble_data);

    jo = hibus_json_object_from_string(bubble_data, strlen(bubble_data), 10);
    if(jo == NULL)
        return;

    // get ssid
    if(json_object_object_get_ex(jo, "ssid", &jo_tmp) == 0)
    {
        if(jo)
            json_object_put (jo);
        return;
    }

    ssid = json_object_get_string(jo_tmp);
    if(strncasecmp(ssid, wifi_ssid, strlen(wifi_ssid)) == 0)
    {
        // get signal strength
        if(json_object_object_get_ex(jo, "signalStrength", &jo_tmp) == 0)
            return;

        strength = json_object_get_int(jo_tmp); 

        // send signal info
        if(m_hStatusBar)
            PostMessage(m_hStatusBar, MSG_WIFI_CHANGED, strength, 0);
    }

    if(jo)
        json_object_put (jo);

    return;
}

static void wifi_hotspot_changed_handler(hibus_conn* conn, const char* from_endpoint, const char* bubble_name, const char* bubble_data)
{
    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    struct array_list *array = NULL;
    struct json_object *obj = NULL;
    const char * bssid = NULL;
    int i = 0;
    char * endpoint = NULL;
    char command[512];

// printf("========================================get hotspot changed, %s\n", bubble_data);

    jo = hibus_json_object_from_string(bubble_data, strlen(bubble_data), 10);
    if(jo == NULL)
        return;

    // get missed network 
    if(json_object_object_get_ex(jo, "missed", &jo_tmp) == 0)
    {
        if(jo)
            json_object_put (jo);
        return;
    }

    array = json_object_get_array(jo_tmp); 
    for(i = 0; i < array_list_length(array); i++)
    {
        obj = array_list_get_idx(array, i);

        // get bssid
        if(obj)
        {
            if(json_object_object_get_ex(obj, "bssid", &jo_tmp) == 0)
                continue;
            else
            {
                bssid = json_object_get_string(jo_tmp);
                if(strncasecmp(bssid, connect_bssid, strlen(bssid)) == 0)
                {
//                    printf("===============================================================missed, %s\n", bssid);
                    if(m_hStatusBar)
                        PostMessage(m_hStatusBar, MSG_WIFI_CHANGED, 0, 0);
                    break;
                }
            }
        }
    }

    // get found network 
    if(json_object_object_get_ex(jo, "found", &jo_tmp) == 0)
        return;

    array = json_object_get_array(jo_tmp); 
    for(i = 0; i < array_list_length(array); i++)
    {
        obj = array_list_get_idx(array, i);

        // get bssid
        if(obj)
        {
            if(json_object_object_get_ex(obj, "ssid", &jo_tmp) == 0)
                continue;
            else
            {
                bssid = json_object_get_string(jo_tmp);
                if(strncasecmp(bssid, wifi_ssid, strlen(wifi_ssid)) == 0)
                {
//                    printf("===============================================================find ssid, %s\n", bssid);
                    if(json_object_object_get_ex(obj, "bssid", &jo_tmp) == 0)
                        continue;
                    else
                    {
                        bssid = json_object_get_string(jo_tmp);
                        memset(connect_bssid, 0, 128);
                        sprintf(connect_bssid, "%s", bssid);
//                        printf("===============================================================find bssid, %s\n", bssid);
                    }

                    endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
                    memset(command, 0, 512);
                    sprintf(command, "{\"device\":\"%s\", \"ssid\":\"%s\", \"password\":\"%s\"}", device_name, wifi_ssid, wifi_password);
                    hibus_call_procedure(conn, endpoint, METHOD_WIFI_CONNECT_AP, command, 1000, wifi_connect_handler);
                    free(endpoint);
                    break;
                }
            }
        }
    }

    if(jo)
        json_object_put (jo);
}

// step 4: get wifi network info
static int wifi_get_network_info_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    int error_code = 0;
    int strength = 0;
    char * endpoint = NULL;
    char command[512];

//printf("======================================== wifi_get_network_info_handler %s\n", ret_value);
    jo = hibus_json_object_from_string(ret_value, strlen(ret_value), 10);
    if(jo == NULL)
        return 0;

    // get error code
    if(json_object_object_get_ex(jo, "errCode", &jo_tmp) == 0)
    {
        if(jo)
            json_object_put (jo);
        return 0;
    }

    error_code = json_object_get_int(jo_tmp); 

    if(error_code)
    {
        if(m_hStatusBar)
            PostMessage(m_hStatusBar, MSG_WIFI_CHANGED, 0, 0);

        endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
        memset(command, 0, 512);
        sprintf(command, "{\"device\":\"%s\", \"ssid\":\"%s\", \"password\":\"%s\"}", device_name, wifi_ssid, wifi_password);
        hibus_call_procedure(conn, endpoint, METHOD_WIFI_CONNECT_AP, command, 1000, wifi_connect_handler);
        free(endpoint);
    }
    else
    {
        if(json_object_object_get_ex(jo, "data", &jo_tmp) == 0)
        {
            if(jo)
                json_object_put (jo);
            return 0;
        }
        // get signal strength
        if(json_object_object_get_ex(jo_tmp, "signalStrength", &jo_tmp) == 0)
        {
            if(jo)
                json_object_put (jo);
            return 0;
        }

        strength = json_object_get_int(jo_tmp); 

        if(m_hStatusBar)
            PostMessage(m_hStatusBar, MSG_WIFI_CHANGED, strength, 0);
    }

    if(jo)
        json_object_put (jo);
    return 0;
}

// step 3: get expected wifi network bssid
static int wifi_scan_hotspots_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    char * endpoint = NULL;
    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    struct array_list *array = NULL;
    struct json_object *obj = NULL;
    const char * bssid = NULL;
    int i = 0;

//printf("========================================wifi_scan_hotspots_handler %s\n", ret_value);
    endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
    hibus_subscribe_event(conn, endpoint, WIFISIGNALSTRENGTHCHANGED, wifi_signal_changed_handler);
    hibus_subscribe_event(conn, endpoint, WIFIHOTSPOTSCHANGED, wifi_hotspot_changed_handler);
    hibus_call_procedure(conn, endpoint, METHOD_WIFI_GET_NETWORK_INFO, command_device, 1000, wifi_get_network_info_handler);
    free(endpoint);

    jo = hibus_json_object_from_string(ret_value, strlen(ret_value), 10);
    if(jo == NULL)
        return 0;

    // get missed network 
    if(json_object_object_get_ex(jo, "data", &jo_tmp) == 0)
    {
        if(jo)
            json_object_put (jo);
        return 0;
    }

    array = json_object_get_array(jo_tmp); 
    for(i = 0; i < array_list_length(array); i++)
    {
        obj = array_list_get_idx(array, i);

        // get bssid
        if(obj)
        {
            if(json_object_object_get_ex(obj, "ssid", &jo_tmp) == 0)
                continue;
            else
            {
                bssid = json_object_get_string(jo_tmp);
                if(strncasecmp(bssid, wifi_ssid, strlen(bssid)) == 0)
                {
//printf("=============================================================================================== ssid is %s\n", bssid);
                    if(json_object_object_get_ex(obj, "bssid", &jo_tmp) == 0)
                        continue;
                    else
                    {
                        bssid = json_object_get_string(jo_tmp);
                        if(bssid)
                        {
                            memset(connect_bssid, 0, 128);
                            sprintf(connect_bssid, "%s", bssid);
//printf("=============================================================================================== bssid is %s\n", bssid);
                        }
                    }

                    if(json_object_object_get_ex(obj, "isConnected", &jo_tmp) == 0)
                        continue;
                    else
                    {
                        bool connect = json_object_get_boolean(jo_tmp);
                        // send signal info
                        if(!connect)
                        {
                            char command[512];
                            memset(command, 0, 512);
                            endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
                            memset(command, 0, 512);
                            sprintf(command, "{\"device\":\"%s\", \"ssid\":\"%s\", \"password\":\"%s\"}", device_name, wifi_ssid, wifi_password);
                            hibus_call_procedure(conn, endpoint, METHOD_WIFI_CONNECT_AP, command, 1000, wifi_connect_handler);
                            free(endpoint);
//printf("=============================================================================================== connect %s ========================================\n", wifi_ssid);
                            break;
                        }
                    }

                    if(json_object_object_get_ex(obj, "signalStrength", &jo_tmp) == 0)
                        continue;
                    else
                    {
                        int strength = json_object_get_int(jo_tmp);
//printf("=============================================================================================== signal is %d\n", strength);
                        // send signal info
                        if(m_hStatusBar)
                            PostMessage(m_hStatusBar, MSG_WIFI_CHANGED, strength, 0);
                        break;
                    }
                }
                else
                    continue;
            }
        }
    }

    if(jo)
        json_object_put (jo);

    return 0;
}

// step 2: open wifi device
static int wifi_open_device_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    char * endpoint = NULL;

//    printf("========================================open device %s\n", ret_value);
    endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
    hibus_call_procedure(conn, endpoint, METHOD_WIFI_START_SCAN, command_device, 1000, wifi_scan_hotspots_handler);
    free(endpoint);

    return 0;
}

// step 1: get wifi device name
static int wifi_get_devices_info_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    hibus_json *jo = NULL;
    hibus_json *jo_tmp = NULL;
    struct array_list *array = NULL;
    struct json_object *obj = NULL;
    const char * bssid = NULL;
    int i = 0;
    char * endpoint = NULL;

//    printf("========================================get device, %s\n", ret_value);
    jo = hibus_json_object_from_string(ret_value, strlen(ret_value), 10);
    if(jo == NULL)
        return 0;

    // get missed network 
    if(json_object_object_get_ex(jo, "data", &jo_tmp) == 0)
    {
        if(jo)
            json_object_put (jo);
        return 0;
    }

    array = json_object_get_array(jo_tmp); 
    for(i = 0; i < array_list_length(array); i++)
    {
        obj = array_list_get_idx(array, i);

        // get bssid
        if(obj)
        {
            if(json_object_object_get_ex(obj, "type", &jo_tmp) == 0)
                continue;
            else
            {
                bssid = json_object_get_string(jo_tmp);
                if(strncasecmp(bssid, "wifi", strlen(bssid)) == 0)
                {
                    if(json_object_object_get_ex(obj, "device", &jo_tmp) == 0)
                        continue;
                    else
                    {
                        bssid = json_object_get_string(jo_tmp);
                        if(bssid)
                        {
                            memset(device_name, 0, 64);
                            sprintf(device_name, "%s", bssid);
//                            printf("================================================= find bssid = %s\n", device_name);

                            memset(command_device, 0, 128);
                            sprintf(command_device, "{\"device\":\"%s\"}", device_name);

                            endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
                            hibus_call_procedure(conn, endpoint, METHOD_NET_OPEN_DEVICE, command_device, 1000, wifi_open_device_handler);
                            free(endpoint);
                            break;
                        }
                    }
                }
                else
                    continue;
            }
        }
    }

    if(jo)
        json_object_put (jo);

    return 0;
}

// step final: close device
static int wifi_close_device_handler(hibus_conn* conn, const char* from_endpoint, const char* method_name, int ret_code, const char* ret_value)
{
    return 0;
}

// wpa_supplicant must exist
static bool wpa_IsRun(void)
{
    FILE * fp = NULL;
    int count = 1;
    char buf[100];
    char command[150];

    sprintf(command, "ps -ef | grep -w wpa_supplicant | wc -l");

    if((fp = popen(command,"r")) == NULL)
        return false;
    if((fgets(buf, 100, fp)) != NULL)
        count = atoi(buf);
    pclose(fp);
    fp = NULL;

    if((count - 1) == 1) 
        return false;
    else 
        return true;

    return false;
}

// hibus thread
void * thread_hibus(void * arg)
{
    int fd_socket = -1;
    hibus_conn * hibus_context = NULL;
    char * endpoint = NULL;
    char config_path[MAX_PATH + 1];
    char* etc_value = NULL;
    
    while(!wpa_IsRun())
        sleep(2);

    // get the default ssid and password
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
    GetValueFromEtcFile(config_path, "wifi_test", "ssid", wifi_ssid, 128);
    GetValueFromEtcFile(config_path, "wifi_test", "password", wifi_password, 64);
printf("======================================================= find wpa_supplicant, %s, %s\n", wifi_ssid, wifi_password);

    // connect to hibus server
    fd_socket = hibus_connect_via_unix_socket(SOCKET_PATH, AGENT_NAME, AGENT_RUNNER_NAME, &hibus_context);
    if(fd_socket <= 0)
    {
        printf("hibus: connect to HIBUS server error!\n");
        return NULL;
    }

    endpoint = hibus_assemble_endpoint_name_alloc(HIBUS_LOCALHOST, APP_NAME_SETTINGS, RUNNER_NAME_INETD);
    // in this procedure, device name is ignored.
    hibus_call_procedure(hibus_context, endpoint, METHOD_NET_GET_DEVICES_STATUS, "{\"device\":\"wlp5s0\"}", 1000, wifi_get_devices_info_handler);

    while(1)
    {
        hibus_wait_and_dispatch_packet(hibus_context, 1000);
    }

    hibus_call_procedure(hibus_context, endpoint, METHOD_NET_CLOSE_DEVICE, command_device, 1000, wifi_close_device_handler);
    free(endpoint);
    hibus_disconnect(hibus_context);

    return NULL;
}
