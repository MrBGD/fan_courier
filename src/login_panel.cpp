#include "../include/login_panel.h"
#include "../include/admin.h"
#include "../include/basic_user.h"
#include "../include/guest_user.h"
#include "../include/Exceptions.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../ext/json.hpp"
#include "../ext/picosha2.h"

using json = nlohmann::json;

static std::string sha256(const std::string& password) {
    std::string hex;
    picosha2::hash256_hex_string(password, hex);
    return hex;
}

void Login::load_users() {
    std::ifstream file("users.json");
    if (!file.is_open()) {
        throw FileNotFoundException("users.json");
    }

    json data;
    try {
        data = json::parse(file);
    } catch (const json::parse_error& e) {
        throw JsonParseException(std::string("invalid JSON — ") + e.what());
    }

    for (const auto& item : data["users"]) {
        std::string name  = item["username"];
        std::string hash  = item["password"];
        std::string role  = item["role"];
        unsigned int id   = item["id"];

        if (role == "Administrator") {
            users.push_back(std::make_unique<Admin>(name, id, role, hash));
        } else if (role == "Basic") {
            users.push_back(std::make_unique<BasicUser>(name, id, role, hash));
        } else {
            throw JsonParseException("unknown role '" + role + "'");
        }
    }
}

std::unique_ptr<User> Login::login() {
    std::string username;
    std::cout << "username: "; std::cin >> username;

    if (username == "guest") {
        std::cout << "Guest access granted (read-only, no password required).\n";
        return std::make_unique<GuestUser>();
    }

    std::string password;
    std::cout << "password: "; std::cin >> password;

    auto it = std::find_if(users.begin(), users.end(),
        [&username](const std::unique_ptr<User>& u) {
            return u->get_username() == username;
        });

    if (it == users.end()) throw UserNotFoundException();

    if (!(*it)->check_password(sha256(password))) throw InvalidPasswordException();

    std::cout << "Login successful.\n";
    std::unique_ptr<User> authenticated = std::move(*it);
    users.erase(it);
    return authenticated;
}

void Login::add_user(std::unique_ptr<User> user) {
    users.push_back(std::move(user));
}

void Login::printDetails(std::ostream& os) const {
    os << "registered_users=" << users.size();
}

void Login::execute() {
    std::cout << "[Login] Waiting for credentials — call login() to authenticate.\n";
}

std::unique_ptr<Panel> Login::clone() const {
    throw AppException("Login is a singleton and cannot be cloned.");
}
