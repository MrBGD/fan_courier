#include "../include/user.h"
#include <ostream>

int User::totalUsers = 0;

User::User(std::string name, unsigned int id, std::string user_role, std::string hash)
    : username(std::move(name)), userId(id),
      role(std::move(user_role)), hash_password(std::move(hash))
{
    ++totalUsers;
}

void User::display(std::ostream& os) const {
    os << "[" << role << "] " << username << " (id=" << userId << ")";
    appendDetails(os);
}

std::ostream& operator<<(std::ostream& os, const User& u) {
    u.display(os);
    return os;
}
