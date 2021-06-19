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
void transmitt_rc_channels();
extern "C"{
 void app_main();
}

//TTRX_Transmitter tx;
void app_main(void)
{
    cout << "Started Programm version 0.1 compilled on"<<__DATE__<<" "<<__TIME__<<endl;      
    PPMReader RemoteInput((uint8_t)13,(uint8_t)0);
    //TTRX_Transmitter::init();
    //thread adv_thread(transmitt_rc_channels);
    //adv_thread.join()
    while(true){
    cout<<"Throttle:"<<RemoteInput.readChannel(2)<<endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    }
    cout << "Programm finished\n";
}


void transmitt_rc_channels()
{
    while(true){
        //TTRX_Transmitter::send((uint8_t*)(&t),static_cast<size_t>(4));        
        this_thread::sleep_for(chrono::milliseconds(Constants::advertisement_delay_ms));
        cout << "!";
    }
}