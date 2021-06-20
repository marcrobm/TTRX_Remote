#include "TTRX_Constants.h"
namespace TTRX_Remote
{
    uint8_t const Constants::broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t const Constants::receiver_mac[6] = { 0x9C, 0x9C, 0x1F, 0xE2, 0x50, 0x74 };
    //uint8_t const Constants::receiver_mac[6] = { 0x3C, 0x61, 0x05, 0x3E, 0x94, 0x50 };
    int const Constants::advertisement_delay_ms = 2000;
}