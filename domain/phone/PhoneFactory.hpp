#ifndef PHONE_FACTORY_HPP
#define PHONE_FACTORY_HPP

#include <windows.h>
#include <string>
#include "Phone.hpp"
using namespace std;

namespace PhoneFactory
{
    // Yeni bir telefon kaydý oluþturur (veri girerken kullanýlacak)
    inline Phone generatePhone(const string &name, const string &sname, const string &pnumber)
    {
        return Phone(name, sname, pnumber);
    }

    // Veritabanýndan gelen kaydý nesne haline getirir (id dahil)
    inline Phone generateFromDb(int id, const string &name, const string &sname, const string &pnumber)
    {
        return Phone(id, name, sname, pnumber);
    }
}

#endif // PHONE_FACTORY_HPP
