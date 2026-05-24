#include "../include/dashboard_panel.h"
#include <iostream>
#include "PcapLiveDeviceList.h"
Dashboard::Dashboard(std::unique_ptr<User> curr_user, unsigned int do_action, std::string command_to_send)
    : Panel(true), current_user(std::move(curr_user)), action(do_action), command(command_to_send){}

void Dashboard::open_traffic_monitor() {
    if (!current_user->permissions()) {
        std::cout << "Access Denied: You do not have permission to start the monitor.\n";
        return;
    }

    if (traffic_monitor != nullptr) {
        std::cout << "Traffic monitor is already running! Stop it first.\n";
        return;
    }

    auto& devList = pcpp::PcapLiveDeviceList::getInstance();
    const std::vector<pcpp::PcapLiveDevice*>& devListVec = devList.getPcapLiveDevicesList();

    if (devListVec.empty()) {
        std::cerr << "No network interfaces found! Are you running as Administrator?\n";
        return;
    }

    std::cout << "\n=== Available Network Interfaces ===\n";
    for (size_t i = 0; i < devListVec.size(); ++i) {
        std::string name = devListVec[i]->getName();
        std::string desc = devListVec[i]->getDesc();

        std::cout << i << ". " << name << " (" << desc << ")\n";
    }

    std::cout << "\nSelect the interface that has active internet (0-" << devListVec.size() - 1 << "): ";
    size_t choice;
    std::cin >> choice;

    if (choice >= devListVec.size()) {
        std::cout << "Invalid choice. Aborting capture start.\n";
        return;
    }

    std::string interface_name = devListVec[choice]->getName();
    traffic_monitor = std::make_unique<Network_Traffic>(shared_queue, interface_name);

    std::cout << "Starting network capture on " << devListVec[choice]->getDesc() << " in the background...\n";
    traffic_monitor->start_monitoring();
}

void Dashboard::stop_monitor() {
    if (current_user->permissions()) {
        if (traffic_monitor) {
            std::cout << "Stopping network capture...\n";
            traffic_monitor->stop_monitoring();
            traffic_monitor.reset();
            std::cout << "Capture stopped successfully.\n";
        } else {
            std::cout << "Monitor is not currently running.\n";
        }
    } else {
        std::cout << "Access Denied.\n";
    }
}

void Dashboard::check_packet() {
    std::shared_ptr<CapturedPacket> packet;
    int count = 0;

    while (shared_queue.try_pop(packet) && count < 10) {
        std::cout << packet->toString() << "\n";
        count++;
    }

    if (count == 0) {
        std::cout << "Queue is empty. No packets captured yet.\n";
    } else {
        std::cout << "Displayed " << count << " packets. (More may be in the queue)\n";
    }
}

// void Dashboard::send_command() {
//
// }