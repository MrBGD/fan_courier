#include "../include/dashboard_panel.h"
#include "../include/cmd_exec.h"
#include "../include/report_panel.h"
#include "../include/Exceptions.h"
#include <algorithm>
#include <iostream>
#include "PcapLiveDeviceList.h"

Dashboard::Dashboard(std::unique_ptr<User> curr_user)
    : Panel(true), current_user(std::move(curr_user)) {}

Dashboard::Dashboard(const Dashboard& other)
    : Panel(other.is_running),
      current_user(other.current_user ? other.current_user->clone() : nullptr),
      activePanel(other.activePanel ? other.activePanel->clone() : nullptr),
      statistics(other.statistics),
      capturedHistory(other.capturedHistory),
      observers(other.observers) {}

void swap(Dashboard& a, Dashboard& b) noexcept {
    using std::swap;
    swap(a.is_running,      b.is_running);
    swap(a.current_user,    b.current_user);
    swap(a.activePanel,     b.activePanel);
    swap(a.statistics,      b.statistics);
    swap(a.capturedHistory, b.capturedHistory);
    swap(a.observers,       b.observers);
}

Dashboard& Dashboard::operator=(Dashboard other) {
    swap(*this, other);
    return *this;
}

void Dashboard::open_traffic_monitor() {
    if (!current_user->permissions()) {
        std::cout << "Access denied: insufficient permissions.\n";
        return;
    }
    if (traffic_monitor) {
        std::cout << "Monitor is already running.\n";
        return;
    }

    const auto& devList  = pcpp::PcapLiveDeviceList::getInstance();
    const auto& devs = devList.getPcapLiveDevicesList();

    if (devs.empty()) {
        std::cout << "No network interfaces found (need administrator/root).\n";
        return;
    }

    std::cout << "\n=== Available Network Interfaces ===\n";
    for (size_t i = 0; i < devs.size(); ++i) {
        std::cout << i << ". " << devs[i]->getName()
                  << " (" << devs[i]->getDesc() << ")\n";
    }
    std::cout << "Select interface (0-" << devs.size() - 1 << "): ";

    size_t choice = 0;
    std::cin >> choice;
    if (choice >= devs.size()) {
        std::cout << "Invalid choice.\n";
        return;
    }

    traffic_monitor = std::make_unique<Network_Traffic>(shared_queue,
                                                        devs[choice]->getName());
    std::cout << "Starting capture on " << devs[choice]->getDesc() << "...\n";
    traffic_monitor->start_monitoring();
}

void Dashboard::stop_monitor() {
    if (!current_user->permissions()) {
        std::cout << "Access denied.\n";
        return;
    }
    if (!traffic_monitor) {
        std::cout << "Monitor is not running.\n";
        return;
    }
    traffic_monitor->stop_monitoring();
    traffic_monitor.reset();
    std::cout << "Capture stopped.\n";
}

void Dashboard::check_packet() {
    std::shared_ptr<CapturedPacket> packet;
    int count = 0;

    while (shared_queue.try_pop(packet) && count < 10) {
        statistics.update(*packet);
        capturedHistory.push_back(packet);


        for (auto& obs : observers) {
            obs->onPacketCaptured(*packet);
        }

        std::cout << *packet << "\n";
        ++count;
    }

    if (count == 0) {
        std::cout << "No packets in queue yet.\n";
    } else {
        std::cout << "Showed " << count << " packet(s).\n";
    }
}

void Dashboard::show_statistics() const {
    std::cout << statistics;

    auto httpPkts = filterByType<HTTPCapturedPacket>(capturedHistory);
    auto tcpPkts  = filterByType<TCPCapturedPacket>(capturedHistory);

    std::cout << "  (from history: "
              << httpPkts.size() << " HTTP, "
              << tcpPkts.size()  << " TCP)\n";
}

void Dashboard::runCommand(std::string cmd) {
    activePanel = std::make_unique<CmdExec>(std::move(cmd), capturedHistory);

    try {
        activePanel->execute();                 // pure virtual call through the base pointer
    } catch (const AppException& e) {
        std::cout << e.what() << "\n";
        return;
    }

    // CmdExec::printDetails() already includes the result and run count,
    // so the NVI display alone is enough here - no downcast needed.
    std::cout << *activePanel << "\n";
}

void Dashboard::generateReport(std::string title) {
    auto report = std::make_unique<ReportPanel>(std::move(title), statistics, capturedHistory);

    const ReportPanel* reportView = report.get();
    activePanel = std::move(report);

    activePanel->execute();                  // pure virtual call through the base pointer
    std::cout << *activePanel << "\n";       // NVI display (already shows top-talker count)

    const std::string savedPath = reportView->saveToFile();
    std::cout << "  -> report saved to " << savedPath << "\n";
}

void Dashboard::viewSavedReports() {
    const auto reports = ReportPanel::listSavedReports();
    if (reports.empty()) {
        std::cout << "No saved reports yet. Use 'Generate Traffic Report' first.\n";
        return;
    }

    std::cout << "=== Saved Reports ===\n";
    for (size_t i = 0; i < reports.size(); ++i) {
        std::cout << i << ". " << reports[i] << "\n";
    }
    std::cout << "Select a report to view (0-" << reports.size() - 1 << "): ";

    size_t choice = 0;
    std::cin >> choice;
    if (choice >= reports.size()) {
        std::cout << "Invalid choice.\n";
        return;
    }

    std::cout << "\n" << ReportPanel::readReport(reports[choice]) << "\n";
}

void Dashboard::addObserver(std::shared_ptr<PacketObserver> obs) {
    observers.push_back(std::move(obs));
}

// cppcheck-suppress unusedFunction
void Dashboard::removeObserver(const std::string& name) {
    observers.erase(
        std::remove_if(observers.begin(), observers.end(),
            [&name](const std::shared_ptr<PacketObserver>& o) {
                return o->getName() == name;
            }),
        observers.end());
}

void Dashboard::showObservers() const {
    if (observers.empty()) {
        std::cout << "No observers registered.\n";
        return;
    }
    std::cout << "=== Registered Observers ===\n";
    for (const auto& obs : observers) {
        std::cout << *obs << "\n";
    }

    for (auto& obs : observers) {
        if (auto* alert = dynamic_cast<AlertObserver*>(obs.get())) {
            std::string msg;
            while (alert->popAlert(msg)) {
                std::cout << "  >> " << msg << "\n";
            }
        }
    }
}

void Dashboard::execute() {
    if (activePanel) {
        activePanel->execute();
    } else {
        std::cout << "[Dashboard] No active sub-panel; use runCommand() or generateReport().\n";
    }
}

std::unique_ptr<Panel> Dashboard::clone() const {
    return std::make_unique<Dashboard>(*this);
}

void Dashboard::printDetails(std::ostream& os) const {
    if (current_user) os << "user=" << *current_user << " | ";
    os << "monitor=" << (traffic_monitor ? "running" : "stopped")
       << " | queue=" << shared_queue.size()
       << " | history=" << capturedHistory.size()
       << " | observers=" << observers.size();

    if (activePanel) {
        os << " | active_panel={" << *activePanel << "}";
    } else {
        os << " | active_panel=none";
    }
}
