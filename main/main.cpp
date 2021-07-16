#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

#include "drivers/TTRX_Transmitter.h"
#include "TTRX_Constants.h"
#include "drivers/TTRX_TxInputReader.h"
using namespace TTRX_Remote;

//own functions
void transmittRcChannels();
extern "C"
{
    void app_main();
}

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
        TTRX_Transmitter::send((uint8_t *)(&buf), static_cast<size_t>(8));
        this_thread::sleep_for(chrono::milliseconds(Constants::broadcast_frequency_ms));
        //cout << "Channel3:"<<RemoteInput[2]<<endl;
        //cout << "!";
    }
    free(buf);
}