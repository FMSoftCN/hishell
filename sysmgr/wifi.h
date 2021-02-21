#ifndef __WIFI__H__
#define __WIFI__H__

// method for All Network Devices 
#define METHOD_NET_OPEN_DEVICE          "openDevice"
#define METHOD_NET_CLOSE_DEVICE         "closeDevice"
#define METHOD_NET_GET_DEVICES_STATUS   "getNetworkDevicesStatus"
// method for WiFi Device
#define METHOD_WIFI_START_SCAN          "wifiStartScanHotspots"
#define METHOD_WIFI_STOP_SCAN           "wifiStopScanHotspots"
#define METHOD_WIFI_CONNECT_AP          "wifiConnect"
#define METHOD_WIFI_DISCONNECT_AP       "wifiDisconnect"
#define METHOD_WIFI_GET_NETWORK_INFO    "wifiGetNetworkInfo"
// method for Ethernet Device
// method for Mobile Device


// event for All Network Devices
#define NETWORKDEVICECHANGED            "NETWORKDEVICECHANGED"
// event for WiFi Device
#define WIFIHOTSPOTSCHANGED             "WIFIHOTSPOTSCHANGED"
#define WIFISIGNALSTRENGTHCHANGED       "WIFISIGNALSTRENGTHCHANGED"
// event for Ethernet Device
// event for Mobile Device

// parameter for inetd runner
#define APP_NAME_SETTINGS               "cn.fmsoft.hybridos.settings"
#define RUNNER_NAME_INETD               "inetd"
#define SOCKET_PATH                     "/var/tmp/hibus.sock"
#define MAX_DEVICE_NUM                  10          // maximize of network devices is 10
#define DEFAULT_SCAN_TIME               30          // for WiFi scan  period

// device type
#define DEVICE_TYPE_UNKONWN             0
#define DEVICE_TYPE_ETHERNET            1
#define DEVICE_TYPE_WIFI                2
#define DEVICE_TYPE_MOBILE              3
#define DEVICE_TYPE_LO                  4
#define DEVICE_TYPE_DEFAULT             DEVICE_TYPE_ETHERNET

// device status
#define DEVICE_STATUS_UNCERTAIN         0           // uncertain
#define DEVICE_STATUS_DOWN              1           // device is unactive
#define DEVICE_STATUS_UP                2           // device is active, but perhaps do not connect to any network
#define DEVICE_STATUS_RUNNING           3           // device is active and has ip address

// for error
#define ERR_NO                          0
#define ERR_LIBRARY_OPERATION           -1
#define ERR_NONE_DEVICE_LIST            -2
#define ERR_WRONG_PROCEDURE             -3
#define ERR_WRONG_JSON                  -4
#define ERR_NO_DEVICE_NAME_IN_PARAM     -5
#define ERR_NO_DEVICE_IN_SYSTEM         -6
#define ERR_DEVICE_TYPE                 -7
#define ERR_LOAD_LIBRARY                -8
#define ERR_NOT_WIFI_DEVICE             -9
#define ERR_DEVICE_NOT_OPENNED          -10
#define ERR_OPEN_WIFI_DEVICE            -11
#define ERR_CLOSE_WIFI_DEVICE           -12
#define ERR_OPEN_ETHERNET_DEVICE        -13
#define ERR_CLOSE_ETHERNET_DEVICE       -14
#define ERR_OPEN_MOBILE_DEVICE          -15
#define ERR_CLOSE_MOBILE_DEVICE         -16
#define ERR_DEVICE_NOT_CONNECT          -17

// for network changed
#define NETWORK_CHANGED_NAME            ((0x01) << 0)
#define NETWORK_CHANGED_TYPE            ((0x01) << 1)
#define NETWORK_CHANGED_STATUS          ((0x01) << 2)
#define NETWORK_CHANGED_MAC             ((0x01) << 3)
#define NETWORK_CHANGED_IP              ((0x01) << 4)
#define NETWORK_CHANGED_BROADCAST       ((0x01) << 5)
#define NETWORK_CHANGED_SUBNETMASK      ((0x01) << 6)

// for string length
#define HOTSPOT_STRING_LENGTH           64 
#define NETWORK_DEVICE_NAME_LENGTH      32
#define NETWORK_ADDRESS_LENGTH          32

// WiFi AP description
typedef struct _wifi_hotspot                    // the information for one AP
{
    char bssid[HOTSPOT_STRING_LENGTH];          // bssid
    unsigned char ssid[HOTSPOT_STRING_LENGTH];  // ssid
    char frenquency[HOTSPOT_STRING_LENGTH];     // frequency
    char capabilities[HOTSPOT_STRING_LENGTH];   // encrypt type
    int  signal_strength;                       // signal strength
    bool isConnect;                             // whether connected
    struct _wifi_hotspot * next;                // the next node in list
} wifi_hotspot;

#define AGENT_NAME          "cn.fmsoft.hybridos.sysmgr"
#define AGENT_RUNNER_NAME   "gui"

#endif  // __WIFI__H__
