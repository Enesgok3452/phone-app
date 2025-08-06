#ifndef PHONE_HPP
#define PHONE_HPP
//#include "../crow/include/crow_all.h"
#include <windows.h>
#include <optional>
#include <iostream>
using namespace std;

class Phone
{
private:
    std::optional<int> id;
    string name;
    string sname;
    string pnumber;

public:
    void print() const
    {
        if (id.has_value())
            std::cout << "ID: " << id.value() << " |";

        std::cout << "Name: " << name << "|Surname: " << sname << "|Phone: " << pnumber << std::endl;
    }

    Phone(const string &_name, const string &_sname, const string &_pnumber)
        : id(std::nullopt), name(_name), sname(_sname), pnumber(_pnumber) {}

    Phone(const std::optional<int> &_id, const string &_name, const string &_sname, const string &_pnumber)
        : id(_id), name(_name), sname(_sname), pnumber(_pnumber) {}

    optional<int> getId() const { return id; }

    string getName() const { return name; }

    string getSname() const { return sname; }

    string getPnumber() const { return pnumber; }

    void setId(int newId) { id = newId; }
};

#endif
