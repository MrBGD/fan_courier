#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "include/login_panel.h"
#include "include/dashboard_panel.h"
#include "include/cmd_exec.h"
#include "include/Exceptions.h"
#include "include/packet_observer.h"
#include "include/network_packet.h"
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

// Builds a handful of in-memory packets so the Panel-hierarchy demo below
// does not depend on a live capture device being available.
static std::vector<std::shared_ptr<CapturedPacket>> makeSamplePackets() {
    std::vector<std::shared_ptr<CapturedPacket>> sample;

    auto t1 = std::make_shared<TCPCapturedPacket>(1000, 0, 65535);
    t1->setSourceIP("10.0.0.5");
    auto t2 = std::make_shared<TCPCapturedPacket>(2000, 1000, 65535);
    t2->setSourceIP("10.0.0.6");
    auto h1 = std::make_shared<HTTPCapturedPacket>("GET", "/index.html", "example.com");
    h1->setSourceIP("10.0.0.5");

    sample.push_back(t1);
    sample.push_back(t2);
    sample.push_back(h1);
    return sample;
}

// Exercises Panel's pure virtual functions (execute/clone) and its
// Non-Virtual-Interface display purely through base-class pointers,
// independently of any single derived class holding them.
static void runPanelHierarchyDemo(Login& auth) {
    std::cout << "\n--- Panel hierarchy demo (execute/clone/operator<< via base pointer) ---\n";

    const auto sample = makeSamplePackets();

    std::vector<std::unique_ptr<Panel>> demoPanels;
    demoPanels.push_back(std::make_unique<CmdExec>("count tcp", sample));
    demoPanels.push_back(std::make_unique<CmdExec>("list http", sample));
    demoPanels.push_back(std::make_unique<CmdExec>("not_a_real_command", sample));

    for (auto& p : demoPanels) {
        try {
            p->execute();                          // pure virtual call through Panel*
            std::cout << *p << "\n";               // NVI display, dispatched virtually
            const auto cloned = p->clone();        // virtual constructor
            std::cout << "  cloned -> " << *cloned << "\n";
        } catch (const AppException& e) {
            std::cout << "  command failed: " << e.what() << "\n";
        }
    }

    try {
        const Panel& panelRef = auth;              // Login viewed through a base reference
        const auto loginClone = panelRef.clone();  // expected to throw: singleton
        std::cout << "  unexpected clone: " << *loginClone << "\n";
    } catch (const AppException& e) {
        std::cout << "Login::clone() rejected as expected: " << e.what() << "\n";
    }

    std::cout << "Panels created so far: " << Panel::getTotalPanelsCreated() << "\n";
}

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
        // Standalone demo of the GuestUser derived class (separate from the
        // "type 'guest' at the login prompt" shortcut wired into Login::login()).
        GuestUser g1;
        std::cout << "Created guest: " << g1 << "\n";
        std::cout << "Active guest sessions: " << GuestUser::getGuestCount() << "\n";
        g1.check_permissions();

        const auto g1Copy = g1.clone();
        std::cout << "Cloned guest: " << *g1Copy << "\n";
    }
    std::cout << "Active guest sessions after scope: " << GuestUser::getGuestCount() << "\n";
    std::cout << "Total users ever created: " << User::getTotalUsers() << "\n";

    runPanelHierarchyDemo(auth);

    Dashboard dashboard(std::move(loggedInUser));

    auto statsObs = std::make_shared<StatisticsObserver>("global-stats");
    auto alertObs = std::make_shared<AlertObserver>("alert-monitor", 50);
    dashboard.addObserver(statsObs);
    dashboard.addObserver(alertObs);

    std::cout << "\nDashboard ready: " << dashboard << "\n";

    // Exercise Dashboard's copy-and-swap copy constructor: the snapshot gets
    // a freshly cloned user/sub-panel but no running capture thread.
    Dashboard dashboardSnapshot(dashboard);
    std::cout << "Snapshot (fresh session, same user): " << dashboardSnapshot << "\n";

    // Exercise Dashboard's copy-and-swap copy assignment on a differently
    // constructed instance.
    Dashboard otherSession(std::make_unique<GuestUser>());
    otherSession = dashboardSnapshot;
    std::cout << "After copy-assignment: " << otherSession << "\n";

    // Dashboard also participates fully in the Panel interface: clone it
    // through Panel::clone() and recover the concrete type with dynamic_cast.
    const auto clonedPanel = dashboard.clone();
    if (const auto* clonedDashboard = dynamic_cast<const Dashboard*>(clonedPanel.get())) {
        std::cout << "Dashboard cloned via Panel::clone(): " << *clonedDashboard << "\n";
    }

    bool running = true;
    while (running) {
        std::cout << "\n--- DASHBOARD MENU ---\n"
                  << "1. Start Traffic Monitor\n"
                  << "2. Stop Traffic Monitor\n"
                  << "3. Check Packets\n"
                  << "4. Show Statistics\n"
                  << "5. Show Observers\n"
                  << "6. Run Command on Captured History\n"
                  << "7. Generate Traffic Report\n"
                  << "0. Exit\n"
                  << "Select option: ";

        int choice;
        if (!(std::cin >> choice)) break;

        try {
            switch (choice) {
                case 1: dashboard.open_traffic_monitor(); break;
                case 2: dashboard.stop_monitor();         break;
                case 3: dashboard.check_packet();         break;
                case 4: dashboard.show_statistics();      break;
                case 5: dashboard.showObservers();        break;
                case 6: {
                    std::cout << "Enter command (e.g. 'count tcp', 'list http', 'clear'): ";
                    std::cin.ignore();
                    std::string cmdLine;
                    std::getline(std::cin, cmdLine);
                    dashboard.runCommand(cmdLine);
                    break;
                }
                case 7: {
                    std::cout << "Enter report title: ";
                    std::cin.ignore();
                    std::string title;
                    std::getline(std::cin, title);
                    dashboard.generateReport(title);
                    break;
                }
                case 0: running = false; break;
                default: std::cout << "Invalid option.\n";
            }
        } catch (const AppException& e) {
            std::cerr << e.what() << "\n";
        }
    }

    std::cout << "Shutting down.\n";
    return 0;
}
