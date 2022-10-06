#include "TTRX_T30X.h"
#include <bitset>
#include <string>
#include <cstring>
#include <iomanip>
#include "soc/gpio_reg.h"
#include "esp_timer.h"

namespace TTRX_Remote
{
    T30X::T30X(void (*onImageReceived)(uint8_t*,uint32_t)){      
        this->onImageReceived = onImageReceived;
        // configure the clock pin
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
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
        gpio_evt_queue = xQueueCreate(3500, sizeof(rdata));
        if(!gpio_evt_queue){
            std::cout << "error creating frame queue:" << this << std::endl;
        }
        xTaskCreate(data_parser_task, "data_parser", 4096, (void*)this, 10, NULL);
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
        uint8_t frame[1600];
        int64_t last_rx_time = 0;
        uint8_t*writepos = frame;
        for(;;) {
            rdata read_byte;
            if(xQueueReceive(t->gpio_evt_queue, (void*)&read_byte, 0) == pdTRUE) {
                //std::cout << std::hex << (int)read_byte.data << " , " << read_byte.rx_time << " ; ";
                if((read_byte.rx_time-last_rx_time) >= 16  || ((int)writepos-(int)frame)>=1536){ //3ms
                    if(t->onImageReceived && ((int)writepos-(int)frame)>=1536){
                        t->onImageReceived(frame,1539);
                    }
                    writepos = frame;
                    *writepos = read_byte.data;
                }else{
                    *writepos = read_byte.data;
                    writepos++;
                }
                last_rx_time = read_byte.rx_time;
            }else{
                vTaskDelay(pdMS_TO_TICKS(75));
            }
        }
    }
    //Function base64_encode taken from https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
    static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
    typedef unsigned char BYTE;
    std::string base64_encode(BYTE const* buf, unsigned int bufLen) {
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }

  return ret;
}
    void T30X::printFrame(uint8_t* frame,uint32_t length){ 
        //std::cout << std::endl << base64_encode(frame, 1540) << std::endl;
        for(int i = 0;i<10;i++) {
            std::cout << (int)frame[i] << " ";
        }
        std::cout << std::endl;
        for(int i = 0;i<10;i++) {
            std::cout << (int)frame[1540-i] <<" ";
        }
        std::cout << std::endl;

            for(int i = 0;i<194;i++){std::cout << "-";} 
            std::cout << std::endl;     
            for(int y = 0;y<64;y++){   
                std::cout << "|";
                    for(int x = 0;x<192;x++){
                        std::cout << ((frame[x*8+((y+16)%64)/8] & (128 >> (((y+16)%64))%8) )?"\u2588":" ");
                        //std::cout << ((frame[x*8+((y+48)%64)/8+8] & (128 >> (((y+48)%64))%8) )?'@':' ');
                        //std::cout << ((frame[x*8+y/8] & (128 >> y%8))?'#':' ');
                    }
                std::cout << "|" << std::endl;
                }
            for(int i = 0;i<194;i++){std::cout << "-";}
            std::cout << std::endl;      
    }
}