#pragma once
#ifndef ADMINISTRATION_PANEL_USER_H
#define ADMINISTRATION_PANEL_USER_H
#include <string.h>


class User {
    std::string username;
    unsigned int userId;
    std::string role;
    std::string hash_password;


public:
    User(std::string name,unsigned int Id, std::string user_role, std::string hash): username(name), userId(Id), role(user_role),hash_password(hash) {}
    virtual ~User()=default;
    virtual void check_permissions() =0;
    virtual bool permissions() =0;

    std::string get_username() const {return username;}
    std::string get_role() const {return role;}

    bool check_password(const std::string& hash) const {
        return hash_password==hash;
    }

};



#endif