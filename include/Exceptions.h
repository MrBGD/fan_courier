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
    explicit JsonParseException(const std::string& detail)
        : AppException("Error: JSON parse failure — " + detail) {}
};

class FileNotFoundException : public AppException {
public:
    explicit FileNotFoundException(const std::string& filename)
        : AppException("Error: File not found: " + filename) {}
};

class NetworkException : public AppException {
public:
    explicit NetworkException(const std::string& msg)
        : AppException("Network error: " + msg) {}
};

class UnknownCommandException : public AppException {
public:
    explicit UnknownCommandException(const std::string& cmd)
        : AppException("Error: unknown command '" + cmd + "'.") {}
};

#endif
