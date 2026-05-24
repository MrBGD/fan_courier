#pragma once
#ifndef ADMINISTRATION_PANEL_ADMIN_H
#define ADMINISTRATION_PANEL_ADMIN_H

#include "user.h"
#include <memory>

class Admin : public User {
public:
    Admin(std::string name, unsigned int id, std::string, std::string hash)
        : User(std::move(name), id, "Administrator", std::move(hash)) {}

    ~Admin() override = default;

    void check_permissions() override {}
    bool permissions() override { return true; }

    [[nodiscard]] std::unique_ptr<User> clone() const override {
        return std::make_unique<Admin>(*this);
    }
};

#endif
