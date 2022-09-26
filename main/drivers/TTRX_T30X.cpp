#include "TTRX_T30X.h"
#include <bitset>
#include <string>
#include <cstring>
#include "soc/gpio_reg.h"
#include "esp_timer.h"

namespace TTRX_Remote
{
    T30X::T30X(){      
        // configure the clock pin
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_POSEDGE;
        // attach an interrupt on the clock pin
        io_conf.pin_bit_mask = (1ULL<<22); // GPIO 22 as a clk
        // disable pull down
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        // disable pull up
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        // set clk as input
        io_conf.mode = GPIO_MODE_INPUT;
        TTRX_Exception::ThrowOnEspErr(gpio_config(&io_conf));
        // set the data pins as input
        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_13,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_en(GPIO_NUM_13));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_13));

        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_14,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_en(GPIO_NUM_14));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_14));

        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_15,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_dis(GPIO_NUM_15));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_15));
        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_16,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_dis(GPIO_NUM_16));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_16));
        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_17,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_dis(GPIO_NUM_17));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_17));
        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_18,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_dis(GPIO_NUM_18));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_18));
        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_19,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_dis(GPIO_NUM_19));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_19));
        TTRX_Exception::ThrowOnEspErr(gpio_set_direction(GPIO_NUM_21,GPIO_MODE_INPUT));
        TTRX_Exception::ThrowOnEspErr(gpio_pulldown_dis(GPIO_NUM_21));
        TTRX_Exception::ThrowOnEspErr(gpio_pullup_dis(GPIO_NUM_21));

        // Setup queue for captured frames
        gpio_evt_queue = xQueueCreate(5000, sizeof(rdata));
        if(!gpio_evt_queue){
            std::cout << "error creating frame queue:" << this << std::endl;
        }
        xTaskCreate(data_parser_task, "data_parser_task", 4096, (void*)this, 10, NULL);
        // install gpio isr service
        TTRX_Exception::ThrowOnEspErr(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3));
        // hook isr handler for clock pin and supply this object
        TTRX_Exception::ThrowOnEspErr(gpio_isr_handler_add(GPIO_NUM_22, clk_isr_handler, (void*)this));
    }
    T30X::~T30X(){
        gpio_isr_handler_remove(GPIO_NUM_22);
        vQueueDelete(gpio_evt_queue);
    }

    void IRAM_ATTR T30X::clk_isr_handler(void* arg)
    {
        T30X* t = (T30X*)arg;
        uint32_t gpio_values = REG_READ(GPIO_IN_REG);      
        // first we read D0-D6 (pins 13-19) and save them into the 6 least siginificant bits
        uint8_t data_byte = (gpio_values >> 13);               
        // then we add the gpio 21 value in the most significant bit
        data_byte = data_byte | (((gpio_values >> 21) & 0b1)<<7);
        
        rdata val;
        val.data = data_byte;
        val.rx_time = esp_timer_get_time();
        xQueueSendFromISR(t->gpio_evt_queue, &val, NULL);
    }

    void T30X::data_parser_task(void* arg)
    {
        T30X* t = (T30X*)arg;
        uint8_t frame[1540];
        int64_t last_rx_time = 0;
        for(;;) {
            memset((void*)frame,0,1540);
            uint8_t*writepos = frame;
            
            if(uxQueueMessagesWaiting(t->gpio_evt_queue) > 1539){
                // read out a frame
                while(writepos<frame+1540) {
                    rdata read_byte;
                    if(xQueueReceive(t->gpio_evt_queue, (void*)&read_byte, 0) == pdTRUE) {
                        //writepos = ((read_byte.rx_time-last_rx_time) >= 10000)?frame:writepos+1;
                        //std::cout << "diffTime:" << (read_byte.rx_time-last_rx_time) << std::endl;
                        if((read_byte.rx_time-last_rx_time) >= 30000){ //30ms
                            std::cout << "last image length:" << ((int)writepos-(int)frame) << std::endl;
                            std::cout << "received a new image:" << read_byte.rx_time-last_rx_time << std::endl;
                            writepos = frame;
                            *writepos = read_byte.data;
                            last_rx_time = read_byte.rx_time;
                            break;
                        }else{
                            *writepos = read_byte.data;
                            writepos++;
                        }
                        last_rx_time = read_byte.rx_time;
                    }
                }

                // Display a frame
                for(int y = 0;y<64;y++){
                    for(int x = 0;x<192;x++){
                        std::cout << ((frame[x*8+(64-y)/8] & (128 >> (64-y)%8) )?'#':' ');
                    }
                    std::cout << std::endl;
                }

            }
            vTaskDelay(pdMS_TO_TICKS(15));
        }
    }
 
}