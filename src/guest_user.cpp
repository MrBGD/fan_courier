#include "../include/guest_user.h"
#include <iostream>

int GuestUser::guestCount = 0;

GuestUser::GuestUser()
    : User("Guest_" + std::to_string(guestCount + 1), 0, "Guest", ""),
      sessionId(++guestCount) {}

GuestUser::GuestUser(std::string guestName)
    : User(std::move(guestName), 0, "Guest", ""), sessionId(++guestCount) {}

GuestUser::GuestUser(const GuestUser& other)
    : User(other), sessionId(++guestCount) {}

GuestUser::~GuestUser() {
    --guestCount;
}

void GuestUser::check_permissions() {
    std::cout << "[GuestUser] Read-only access. Session: " << sessionId << "\n";
}
