#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

//#include "drivers/TTRX_Transmitter.h"
#include "TTRX_Constants.h"
//#include "drivers/TTRX_TxInputReader.h"
//#include "drivers/TTRX_HBridge.h"
#include "drivers/TTRX_ZDI.h"
#include "drivers/TTRX_T30X.h"
using namespace TTRX_Remote;

extern "C"
{
    void app_main();
}
/*
void updateMotors();
HBridge mainMotor(GPIO_NUM_32,GPIO_NUM_33);
*/

// stolen from https://github.com/Jeija/esp32-80211-tx/blob/master/main/main.c
/*
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
uint8_t beacon_raw[] = {
	0x80, 0x00,							                    // 0-1: Frame Control
	0x00, 0x00,						                      	// 2-3: Duration
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,				        // 4-9: Destination address (broadcast)
	0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,				        // 10-15: Source address
	0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,				        // 16-21: BSSID
	0x00, 0x00,							                    // 22-23: Sequence / fragment number
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,			// 24-31: Timestamp (GETS OVERWRITTEN TO 0 BY HARDWARE)
	0x64, 0x00,							                    // 32-33: Beacon interval
	0x31, 0x04,						                       	// 34-35: Capability info
	0x00, 0x00,                                             // FILL CONTENT HERE 				// 36-38: SSID parameter set, 0x00:length:content
	0x01, 0x08, 0x82, 0x84,	0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,	// 39-48: Supported rates
	0x03, 0x01, 0x01,						                // 49-51: DS Parameter set, current channel 1 (= 0x01),
	0x05, 0x04, 0x01, 0x02, 0x00, 0x00,				        // 52-57: Traffic Indication Map
	
};
#define BEACON_SSID_OFFSET 38
#define SRCADDR_OFFSET 10
#define BSSID_OFFSET 16
#define SEQNUM_OFFSET 22

char* ssid_to_broadcast=(char*)"TraclerFakeAP";

esp_err_t event_handler(void *ctx, system_event_t *event) {
	return ESP_OK;
}

void beacon_task(void *pvParameter) {
	uint16_t seqnum=0;
	for (;;) {
		vTaskDelay(500 / portTICK_PERIOD_MS);
		printf("%i %i %s\r\n", strlen(ssid_to_broadcast),1, ssid_to_broadcast);
		uint8_t beacon_data[200];
		memcpy(beacon_data, beacon_raw, BEACON_SSID_OFFSET - 1);
		beacon_data[BEACON_SSID_OFFSET - 1] = strlen(ssid_to_broadcast);
		memcpy(&beacon_data[BEACON_SSID_OFFSET], ssid_to_broadcast, strlen(ssid_to_broadcast));
		memcpy(&beacon_data[BEACON_SSID_OFFSET + strlen(ssid_to_broadcast)], &beacon_raw[BEACON_SSID_OFFSET], sizeof(beacon_raw) - BEACON_SSID_OFFSET);
		// Update sequence number
		beacon_data[SEQNUM_OFFSET] = (seqnum & 0x0f) << 4;
		beacon_data[SEQNUM_OFFSET + 1] = (seqnum & 0xff0) >> 4;
		seqnum++;
		if (seqnum > 0xfff)
			seqnum = 0;

		esp_wifi_80211_tx(WIFI_IF_AP, beacon_data, sizeof(beacon_raw) + strlen(ssid_to_broadcast), false);
	}
}
*/
void app_main(void)
{
    cout << "Started Programm version 0.1 compilled on " << __DATE__ << " " << __TIME__ << endl;
    auto interface = T30X(T30X::printFrame);
	vTaskDelay(pdMS_TO_TICKS(150000000));
	cout << "Programm finished\n";

	/*
	nvs_flash_init();
	tcpip_adapter_init();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	// Init dummy AP to specify a channel and get WiFi hardware into
	// a mode where we can send the actual fake beacon frames.
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	uint8_t password[64] = "abcdeftre";
	wifi_ap_config_t ap_config = {
			.ssid_len = 0,
			.channel = 1,
			.authmode = WIFI_AUTH_WPA2_PSK,
			.ssid_hidden = 1,
			.max_connection = 1,
			.beacon_interval = 60000,
            .pairwise_cipher = WIFI_CIPHER_TYPE_TKIP,
            .ftm_responder=false
	};
	memcpy(ap_config.password,password,9);
	memcpy(ap_config.ssid,password,9);
    wifi_config_t ap_config_wrap = {.ap=ap_config};
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config_wrap));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
	xTaskCreate(&beacon_task, "beacon_task", 2048, NULL, 5, NULL);
	*/
}






/*
    thread adv_thread(updateMotors);
    adv_thread.join();
    cout << "Programm finished\n";
    */
/*
void updateMotors(){
    while(true){
    for(float i = 0 ;i<1;i+=0.01){
        this_thread::sleep_for(chrono::milliseconds(30));
        mainMotor.setPower(i);
    }
    mainMotor.setPower(0);
    this_thread::sleep_for(chrono::milliseconds(1000));
        for(float i = 1 ;i> -1 ;i-=0.01){
            this_thread::sleep_for(chrono::milliseconds(30));
            mainMotor.setPower(i);
        }
    }
}*/

/*
void transmittRcChannels();
PPMReader RemoteInput((uint8_t)13);
void app_main(void)
{
    cout << "Started Programm version 0.1 compilled on " << __DATE__ << " " << __TIME__ << endl;
    TTRX_Transmitter::init();
    thread adv_thread(transmittRcChannels);
    adv_thread.join();
    cout << "Programm finished\n";
}

void transmittRcChannels()
{
    uint16_t *buf = (uint16_t *)calloc(8, sizeof(uint16_t));
    while (true)
    {
        for (int i = 0; i < 8; i++)
        {
            buf[i] = RemoteInput[i];
        }
        TTRX_Transmitter::send((uint8_t *)(buf), static_cast<size_t>(8));
        this_thread::sleep_for(chrono::milliseconds(Constants::broadcast_frequency_ms));
    }
    free(buf);
}
*/