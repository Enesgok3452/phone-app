#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
public:
    int id;
    std::string username;
    std::string password;

    User() = default;

    User(int id, const std::string &username, const std::string &password)
        : id(id), username(username), password(password) {
    }
};



class UserToken {
public:
    int id;
    int userId;
    std::string token;
    std::string createdAt;

    UserToken() = default;

    UserToken(int id, int userId, const std::string &token, const std::string &createdAt)
        : id(id), userId(userId), token(token), createdAt(createdAt) {
    }
};

#endif
