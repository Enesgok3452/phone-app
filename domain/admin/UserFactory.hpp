#ifndef ADMIN_USERFACTORY_HPP
#define ADMIN_USERFACTORY_HPP

#include <string>
#include "User.hpp"

namespace UserFactory {
    inline User createUser(int id, const std::string& username, const std::string& plainPassword, const std::string& rol = "user") {
        return User(id, username, plainPassword, rol);
    }
}

#endif // ADMIN_USERFACTORY_HPP
