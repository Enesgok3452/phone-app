#ifndef PHONEREPOSITORY_HPP
#define PHONEREPOSITORY_HPP

#include <windows.h>
#include <sqlext.h>
#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "Phone.hpp"
#include "PhoneFactory.hpp"
#include "../infrastructure/Db.hpp"

namespace PhoneRepository
{
    inline void check(SQLRETURN ret, const char *msg)
    {
        if (!SQL_SUCCEEDED(ret))
        {
            throw std::runtime_error(msg);
        }
    }

    class StatementHelper
    {
    private:
        SQLHDBC conn;
        SQLHSTMT hStmt;
        DbContext &dbContext;

        void check(SQLRETURN ret, const char *msg)
        {
            if (!SQL_SUCCEEDED(ret))
            {
                throw std::runtime_error(msg);
            }
        }

    public:
        StatementHelper(DbContext &context) : dbContext(context)
        {
            conn = dbContext.getConnection();
            check(SQLAllocHandle(SQL_HANDLE_STMT, conn, &hStmt), "Statement handle oluþturulamadý");
        }

        void prepare(const std::string &sql)
        {
            check(SQLPrepareA(hStmt, (SQLCHAR *)sql.c_str(), SQL_NTS), "SQLPrepare baþarýsýz");
        }

        void bind(int paramIndex, const std::string &value)
        {
            check(SQLBindParameter(
                      hStmt, paramIndex, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                      static_cast<SQLULEN>(value.size()), 0, (SQLPOINTER)value.c_str(), 0, NULL),
                  "Parametre baðlama baþarýsýz (std::string)");
        }

        void bind(int paramIndex, int value)
        {
            check(SQLBindParameter(
                      hStmt, paramIndex, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                      0, 0, (SQLPOINTER)&value, 0, NULL),
                  "Parametre baðlama baþarýsýz (int)");
        }

        void execute()
        {
            check(SQLExecute(hStmt), "SQLExecute baþarýsýz");
        }

        SQLHSTMT getStmt() { return hStmt; }

        ~StatementHelper()
        {
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            dbContext.releaseConnection(conn);
        }
    };

    inline std::vector<Phone> fetchAll(SQLHSTMT stmt)
    {
        std::vector<Phone> result;

        int id;
        char name[50] = {0}, surname[50] = {0}, phonenumber[20] = {0};

        void *columnPtrs[] = {&id, name, surname, phonenumber};
        SQLSMALLINT columnTypes[] = {SQL_C_SLONG, SQL_C_CHAR, SQL_C_CHAR, SQL_C_CHAR};
        SQLLEN columnSizes[] = {sizeof(id), sizeof(name), sizeof(surname), sizeof(phonenumber)};

        for (int i = 0; i < 4; ++i)
        {
            SQLBindCol(stmt, i + 1, columnTypes[i], columnPtrs[i], columnSizes[i], nullptr);
        }

        while (SQLFetch(stmt) == SQL_SUCCESS)
        {
            result.emplace_back(PhoneFactory::generateFromDb(id, name, surname, phonenumber));
        }

        return result;
    }

    inline void addPhone(const Phone &p, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("INSERT INTO Directory (name, surname, phonenumber) VALUES (?, ?, ?)");
        stmt.bind(1, p.getName());
        stmt.bind(2, p.getSname());
        stmt.bind(3, p.getPnumber());
        stmt.execute();
    }

    inline void deletePhoneById(int id, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("DELETE FROM Directory WHERE id = ?");
        stmt.bind(1, id);
        stmt.execute();
    }

    inline void updatePhone(const Phone &p, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("UPDATE Directory SET name = ?, surname = ?, phonenumber = ? WHERE id = ?");
        stmt.bind(1, p.getName());
        stmt.bind(2, p.getSname());
        stmt.bind(3, p.getPnumber());
        int id = p.getId().value();
        stmt.bind(4, id);
        stmt.execute();
    }

    inline std::optional<Phone> findPhoneById(int id, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory WHERE id = ?");
        stmt.bind(1, id);
        stmt.execute();

        int fetchedId;
        char name[50] = {0}, surname[50] = {0}, phonenumber[20] = {0};

        void *columnPtrs[] = {&fetchedId, name, surname, phonenumber};
        SQLSMALLINT columnTypes[] = {SQL_C_SLONG, SQL_C_CHAR, SQL_C_CHAR, SQL_C_CHAR};
        SQLLEN columnSizes[] = {sizeof(fetchedId), sizeof(name), sizeof(surname), sizeof(phonenumber)};

        for (int i = 0; i < 4; ++i)
        {
            SQLBindCol(stmt.getStmt(), i + 1, columnTypes[i], columnPtrs[i], columnSizes[i], nullptr);
        }

        if (SQLFetch(stmt.getStmt()) == SQL_SUCCESS)
        {
            return PhoneFactory::generateFromDb(fetchedId, name, surname, phonenumber);
        }
        return std::nullopt;
    }

    inline std::optional<Phone> findPhoneByNumber(const std::string &pnumber, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory WHERE phonenumber = ?");
        stmt.bind(1, pnumber);
        stmt.execute();

        int fetchedId;
        char name[50] = {0}, surname[50] = {0}, phonenumber[20] = {0};

        void *columnPtrs[] = {&fetchedId, name, surname, phonenumber};
        SQLSMALLINT columnTypes[] = {SQL_C_SLONG, SQL_C_CHAR, SQL_C_CHAR, SQL_C_CHAR};
        SQLLEN columnSizes[] = {sizeof(fetchedId), sizeof(name), sizeof(surname), sizeof(phonenumber)};

        for (int i = 0; i < 4; ++i)
        {
            SQLBindCol(stmt.getStmt(), i + 1, columnTypes[i], columnPtrs[i], columnSizes[i], nullptr);
        }

        if (SQLFetch(stmt.getStmt()) == SQL_SUCCESS)
        {
            return PhoneFactory::generateFromDb(fetchedId, name, surname, phonenumber);
        }
        return std::nullopt;
    }

    inline std::vector<Phone> findAllPhones(DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory");
        stmt.execute();

        return fetchAll(stmt.getStmt());
    }

    inline bool isPhoneRegistered(const std::string &phonenumber, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);
        stmt.prepare("SELECT 1 FROM Directory WHERE phonenumber = ?");
        stmt.bind(1, phonenumber);
        stmt.execute();
        return (SQLFetch(stmt.getStmt()) == SQL_SUCCESS);
    }

    inline std::vector<Phone> FilterPhonesByName(const std::string &filterName, DbContext &dbContext)
    {
        StatementHelper stmt(dbContext);

        if (!filterName.empty())
        {
            stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory WHERE LOWER(name) LIKE ? "
                         "ORDER BY name COLLATE Turkish_CI_AS, surname COLLATE Turkish_CI_AS");
            std::string pattern = filterName + "%";
            std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
            stmt.bind(1, pattern);
        }
        else
        {
            stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory "
                         "ORDER BY name COLLATE Turkish_CI_AS, surname COLLATE Turkish_CI_AS");
        }

        stmt.execute();
        return fetchAll(stmt.getStmt());
    }
}

#endif // PHONEREPOSITORY_HPP
