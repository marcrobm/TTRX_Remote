#ifndef TTRX_SERVO_H
#define TTRX_SERVO_H
#include "driver/ledc.h"
#include "../TTRX_Exceptions.h"
namespace TTRX_Remote
{
    #define TTRX_Servo_PWM_Frequency 50 //standard rc-servos require 50 Hz
    #define TTRX_Servo_Duty_Resolution LEDC_TIMER_8_BIT
    class Servo{
        public:
            Servo(gpio_num_t gpio,uint32_t value);
            ~Servo();
        private:
            static ledc_timer_t SetupTimer();
            ledc_channel_t pwmChannel;
            gpio_num_t pinNum;
            static ledc_timer_t ServoTimer;
            static ledc_mode_t speedMode;
    };
}
#endif