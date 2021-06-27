#include <stdio.h>
#include <string>
#include <exception>
class TTRX_Exception : public std::exception
{
private:    
    std::string message = " ";
public:
    TTRX_Exception(std::string msg)
    {
         message = "TTRX_Exception:" + msg;
    }
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};