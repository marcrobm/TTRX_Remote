#ifndef TTRX_HBridge_H
#define TTRX_HBridge_H
#include "driver/ledc.h"
#include "TTRX_ChannelManager.h"
namespace TTRX_Remote
{
    #define TTRX_HBridge_PWM_Frequency 1000 //in Hz
    #define TTRX_HBridge_Duty_Resolution LEDC_TIMER_8_BIT
    //should be threadsave
    class HBridge{
        private:
            float outputPower;
            bool movingPositive;
            ledc_channel_t pwmChannel;
            gpio_num_t d0;
            gpio_num_t d1;
        public:
            HBridge(gpio_num_t d0,gpio_num_t d1);
            ~HBridge();
            void setPower(float power);
            static ledc_timer_t HBridgeTimer;
            static ledc_mode_t speedMode;
    };    
    ledc_timer_t HBridgeSetupTimer();
}
#endif