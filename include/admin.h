#pragma once
#ifndef ADMINISTRATION_PANEL_ADMIN_H
#define ADMINISTRATION_PANEL_ADMIN_H
#include "user.h"

class Admin : public User {

public:

    Admin(std::string name, unsigned int Id,std::string , std::string hash_pass): User(name, Id, "Administrator", std::move(hash_pass)){}

    ~Admin() override=default;
    void check_permissions() override{}
    bool permissions() override{return true;}


};


#endif