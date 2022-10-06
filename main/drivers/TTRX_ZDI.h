// WIP untested and might be removed in the future, used to interface with the zilog debug protocoll 

#ifndef TTRX_ZDI_H
#define TTRX_ZDI_H
#include "stdio.h"
#include <iostream>

#include "driver/gpio.h"

#include "../TTRX_Exceptions.h"
#include "TTRX_ChannelManager.h"
namespace TTRX_Remote
{
    class ZDI{
        public:
            ZDI(gpio_num_t zcl,gpio_num_t zda);
            ~ZDI();
            void write(unsigned char addr, unsigned char data);
            unsigned char read(unsigned char addr);
        #define TTRX_ZDI_Frequency 50
    #define TTRX_ZCL_BASE_CLOCK_DIVIDER 20
    };
}
#endif
