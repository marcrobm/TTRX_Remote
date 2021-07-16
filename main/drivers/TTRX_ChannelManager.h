#ifndef TTRX_CHANNELMANAGER_H
#define TTRX_CHANNELMANAGER_H
#include <vector>
#include <mutex>
#include <inttypes.h>
//only using RMT_CHANNEL_MAX
#include "driver/rmt.h"

namespace TTRX_Remote
{
     class ChannelManager
     {
     private:
          std::mutex rmtChLock;

     public:
          ChannelManager(int maxChannels);
          std::vector<bool> occupiedChannels;
          uint8_t requestChannel();
          void freeChannel(uint8_t channel);
          uint8_t availableChannelCount();
     };
     extern ChannelManager RMTChannelsM;
}
#endif