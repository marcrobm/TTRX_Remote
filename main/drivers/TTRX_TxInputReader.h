#ifndef TTRX_TXINPUTREADER_H
#define TTRX_TXINPUTREADER_H

#include <vector>
#include <inttypes.h>
#include "driver/rmt.h"
#include "stdio.h"
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <chrono>
#include <thread>
#include <mutex>

#define RMT_CLK_DIV      10    /*!< RMT counter clock divider */
#define RMT_TICK_US    (80000000/RMT_CLK_DIV/1000000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */
#define PPM_TIMEOUT_US  3500   /*!< RMT receiver timeout value(us) */


namespace TTRX_Remote{ 
    class RMT_Wrapper{
            std::vector<int8_t> occupiedChannels;
        public:
            int requestChannel();
            void freeChannel(int8_t channel);
    };

    class PPMReader{
        private:
            rmt_channel_t rmt_channel;
            std::mutex rc_vector_lock;
            std::vector<uint16_t> channels ={0,0,0,0,0,0};
            void readChannels();
        public:            
            PPMReader(uint8_t gpio,uint8_t channel);
            uint16_t readChannel(uint8_t num);     
            friend void taskreadChannels(void* param);
    };
    void taskreadChannels(void* param);
}
#endif

