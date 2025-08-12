/*
 *#ifndef ADMIN_USERFACTORY_HPP
#define ADMIN_USERFACTORY_HPP

#include <string>
#include "User.hpp"

namespace UserFactory {

    inline User generateUser(int id, const std::string username, const std::string password) {
        return User(id, username, password);
    }

    inline UserToken generateUserToken(int id, int userId, std::string token , std::string createdAt) {
        return UserToken(id, userId, token, createdAt);
    }

}


#endif // ADMIN_USERFACTORY_HPP
*/