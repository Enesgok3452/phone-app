    #ifndef HISTORY_FACTORY_HPP
    #define HISTORY_FACTORY_HPP

    #include <string>
    #include "History.hpp"
    #include <ctime>
    #include <iomanip>
    #include <sstream>

    namespace HistoryFactory
    {
        // Yeni çaðrý oluþtur, id otomatik 0 olur
        inline History generateHistory(const std::string &dialedName,const std::string &dialedTime)
        {
            return History(dialedName , dialedTime);
        }

        // Veritabanýndan gelen çaðrý için id ve isim ile nesne oluþtur
        inline History generateFromDb(int id, const string &dialedName, const std::string &dialedTime)
        {
            return History(id, dialedName, dialedTime);
        }
    }

    #endif
