#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>

class User {
private:
    int id;
    std::string username;
    std::string password;
    std::string rol;   //admin ve ya user

public:

    //constructer kuralım.
    User(int id, const std::string& username, const std::string& password, const std::string& role)
    : id(id), username(username), password(password), rol(role) {}


    //getter
    int getId() const { return id;}
    const std::string& getUsername() const { return username;}
    const std::string& getPassword() const { return password;}
    const std::string& getRol() const { return rol;}

    //setterlar
    void setId(int newId) { id = newId;}
    void setUsername (const std::string&  newUsername) { username = newUsername;}
    void setPassword (const std::string&  newPassword) { password = newPassword;}
    void setRol (const std::string&  newRol) { rol = newRol;}
};


#endif //USER_HPP
