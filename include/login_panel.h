#pragma once
#ifndef ADMINISTRATION_PANEL_LOGIN_PANEL_H
#define ADMINISTRATION_PANEL_LOGIN_PANEL_H
#include <string>
#include "user.h"
#include <memory>
#include <vector>
#include "panel.h"



class Login : public Panel {

    Login() : Panel(true) {}
    std::vector<std::unique_ptr<User>> users ;

    [[nodiscard]] std::string panelLabel() const override { return "Login"; }
    void printDetails(std::ostream& os) const override;

public:
    Login(const Login&) = delete;
    Login& operator=(const Login&) = delete;
    ~Login() override = default;

    static Login& getInstance() {
        static Login instance;
        return instance;
    }
    std::unique_ptr<User> login();
    void add_user(std::unique_ptr<User> user);
    void load_users();
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t userCount() const { return users.size(); }

    // Theme-specific action: hand off to login() and report the outcome.
    void execute() override;

    // A singleton cannot be meaningfully duplicated
    [[nodiscard]] std::unique_ptr<Panel> clone() const override;

};




#endif
