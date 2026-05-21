#pragma once
#ifndef ADMINISTRATION_PANEL_LOGIN_PANEL_H
#define ADMINISTRATION_PANEL_LOGIN_PANEL_H
#include <string>
#include "user.h"
#include <memory>
#include <vector>
#include "panel.h"



class Login : public Panel {

    Login()=default;
    std::vector<std::unique_ptr<User>> users ;

public:
    Login(const Login&) = delete;
    Login& operator=(const Login&) = delete;

    static Login& getInstance() {
        static Login instance;
        return instance;
    }
    std::unique_ptr<User> login();
    void add_user(std::unique_ptr<User> user);
    void load_users();

};





#endif