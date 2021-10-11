#include "TTRX_Servo.h"
#include "TTRX_ChannelManager.h"
namespace TTRX_Remote
{
Servo::Servo(gpio_num_t gpio,uint32_t value){
    pinNum = gpio;
    pwmChannel = static_cast<ledc_channel_t>((speedMode == LEDC_HIGH_SPEED_MODE)?LedcChannelsHighM.requestChannel():LedcChannelsLowM.requestChannel());
    ledc_channel_config_t configPwmCh = {
            .gpio_num = pinNum,
            .speed_mode = speedMode,
            .channel = pwmChannel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = ServoTimer,
            .duty = value,
            .hpoint = 0,
            };
    TTRX_Exception::ThrowOnEspErr(ledc_channel_config(&configPwmCh));
}
Servo::~Servo(){
    ledc_stop(speedMode,pwmChannel,0);
    gpio_reset_pin(pinNum);
    gpio_set_direction(pinNum,GPIO_MODE_DISABLE);
    if(speedMode == LEDC_HIGH_SPEED_MODE){
        LedcChannelsHighM.freeChannel(static_cast<uint8_t>(pwmChannel));
    }else{
        LedcChannelsLowM.freeChannel(static_cast<uint8_t>(pwmChannel));
    }
}
ledc_timer_t Servo::SetupTimer(){
    ledc_timer_t timer = static_cast<ledc_timer_t>(LedcTimersM.requestChannel());
    ledc_timer_config_t timerConfig;
    timerConfig.timer_num = timer;
    timerConfig.speed_mode = Servo::speedMode;
    timerConfig.duty_resolution = TTRX_Servo_Duty_Resolution;
    timerConfig.freq_hz = TTRX_Servo_PWM_Frequency;
    timerConfig.clk_cfg = LEDC_AUTO_CLK;
    TTRX_Exception::ThrowOnEspErr(ledc_timer_config(&timerConfig));   
    return timer;
}
ledc_mode_t  Servo::speedMode = LEDC_LOW_SPEED_MODE;
ledc_timer_t Servo::ServoTimer = Servo::SetupTimer();
}