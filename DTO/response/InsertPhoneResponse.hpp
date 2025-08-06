#ifndef INSERT_RESPONSE_DTO_HPP
#define INSERT_RESPONSE_DTO_HPP

#include <iostream>
#include <string>

class InsertResponse
{

public:
  
    std::string name;
    std::string sname;
    // std::String pnumber; ben telefon numarasýný dýþarýya kullanýcýya göndermek istemiyorum.

    // void print() const
    // {
    //     if (success)
    //     {
    //         std::cout << "Baþarýlý: " << message << std::endl;
    //         std::cout << "Ad: " << name << " Soyad: " << sname << std::endl;
    //     }
    //     else
    //     {
    //         std::cout << "Hata: " << message << std::endl;
    //     }
    // }
};

#endif // PHONE_RESPONSE_HPP