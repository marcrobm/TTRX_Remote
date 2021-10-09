#include "TTRX_Exceptions.h"
TTRX_Exception::TTRX_Exception(std::string msg)
{
    message = "TTRX_Exception:" + msg;
}
void TTRX_Exception::ThrowOnEspErr(esp_err_t err,std::string msg){
    if(err != ESP_OK){
        throw TTRX_Exception(msg+" esp_err_t:"+std::string(esp_err_to_name(err)));
    }
}