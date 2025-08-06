#ifndef HISTORY_RESPONSE_HPP
#define HISTORY_RESPONSE_HPP


#include <iostream>
#include <string>
#include "../domain/History/History.hpp"
#include <vector>
class HistoryResponse 
{
public:
    struct HistoryItem
    {
        std::string dialedName; 
        std::string dialedTime;
        std::string callerName = "ENES GÖK";
    };

    bool success;
    std::vector<HistoryItem> histories;



};
#endif