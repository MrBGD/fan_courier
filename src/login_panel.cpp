#include "../include/login_panel.h"
#include "../include/admin.h"
#include "../include/basic_user.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "../ext/json.hpp"
#include "../ext/picosha2.h"
#include <algorithm>
using json=nlohmann::json;

static std::string hash_password(const std::string& password) {
    std::string hash_hex_str;
    picosha2::hash256_hex_string(password, hash_hex_str);
    return hash_hex_str;
}

void Login::load_users() {
    std::ifstream file("../users.json");
    if (!file.is_open()) {
        std::cout<<"error"; // adaugat la exceptii
    }
    json data=json::parse(file);

    for (const auto& item : data["users"]) {
        std::string name=item["username"];
        std::string hash_password=item["password"];
        std::string role=item["role"];
        unsigned int id=item["id"];
        if (role =="Administrator") {
            users.push_back(std::make_unique<Admin>(name,id,role,hash_password));
        }
        else if (role=="Basic") {
            users.push_back(std::make_unique<BasicUser>(name,id,role,hash_password));
        }
        else {
            std::cout<<"error de json"; //exceptie
        }
    }

}

std::unique_ptr<User> Login::login() {
    std::string username;
    std::string password;
    std::cout<<"username: "; std::cin>>username;
    std::cout<<"password: "; std::cin>>password;

    auto user_iterate = std::find_if(users.begin(),users.end(),[&username](const std::unique_ptr<User>& user) {
        return user->get_username()==username;
    });

    if (user_iterate==users.end()) {
        throw UserNotFoundException();
    }
    if ((*user_iterate)->check_password(hash_password(password))) {
        std::cout<<"logged in !";
        std::unique_ptr<User> authenticatedUser = std::move(*user_iterate);
        users.erase(user_iterate);
        return authenticatedUser;
    }
    else
        throw InvalidPasswordException();
}

// void Login::add_user(std::unique_ptr<User> user) {
//
// }
