#include "../include/dashboard_panel.h"
#include <iostream>
#include <algorithm>
#include "PcapLiveDeviceList.h"

Dashboard::Dashboard(std::unique_ptr<User> curr_user,
                     unsigned int do_action, std::string cmd)
    : Panel(true), current_user(std::move(curr_user)),
      action(do_action), command(std::move(cmd)) {}

void Dashboard::open_traffic_monitor() {
    if (!current_user->permissions()) {
        std::cout << "Access denied: insufficient permissions.\n";
        return;
    }
    if (traffic_monitor) {
        std::cout << "Monitor is already running.\n";
        return;
    }

    auto& devList  = pcpp::PcapLiveDeviceList::getInstance();
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

void Dashboard::send_command() {
    std::cout << "Sending: " << command << "\n";
}

void Dashboard::addObserver(std::shared_ptr<PacketObserver> obs) {
    observers.push_back(std::move(obs));
}

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

std::ostream& operator<<(std::ostream& os, const Dashboard& d) {
    os << "[Dashboard]";
    if (d.current_user) os << " user=" << *d.current_user;
    os << " | monitor=" << (d.traffic_monitor ? "running" : "stopped")
       << " | queue=" << d.shared_queue.size()
       << " | observers=" << d.observers.size();
    return os;
}
