    #ifndef HISTORY_FACTORY_HPP
    #define HISTORY_FACTORY_HPP

    #include <string>
    #include "History.hpp"
    #include <ctime>
    #include <iomanip>
    #include <sstream>

    namespace HistoryFactory
    {
        // Yeni �a�r� olu�tur, id otomatik 0 olur
        inline History generateHistory(const std::string &dialedName,const std::string &dialedTime)
        {
            return History(dialedName , dialedTime);
        }

        // Veritaban�ndan gelen �a�r� i�in id ve isim ile nesne olu�tur
        inline History generateFromDb(int id, const string &dialedName, const std::string &dialedTime)
        {
            return History(id, dialedName, dialedTime);
        }
    }

    #endif
