#ifndef PHONE_UPDATE_RESPONSE_DTO_HPP
#define PHONE_UPDATE_RESPONSE_DTO_HPP

#include <iostream>
#include <string>

class UpdateResponse
{

public:
    bool success;
    std::string name;
    std::string sname;
    // std::String pnumber; ben telefon numaras�n� d��ar�ya kullan�c�ya g�ndermek istemiyorum.

    // void print() const
    // {
    //     if (success)
    //     {
    //         std::cout << "Ba�ar�l�: " << message << std::endl;
    //         std::cout << "Ad: " << name <<
    //         endl << "Soyad: " << sname << std::endl;
    //     }
    //     else
    //     {
    //         std::cout << "Hata: " << message << std::endl;
    //     }
    // }
};

#endif // PHONE_RESPONSE_HPP