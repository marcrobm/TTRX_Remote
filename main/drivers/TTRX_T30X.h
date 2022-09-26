#ifndef TTRX_T30X_H
#include "../TTRX_Exceptions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <iostream>
#include <atomic>
namespace TTRX_Remote
{
    /* This always uses GPIO's 13,14,15,16,17,18,19,21 for data and 22 as a clock
    * (there aren't any sensible alternative pins on the esp32 for this application)
    */
    class T30X{
        public:
            T30X();
            ~T30X();
        private:
            typedef struct{
                uint8_t data;
                int64_t rx_time;
            }rdata;
            //this is a queue of queues where each subqueue contains the individual bytes of a frame
            QueueHandle_t gpio_evt_queue = NULL;
            std::atomic<int64_t> last_received_clock = 0;
            static void clk_isr_handler(void* arg);
            static void data_parser_task(void* arg);
    };
}
#endif