#ifndef TTRX_EXCEPTION_H
#define TTRX_EXCEPTION_H
#include <stdio.h>
#include <string>
#include <exception>
#include "esp_err.h"
//Header only
class TTRX_Exception : public std::exception
{
private:
    std::string message = " ";
public:
    TTRX_Exception(std::string msg);
    const char *what() const noexcept override {return message.c_str();}
    static void ThrowOnEspErr(esp_err_t err,std::string msg = "");
};
#endif