#ifndef HISTORY_HPP
#define HISTORY_HPP
#include <iostream>
#include <windows.h>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

class History // call olan t�m adlar� dosya adlar� ile birlikte yap.
{

private:
    int id;
    string dialedName;
    string callerName = "ENES G�K";
    string dialedTime;

public:
    History(const std::string &dialedName ,const std::string &dialedTime)
    : id(0), dialedName(dialedName) , dialedTime(dialedTime){}

    History(int _id, const std::string &dialedName,const std::string &dialedTime)
    : id(_id), dialedName(dialedName), dialedTime(dialedTime){}

    int getId() const { return id; }
    
    std::string getdialedName() const { return dialedName; }
    
    std::string getcallerName() const { return callerName; }
    
    std::string getdialedTime() const { return dialedTime; }
    
    void print() const
    {
        std::cout << callerName << " ki�isi , " << dialedTime << " tarihinde " << dialedName << " ki�isini arad�." << std::endl;
    }
};
#endif