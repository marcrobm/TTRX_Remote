#ifndef TTRX_CONSTANTS_H
#define TTRX_CONSTANTS_H
#include <chrono>
namespace TTRX_Remote
{
  class Constants{
    public:
      static int const advertisement_delay_ms;
      static uint8_t const broadcast_mac[6];
      static uint8_t const receiver_mac[6];
  };
}
#endif