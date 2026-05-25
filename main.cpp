#include <iostream>
#include <memory>
#include "include/login_panel.h"
#include "include/dashboard_panel.h"
#include "include/Exceptions.h"
#include "include/packet_observer.h"
#include "include/guest_user.h"
#include "include/user.h"

#ifdef _WIN32
#include <windows.h>
extern "C" {
    int nanosleep64(const void* /*req*/, void* /*rem*/) {
        Sleep(1);
        return 0;
    }
}
#endif

int main() {
    std::cout << "=== Traffic Monitor Administration Panel ===\n";

    std::cout << "Users registered so far: " << User::getTotalUsers() << "\n";

    Login& auth = Login::getInstance();

    try {
        auth.load_users();
    } catch (const AppException& e) {
        std::cerr << "Startup error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Users loaded. Total: " << User::getTotalUsers() << "\n";

    std::unique_ptr<User> loggedInUser;
    while (!loggedInUser) {
        try {
            loggedInUser = auth.login();
        } catch (const AppException& e) {
            std::cerr << e.what() << "\nTry again.\n\n";
        }
    }
    std::cout << "\nWelcome, " << *loggedInUser << "!\n";

    {
        GuestUser g1;
        std::cout << "Created guest: " << g1 << "\n";
        std::cout << "Active guest sessions: " << GuestUser::getGuestCount() << "\n";
        g1.check_permissions();


        auto g1Copy = g1.clone();
        std::cout << "Cloned guest: " << *g1Copy << "\n";
    }
    std::cout << "Active guest sessions after scope: " << GuestUser::getGuestCount() << "\n";
    std::cout << "Total users ever created: " << User::getTotalUsers() << "\n";

    Dashboard dashboard(std::move(loggedInUser), "");

    auto statsObs = std::make_shared<StatisticsObserver>("global-stats");
    auto alertObs = std::make_shared<AlertObserver>("alert-monitor", 50);
    dashboard.addObserver(statsObs);
    dashboard.addObserver(alertObs);

    std::cout << "\nDashboard ready: " << dashboard << "\n";

    bool running = true;
    while (running) {
        std::cout << "\n--- DASHBOARD MENU ---\n"
                  << "1. Start Traffic Monitor\n"
                  << "2. Stop Traffic Monitor\n"
                  << "3. Check Packets\n"
                  << "4. Show Statistics\n"
                  << "5. Show Observers\n"
                  << "0. Exit\n"
                  << "Select option: ";

        int choice;
        if (!(std::cin >> choice)) break;

        switch (choice) {
            case 1: dashboard.open_traffic_monitor(); break;
            case 2: dashboard.stop_monitor();         break;
            case 3: dashboard.check_packet();         break;
            case 4: dashboard.show_statistics();      break;
            case 5: dashboard.showObservers();        break;
            case 0: running = false;                  break;
            default: std::cout << "Invalid option.\n";
        }
    }

    std::cout << "Shutting down.\n";
    return 0;
}
