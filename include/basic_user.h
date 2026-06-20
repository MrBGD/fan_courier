#pragma once
#ifndef ADMINISTRATION_PANEL_BASIC_USER_H
#define ADMINISTRATION_PANEL_BASIC_USER_H

#include "user.h"
#include <memory>

class BasicUser : public User {
protected:
    void appendDetails(std::ostream& os) const override {
        os << " [limited access]";
    }

public:
    BasicUser(std::string name, unsigned int id, std::string, std::string hash)
        : User(std::move(name), id, "Basic", std::move(hash)) {}

    ~BasicUser() override = default;

    void check_permissions() override {}
    bool permissions() override { return false; }

    [[nodiscard]] std::unique_ptr<User> clone() const override {
        return std::make_unique<BasicUser>(*this);
    }
};

#endif
