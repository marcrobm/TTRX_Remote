/*
* A Driver to monitor the Display used in the TI30x MathPrint Calculators
* This always uses GPIO's 13,14,15,16,17,18,19,21 for data and 22 as a clock
* (there aren't any sensible alternative pins on the esp32 for this application)
* to use this Driver just create an instance of it and call printFrame
* to print in to std::cout
*/


#ifndef TTRX_T30X_H
#include "../TTRX_Exceptions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <iostream>
#include <atomic>
#include <functional>
namespace TTRX_Remote
{
    class T30X{
        public:
            T30X(void (*onImageReceived)(uint8_t*,uint32_t));
            ~T30X();
            static void printFrame(uint8_t* frame,uint32_t length);
        private:
            typedef struct{
                uint8_t data;
                int64_t rx_time;
            }rdata;
            //this is a queue of queues where each subqueue contains the individual bytes of a frame
            QueueHandle_t gpio_evt_queue = NULL;
            std::atomic<int64_t> last_received_clock = 0;
            void (*onImageReceived)(uint8_t*,uint32_t) = printFrame;
            static void clk_isr_handler(void* arg);
            static void data_parser_task(void* arg);
    };
}
#endif