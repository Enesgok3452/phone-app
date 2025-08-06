#ifndef HISTORY_SERVICE_HPP
#define HISTORY_SERVICE_HPP

#include "History.hpp"
#include "HistoryRepository.hpp"
#include "HistoryFactory.hpp"
#include "../infrastructure/Db.hpp"
#include <vector>
#include <optional>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace HistoryService
{
    inline std::string getCurrentDateTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline void add(const std::string &dialedName, DbContext &dbContext)
    {
        std::string now = getCurrentDateTime();
        History history = HistoryFactory::generateHistory(dialedName, now);
        HistoryRepository::Add(history, dbContext);
    }

    inline std::vector<History> GetAll(DbContext &dbContext)
    {
        return HistoryRepository::FindAll(dbContext);
    }

    inline std::vector<History> GetAll(DbContext &dbContext, const std::string &filterName)
    {
        auto allHistories = HistoryRepository::FindAll(dbContext);

        if (filterName.empty())
            return allHistories;

        std::vector<History> filteredHistories;
        for (const auto &h : allHistories)
        {
            if (h.getcallerName().find(filterName) != std::string::npos ||
                h.getdialedName().find(filterName) != std::string::npos)
            {
                filteredHistories.push_back(h);
            }
        }
        return filteredHistories;
    }
}

#endif
