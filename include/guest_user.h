#pragma once
#ifndef ADMINISTRATION_PANEL_GUEST_USER_H
#define ADMINISTRATION_PANEL_GUEST_USER_H



#include "user.h"
#include <memory>
#include <string>

class GuestUser : public User {
    static int guestCount;
    int sessionId;

public:
    GuestUser();
    explicit GuestUser(std::string guestName);
    ~GuestUser() override;

    void check_permissions() override;
    bool permissions() override { return false; }

    [[nodiscard]] std::unique_ptr<User> clone() const override {
        return std::make_unique<GuestUser>(*this);
    }

    [[nodiscard]] int getSessionId()        const { return sessionId; }
    static int        getGuestCount()             { return guestCount; }
};

#endif
