#ifndef USER_REPOSITORY_HPP
#define USER_REPOSITORY_HPP

#include <string>
#include <optional>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include "../infrastructure/Db.hpp"
#include "User.hpp"


namespace UserRepository {
    inline std::optional<User> findByUsername(DbContext &dbContext, const std::string &username) {
        SQLHDBC dbc = dbContext.getConnection();
        SQLHSTMT stmt = nullptr;
        SQLRETURN ret;

        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLAllocHandle failed\n";
            return std::nullopt;
        }

        std::string query = "SELECT id, username, password FROM Users WHERE username = ?";
        ret = SQLPrepare(stmt, (SQLCHAR *) query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLPrepare failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                               username.size(), 0, (SQLPOINTER) username.c_str(), 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLExecute failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        int id = 0;
        char userBuffer[256] = {0};
        char passBuffer[256] = {0};
        SQLLEN idInd = 0, userInd = 0, passInd = 0;

        ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &id, 0, &idInd);
        ret = SQLBindCol(stmt, 2, SQL_C_CHAR, userBuffer, sizeof(userBuffer), &userInd);
        ret = SQLBindCol(stmt, 3, SQL_C_CHAR, passBuffer, sizeof(passBuffer), &passInd);

        ret = SQLFetch(stmt);
        if (ret == SQL_NO_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            std::cerr << "[UserRepository] Kullanıcı bulunamadı: " << username << "\n";
            return std::nullopt;
        } else if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLFetch failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        User user(id, std::string(userBuffer), std::string(passBuffer));

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return user;
    }


    inline std::optional<UserToken> getTokenByUsername(DbContext &dbContext, int userId) {
        SQLHDBC dbc = dbContext.getConnection();
        SQLHSTMT stmt = nullptr;
        SQLRETURN ret;

        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLAllocHandle failed\n";
            return std::nullopt;
        }

        std::string query = "SELECT id, user_id, token, created_at FROM UserTokens WHERE user_id = ?";
        ret = SQLPrepare(stmt, (SQLCHAR *) query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLPrepare failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                               0, 0, &userId, 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLExecute failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        int id = 0;
        int fetchedUserId = 0;
        char tokenBuffer[512] = {0};
        char createdAtBuffer[64] = {0};
        SQLLEN idInd = 0, userIdInd = 0, tokenInd = 0, createdAtInd = 0;

        ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &id, 0, &idInd);
        ret = SQLBindCol(stmt, 2, SQL_C_SLONG, &fetchedUserId, 0, &userIdInd);
        ret = SQLBindCol(stmt, 3, SQL_C_CHAR, tokenBuffer, sizeof(tokenBuffer), &tokenInd);
        ret = SQLBindCol(stmt, 4, SQL_C_CHAR, createdAtBuffer, sizeof(createdAtBuffer), &createdAtInd);

        ret = SQLFetch(stmt);
        if (ret == SQL_NO_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        } else if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLFetch failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        UserToken token(id, fetchedUserId, std::string(tokenBuffer), std::string(createdAtBuffer));

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return token;
    }


    inline bool generateToken(DbContext &dbContext, int userId, const std::string &token) {
        SQLHDBC dbc = dbContext.getConnection();
        SQLHSTMT stmt = nullptr;
        SQLRETURN ret;

        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLAllocHandle failed\n";
            return false;
        }

        std::string query = "INSERT INTO UserTokens(user_id, token) VALUES (?, ?)";
        ret = SQLPrepare(stmt, (SQLCHAR *) query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLPrepare failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                               0, 0, &userId, 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter 1 failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                               token.size(), 0, (SQLPOINTER) token.c_str(), 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter 2 failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLExecute failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }


    inline bool isTokenValid(DbContext &dbContext, const std::string &token) {
        SQLHDBC dbc = dbContext.getConnection();
        SQLHSTMT stmt = nullptr;
        SQLRETURN ret;

        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLAllocHandle failed\n";
            return false;
        }

        const std::string query = "SELECT COUNT(*) FROM UserTokens WHERE token = ?";
        ret = SQLPrepare(stmt, (SQLCHAR *) query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLPrepare failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                               token.size(), 0, SQLPOINTER(token.c_str()), 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLExecute failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        int count = 0;
        SQLLEN countInd = 0;
        ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &count, 0, &countInd);

        ret = SQLFetch(stmt);
        if (ret == SQL_NO_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        } else if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLFetch failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return count > 0;
    }

    inline bool isTokenValidForUser(DbContext &dbContext, const std::string &token, const std::string &username) {
        SQLHDBC dbc = dbContext.getConnection();
        SQLHSTMT stmt = nullptr;
        SQLRETURN ret;

        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLAllocHandle failed\n";
            return false;
        }

        const std::string query =
                "SELECT COUNT(*) FROM UserTokens ut "
                "JOIN Users u ON ut.user_id = u.id "
                "WHERE ut.token = ? AND u.username = ?";

        ret = SQLPrepare(stmt, (SQLCHAR *) query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLPrepare failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                               (SQLLEN) token.size(), 0, (SQLPOINTER) token.c_str(), 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter token failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                               (SQLLEN) username.size(), 0, (SQLPOINTER) username.c_str(), 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLBindParameter username failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLExecute failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        int count = 0;
        SQLLEN countInd = 0;
        ret = SQLBindCol(stmt, 1, SQL_C_SLONG, &count, 0, &countInd);

        ret = SQLFetch(stmt);
        if (ret == SQL_NO_DATA) {
            std::cerr << "[UserRepository] SQLFetch: no data found\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        } else if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "[UserRepository] SQLFetch failed\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        std::cerr << "[UserRepository] isTokenValidForUser: username=" << username
                << ", token=" << token << ", valid=" << (count > 0) << std::endl;

        return count > 0;
    }
} // namespace UserRepository

#endif // USER_REPOSITORY_HPP
