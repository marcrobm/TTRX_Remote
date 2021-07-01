#include "TTRX_TxInputReader.h"
#include "../TTRX_Exceptions.h"
#include <numeric>
using namespace TTRX_Remote;
using namespace std;
PPMReader::PPMReader(uint8_t gpio)
{
    rmt_config_t rmt_rx;
    rmt_rx.channel = (rmt_channel_t)RMT_Wrapper::requestChannel();
    rmt_rx.gpio_num = (gpio_num_t)gpio;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = PPM_TIMEOUT_US * (RMT_TICK_US);
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
    //actually required, does not work with c++ threads, reason for wrapper taskreadChannels
    xTaskCreate(&taskreadChannels, "ppmtask", 2048, this, 10, NULL);
    std::cout << "PPM init done\n";
}
void TTRX_Remote::taskreadChannels(void *param)
{
    static_cast<PPMReader *>(param)->readChannels();
}
void PPMReader::readChannels()
{
    std::cout << "starting to read channels" << std::endl;
    rmt_channel_t channel = RMT_CHANNEL_0;
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(channel, &rb);
    rmt_rx_start(channel, true);
    while (rb)
    {
        size_t length = 0;
        int ppmchannelcount;
        rmt_item32_t *item = (rmt_item32_t *)xRingbufferReceive(rb, &length, 1000);
        if (item)
        {
            rc_vector_lock.lock();
            ppmchannelcount = (length / 4) - 1;
            channels.resize(ppmchannelcount);
            //printf("PPM RX %d (%d) channels: ", ppmchannelcount, length);
            for (int i = 0; i < ppmchannelcount; i++)
            {
                channels[i] = (item[i].duration1 + item[i].duration0) / RMT_TICK_US;
            }
            rc_vector_lock.unlock();
            vRingbufferReturnItem(rb, (void *)item);
        }
        else
        {
            break;
        }
    }
}
uint16_t PPMReader::readChannel(uint8_t num)
{
    rc_vector_lock.lock();
    uint16_t val = 0;
    if (num < channels.size())
    {
        val = channels[num];
    }
    rc_vector_lock.unlock();
    return val;
}
uint16_t PPMReader::operator[](int i){
    return readChannel((uint8_t)i);
}

//RMT_Wrapper
bool RMT_Wrapper::occupiedChannels[RMT_CHANNEL_MAX] = {0};
std::mutex RMT_Wrapper::rmtChLock;
uint8_t RMT_Wrapper::requestChannel()
{
    lock_guard<mutex> lock(rmtChLock);
    for (int i = 0; i < RMT_CHANNEL_MAX; i++)
    {
        if (occupiedChannels[i] == false)
        {
            occupiedChannels[i]=true;
            return i;
        }
    }
    throw TTRX_Exception("Could not find any free RMT Channels");
}
void RMT_Wrapper::freeChannel(uint8_t channel){
    lock_guard<mutex> lock(rmtChLock);
    if(channel<RMT_CHANNEL_MAX){
        occupiedChannels[channel]=false;
        return;
    }
    throw TTRX_Exception("Could not free that channel");
}
uint8_t RMT_Wrapper::availableChannelCount(){
    lock_guard<mutex> lock(rmtChLock);
    uint8_t num = (uint8_t)(RMT_CHANNEL_MAX-std::accumulate(occupiedChannels,occupiedChannels + RMT_CHANNEL_MAX, 0));
    return num;
}