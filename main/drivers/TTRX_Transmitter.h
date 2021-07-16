#ifndef ESPNOWWRAPPER_H
#define ESPNOWWRAPPER_H
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_crc.h"
#define ESPNOW_QUEUE_SIZE 6

namespace TTRX_Remote
{
    class TTRX_Transmitter
    {
    public:
        static void init();
        static bool send(const uint8_t *data, size_t length);
        template <class T>
        bool sendstruct(T data);

    private:
        static void init_wifi();
        static void init_espnow();
        static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len);
        static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
    };
}
#endif