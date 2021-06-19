#include "TTRX_Transmitter.h"
using namespace TTRX_Remote;
#include <stdio.h>
#include <iostream>
#include "nvs_flash.h"
using std::cout;

#include "../TTRX_Constants.h"
bool TTRX_Transmitter::send(const uint8_t* data,size_t length){
    esp_err_t error = esp_now_send(Constants::receiver_mac,data,length);
    return (error==ESP_OK);
}
template <class T>
bool TTRX_Transmitter::sendstruct(T data){
    esp_err_t error = esp_now_send(Constants::receiver_mac,(uint8_t*)&data,sizeof(data));
    return (error==ESP_OK);
}

void TTRX_Transmitter::init_wifi()
{
    //init wifi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());
}
void TTRX_Transmitter::init_espnow()
{
    //* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK( esp_now_init() );
    ESP_ERROR_CHECK( esp_now_register_send_cb(espnow_send_cb) );
    ESP_ERROR_CHECK( esp_now_register_recv_cb(espnow_recv_cb) );
    /* Set primary master key. */
    //ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );
    esp_now_peer_info_t * other_device = (esp_now_peer_info_t *)calloc(1,sizeof(esp_now_peer_info_t));
    memcpy(other_device->peer_addr,&Constants::receiver_mac,6);//Todo
    other_device->channel = 0;//use default wifi channel
    other_device->encrypt = false;
    cout <<"addpeerresult:"<<esp_now_add_peer(other_device)<<"\n";
    free(other_device);
}
void TTRX_Transmitter::init()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    init_wifi();
    init_espnow();
}
void TTRX_Transmitter::espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len){
    cout<<"received "<<len<<" bytes of data\n";
}  
void TTRX_Transmitter::espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    cout<<"Just sent some data\n";
}