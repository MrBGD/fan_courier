#pragma once
#ifndef ADMINISTRATION_PANEL_BASIC_USER_H
#define ADMINISTRATION_PANEL_BASIC_USER_H
#include "user.h"
#include <string>

class BasicUser : public User {
public:
    BasicUser(std::string name, unsigned int Id, std::string , std::string pass_hash)
        : User(std::move(name), Id, "Basic", std::move(pass_hash)){}

    ~BasicUser() override = default;

    void check_permissions() override{}
    bool permissions() override{return false;}
};

#endif