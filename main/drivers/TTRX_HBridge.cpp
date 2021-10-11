#include "TTRX_HBridge.h"
#include "../TTRX_Exceptions.h"
#include "driver/gpio.h"
#include <cmath>
namespace TTRX_Remote
{

HBridge::HBridge(gpio_num_t d0,gpio_num_t d1): outputPower(0),movingPositive(false),d0(d0),d1(d1) {
  //setup gpio pins as output
  gpio_set_direction(d0,GPIO_MODE_OUTPUT);
  gpio_set_direction(d1,GPIO_MODE_OUTPUT);
  //setup pwm_channel for HBridge
  pwmChannel = static_cast<ledc_channel_t>((speedMode == LEDC_HIGH_SPEED_MODE)?LedcChannelsHighM.requestChannel():LedcChannelsLowM.requestChannel());
  ledc_channel_config_t configPwmCh = {
  .gpio_num = d0,
  .speed_mode = speedMode,
  .channel = pwmChannel,
  .intr_type = LEDC_INTR_DISABLE,
  .timer_sel = HBridgeTimer,
  .duty = 0,
  .hpoint = 0,
  };
  TTRX_Exception::ThrowOnEspErr(ledc_channel_config(&configPwmCh));
}
HBridge::~HBridge(){
    ledc_stop(speedMode,pwmChannel,0);
    gpio_reset_pin(d0);
    gpio_reset_pin(d1);
    gpio_set_direction(d0,GPIO_MODE_DISABLE);
    gpio_set_direction(d1,GPIO_MODE_DISABLE); 
    if(speedMode == LEDC_HIGH_SPEED_MODE){
        LedcChannelsHighM.freeChannel(static_cast<uint8_t>(pwmChannel));
    }else{
        LedcChannelsLowM.freeChannel(static_cast<uint8_t>(pwmChannel));
    }
}

void HBridge::setPower(float power){
    if(movingPositive != (power >= 0)){
      //changed direction
        movingPositive = (power >= 0);
        ledc_channel_config_t configPwmCh;
        if(power>=0){
           //required to unbind pin from ledc channel
           gpio_reset_pin(d1);
           gpio_set_direction(d1,GPIO_MODE_OUTPUT);
           gpio_set_level(d1,0);
            configPwmCh = {
            .gpio_num = d0,
            .speed_mode = speedMode,
            .channel = pwmChannel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = HBridgeTimer,
            .duty = 0,
            .hpoint = 0,
            };
       }else{
         //required to unbind pin from ledc channel
              gpio_reset_pin(d0);
              gpio_set_direction(d0,GPIO_MODE_OUTPUT);
              gpio_set_level(d0,0);
           configPwmCh = {
            .gpio_num = d1,
            .speed_mode = speedMode,
            .channel = pwmChannel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = HBridgeTimer,
            .duty = 0,
            .hpoint = 0,
            };
       }
       TTRX_Exception::ThrowOnEspErr(ledc_channel_config(&configPwmCh));
    }
    uint32_t duty = static_cast<uint32_t>(std::abs(power)*(pow(2,TTRX_HBridge_Duty_Resolution)-1));
    TTRX_Exception::ThrowOnEspErr(ledc_set_duty(speedMode,pwmChannel,duty));
    TTRX_Exception::ThrowOnEspErr(ledc_update_duty(speedMode,pwmChannel));
}

ledc_timer_t HBridgeSetupTimer(){
    ledc_timer_t timer = static_cast<ledc_timer_t>(LedcTimersM.requestChannel());
    ledc_timer_config_t timerConfig;
    timerConfig.timer_num = timer;
    timerConfig.speed_mode = HBridge::speedMode;
    timerConfig.duty_resolution = TTRX_HBridge_Duty_Resolution;
    timerConfig.freq_hz = TTRX_HBridge_PWM_Frequency;
    timerConfig.clk_cfg = LEDC_AUTO_CLK;
    TTRX_Exception::ThrowOnEspErr(ledc_timer_config(&timerConfig));   
    return timer;
}
ledc_mode_t  HBridge::speedMode = LEDC_HIGH_SPEED_MODE;//has to be initialised before TTRX_HBridge::HBridgeTimer
ledc_timer_t HBridge::HBridgeTimer = HBridgeSetupTimer();
}