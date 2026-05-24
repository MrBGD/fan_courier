#pragma once
#ifndef ADMINISTRATION_PANEL_USER_H
#define ADMINISTRATION_PANEL_USER_H

#include <string>
#include <memory>
#include <ostream>


class User {
    std::string  username;
    unsigned int userId;
    std::string  role;
    std::string  hash_password;

    static int totalUsers;

public:
    User(std::string name, unsigned int id, std::string user_role, std::string hash);
    virtual ~User() = default;

    virtual void check_permissions() = 0;
    virtual bool permissions() = 0;
    [[nodiscard]] virtual std::unique_ptr<User> clone() const = 0;

    [[nodiscard]] const std::string& get_username() const { return username; }
    [[nodiscard]] const std::string& get_role()     const { return role; }
    [[nodiscard]] unsigned int       get_id()       const { return userId; }

    [[nodiscard]] bool check_password(const std::string& hash) const {
        return hash_password == hash;
    }

    static int getTotalUsers() { return totalUsers; }

    friend std::ostream& operator<<(std::ostream& os, const User& u);
};

#endif
