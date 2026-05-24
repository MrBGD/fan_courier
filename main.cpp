#include <iostream>
#include <memory>
#include "include/login_panel.h"
#include "include/dashboard_panel.h"
#include "include/Exceptions.h"
#ifdef _WIN32
#include <windows.h>
extern "C" {
    int nanosleep64(const void* req, void* rem) {
        Sleep(1);
        return 0;
    }
}
#endif


int main() {
    std::cout << "Starting Administration Panel\n";

    Login& authSystem = Login::getInstance();

    try {
        authSystem.load_users();
    } catch (const std::exception& e) {
        std::cerr << "Startup Error: " << e.what() << "\n";
        return 1;
    }

    std::unique_ptr<User> loggedInUser = nullptr;
    while (!loggedInUser) {
        try {
            loggedInUser = authSystem.login();
        } catch (const AppException& e) {
            std::cerr << e.what() << "\nTry again.\n\n";
        }
    }

    std::cout << "\nWelcome, " << loggedInUser->get_username() << "!\n";

    Dashboard dashboard(std::move(loggedInUser), 1, "");

    bool running = true;
    while(running) {
        std::cout << "\n--- DASHBOARD MENU ---\n";
        std::cout << "1. Start Traffic Monitor\n";
        std::cout << "2. Stop Traffic Monitor\n";
        std::cout << "3. Check Packets\n";
        std::cout << "0. Exit\n";
        std::cout << "Select option: ";

        int choice;
        std::cin >> choice;

        switch(choice) {
            case 1:
                dashboard.open_traffic_monitor();
                break;
            case 2:
                dashboard.stop_monitor();
                break;
            case 3:
                dashboard.check_packet();
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Invalid option\n";
        }
    }

    std::cout << "Shutting down \n";
    return 0;
}