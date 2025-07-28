#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "upgrade.h"

static struct espconn ota_conn;
static esp_tcp ota_tcp;
static uint32 fw_size = 0;
static uint32 received = 0;

void ota_recv_cb(void *arg, char *pdata, unsigned short len) {
    if (fw_size == 0 && len >= 4) {
        fw_size = *((uint32*)pdata);  // primi 4 byte = size firmware
        pdata += 4;
        len -= 4;
        system_upgrade_init();
    }
    if (len > 0) {
        system_upgrade(pdata, len);
        received += len;
        if (received >= fw_size) {
            system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
            system_upgrade_reboot();
        }
    }
}

void ota_listen() {
    ota_conn.type = ESPCONN_TCP;
    ota_conn.state = ESPCONN_NONE;
    ota_tcp.local_port = 8266;
    ota_conn.proto.tcp = &ota_tcp;
    espconn_regist_recvcb(&ota_conn, ota_recv_cb);
    espconn_accept(&ota_conn);
}

void wifi_event_cb(System_Event_t *evt) {
    if (evt->event == EVENT_STAMODE_GOT_IP) ota_listen();
}

void user_init() {
    struct station_config conf;
    wifi_set_opmode(STATION_MODE);
    os_memset(&conf, 0, sizeof(conf));
    os_strcpy(conf.ssid, "SSID");
    os_strcpy(conf.password, "PASS");
    wifi_station_set_config(&conf);
    wifi_set_event_handler_cb(wifi_event_cb);
}

