#ifndef ADMINISTRATION_PANEL_EXCEPTIONS_H
#define ADMINISTRATION_PANEL_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& msg) : std::runtime_error(msg) {}
};

class UserNotFoundException : public AppException {
public:
    UserNotFoundException() : AppException("Error: User not found in database.") {}
};

class InvalidPasswordException : public AppException {
public:
    InvalidPasswordException() : AppException("Error: Incorrect password provided.") {}
};

class JsonParseException : public AppException {
public:
    explicit JsonParseException(const std::string& role)
        : AppException("Error: Invalid role '" + role + "' found in JSON.") {}
};

#endif //ADMINISTRATION_PANEL_EXCEPTIONS_H