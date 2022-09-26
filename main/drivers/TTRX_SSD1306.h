#ifndef TTRX_SSD1305_H
#include "driver/gpio.h"
namespace TTRX_Remote
{
    class SSD1306{
        public:
            SSD1306(gpio_num_t scl,gpio_num_t sda,char addr);
            ~SSD1306();
    };
}
#endif