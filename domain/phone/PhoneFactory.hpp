#ifndef PHONE_FACTORY_HPP
#define PHONE_FACTORY_HPP

#include <windows.h>
#include <string>
#include "Phone.hpp"
using namespace std;

namespace PhoneFactory
{
    // Yeni bir telefon kayd� olu�turur (veri girerken kullan�lacak)
    inline Phone generatePhone(const string &name, const string &sname, const string &pnumber)
    {
        return Phone(name, sname, pnumber);
    }

    // Veritaban�ndan gelen kayd� nesne haline getirir (id dahil)
    inline Phone generateFromDb(int id, const string &name, const string &sname, const string &pnumber)
    {
        return Phone(id, name, sname, pnumber);
    }
}

#endif // PHONE_FACTORY_HPP
