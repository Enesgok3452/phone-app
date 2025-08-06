#ifndef PHONE_UPDATE_REQUEST_DTO_HPP
#define PHONE_UPDATE_REQUEST_DTO_HPP

#include <iostream>
#include <string>

class UpdateRequest
{

public:
    int id;              // id
    std::string name;    // ad
    std::string sname;   // soyad
    std::string pnumber; // telefon no
};

#endif // PHONE_UPDATE_REQUEST_DTO