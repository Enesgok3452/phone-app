#ifndef LIST_RESPONSE_DTO_HPP
#define LIST_RESPONSE_DTO_HPP

#include <string>
#include <vector>
#include <optional>

// class ListItem      // bunu struct olarak getphonelist e yap
// {
// public:
//     int id;
//     std::string name;
//     std::string sname;
//     std::string pnumber;
// };

class GetPhoneListResponse
{

public:

    struct PhoneListItem
    {
        int id;
        std::string name;
        std::string sname;
        std::string pnumber; 
    };

    bool success;   
    std::vector<PhoneListItem> phones;
    
};

#endif
