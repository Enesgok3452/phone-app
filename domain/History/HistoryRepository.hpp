#ifndef HISTORY_REPOSITORY_HPP
#define HISTORY_REPOSITORY_HPP

#include <vector>
#include <optional>
#include <string>
#include "../infrastructure/Db.hpp"
#include "History.hpp"
#include "HistoryFactory.hpp"
#include <iostream>

namespace HistoryRepository
{

    // inline void CreateTable(DbContext &dbContext)
    // {
    //     SQLHDBC conn = dbContext.getConnection();
    //     SQLHSTMT hStmt;
    //     SQLAllocHandle(SQL_HANDLE_STMT, conn, &hStmt);

    //     std::string createTableQuery =
    //         "IF OBJECT_ID('Call_History', 'U') IS NULL "
    //         "BEGIN "
    //         "CREATE TABLE Call_History ("
    //         "id INT PRIMARY KEY IDENTITY(1,1), "
    //         "dialedName VARCHAR(50) ,"
    //         "dialedTime TEXT"
    //         ") "
    //         "END";

    //     SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR *)createTableQuery.c_str(), SQL_NTS);
    //     if (!SQL_SUCCEEDED(ret))
    //     {
    //         std::cerr << "createTable (Call_History) failed" << std::endl;
    //     }

    //     SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    //     dbContext.releaseConnection(conn);
    // }

    inline void Add(const History &h, DbContext &dbContext)
    {
        SQLHDBC conn = dbContext.getConnection();
        SQLHSTMT hStmt;
        SQLAllocHandle(SQL_HANDLE_STMT, conn, &hStmt);

        std::string query = "INSERT INTO Call_History (dialedName, dialedTime) VALUES (?,?)";
        SQLPrepareA(hStmt, (SQLCHAR *)query.c_str(), SQL_NTS);

        std::string name = h.getdialedName();

        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLCHAR *)name.c_str(), 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)h.getdialedTime().c_str(), 0, NULL);
        SQLRETURN ret = SQLExecute(hStmt);

        if (!SQL_SUCCEEDED(ret))
        {
            std::cerr << "SQLExecute (add Call_History) failed" << std::endl;
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        dbContext.releaseConnection(conn);
    }

    inline std::vector<History> FindAll(DbContext &dbContext)
    {
        SQLHDBC conn = dbContext.getConnection();
        SQLHSTMT hStmt;
        SQLAllocHandle(SQL_HANDLE_STMT, conn, &hStmt);

        std::vector<History> list;

        std::string query =
            "SELECT ch.id, p.name, p.surname , ch.dialedTime "
            "FROM Call_History ch "
            "LEFT JOIN Directory p ON ch.dialedName = p.phonenumber";

        SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR *)query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret))
        {
            std::cerr << "findAll (Call_History) failed" << std::endl;
            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
            dbContext.releaseConnection(conn);
            return list;
        }

        while (SQLFetch(hStmt) == SQL_SUCCESS)
        {
            int id;
            char name[51] = {0};
            char surname[51] = {0};
            char dialedTime[51] = {0};
            SQLGetData(hStmt, 1, SQL_C_SLONG, &id, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
            SQLGetData(hStmt, 3, SQL_C_CHAR, surname, sizeof(surname), NULL);
            SQLGetData(hStmt, 4, SQL_C_CHAR, dialedTime, sizeof(dialedTime), NULL);

            std::string fullName = std::string(name) + " " + std::string(surname);

            list.emplace_back(HistoryFactory::generateFromDb(id, fullName,dialedTime));
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        dbContext.releaseConnection(conn);
        return list;
    }

}

#endif
