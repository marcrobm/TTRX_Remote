#include "TTRX_ChannelManager.h"
#include "../TTRX_Exceptions.h"
#include <numeric>
using namespace std;
namespace TTRX_Remote
{
    ChannelManager::ChannelManager(int maxChannels)
    {
        occupiedChannels = std::vector<bool>(maxChannels);
    }
    uint8_t ChannelManager::requestChannel()
    {
        lock_guard<mutex> lock(rmtChLock);
        for (int i = 0; i < occupiedChannels.size(); i++)
        {
            if (occupiedChannels[i] == false)
            {
                occupiedChannels[i] = true;
                return i;
            }
        }
        throw TTRX_Exception("Could not find any free RMT Channels");
    }
    void ChannelManager::freeChannel(uint8_t channel)
    {
        lock_guard<mutex> lock(rmtChLock);
        if (channel < occupiedChannels.size())
        {
            occupiedChannels[channel] = false;
            return;
        }
        throw TTRX_Exception("Could not free that channel");
    }
    uint8_t ChannelManager::availableChannelCount()
    {
        lock_guard<mutex> lock(rmtChLock);
        uint8_t num = (uint8_t)(occupiedChannels.size() - std::accumulate(occupiedChannels.begin(), occupiedChannels.end(), 0));
        return num;
    }
    ChannelManager RMTChannelsM(RMT_CHANNEL_MAX);
}