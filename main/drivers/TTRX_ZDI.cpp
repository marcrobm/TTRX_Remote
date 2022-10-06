#include "TTRX_ZDI.h"
#include "driver/rmt_tx.h"
namespace TTRX_Remote
{
    
    /*
    RMT Interface
        1. 
    */

    ZDI::ZDI(gpio_num_t zcl,gpio_num_t zda){
        std::cout << "Creating RMT TX channel for ZCL" << std::endl;
        rmt_channel_handle_t zcl_chan = NULL;
        rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = zcl,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select a clock that can provide needed resolution
        .resolution_hz = 8000000, //8MHz resolution
        .mem_block_symbols = 64,
        .trans_queue_depth = 10, // set the number of transactions that can be pending in the background
        .flags = {
            .invert_out = 0,   /*!< Whether to invert the RMT channel signal before output to GPIO pad */
            .with_dma = 0,     /*!< If set, the driver will allocate an RMT channel with DMA capability */
            .io_loop_back = 1, /*!< The signal output from the GPIO will be fed to the input path as well */
            .io_od_mode =  1
        }
        };
        TTRX_Exception::ThrowOnEspErr(rmt_new_tx_channel(&tx_chan_config, &zcl_chan));
        std::cout << "Creating encoder for ZCL" << std::endl;
        rmt_encoder_handle_t zcl_encoder = NULL;
        rmt_symbol_word_t high = {
            .duration0=1,
            .level0=0,
            .duration1=0,
            .level1=1
            };
        rmt_symbol_word_t low = {
            .duration0=1,
            .level0=1,
            .duration1=0,
            .level1=1
            };
        rmt_bytes_encoder_config_t enc_config = {
            .bit0 = low,/*!< How to represent BIT0 in RMT symbol */
            .bit1 = high, /*!< How to represent BIT1 in RMT symbol */
           .flags = {
                .msb_first = 1 /*!< Whether to encode MSB bit first */
            }                   /*!< Encoder config flag */
        };
        ESP_ERROR_CHECK(rmt_new_bytes_encoder(&enc_config, &zcl_encoder));
        std::cout << "Creating encoder for ZCL" << std::endl;
    }
    
    ZDI::~ZDI(){

    }
    void ZDI::write(unsigned char addr, unsigned char data){

    }
    unsigned char ZDI::read(unsigned char addr){
     return (unsigned char)0;
    }

}