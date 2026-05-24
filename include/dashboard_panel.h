#ifndef ADMINISTRATION_PANEL_DASHBOARD_PANEL_H
#define ADMINISTRATION_PANEL_DASHBOARD_PANEL_H

#include "panel.h"
#include <memory>
#include <vector>
#include <string>
#include "user.h"
#include "network.h"
#include "safe_queue.h"
#include "network_packet.h"

class Dashboard : public Panel {
private:
    std::unique_ptr<User> current_user;
    unsigned int action;
    std::string command;
    Safe_Queue<std::shared_ptr<CapturedPacket>> shared_queue;
    std::unique_ptr<Network_Traffic> traffic_monitor;

public:
    Dashboard(std::unique_ptr<User> curr_user, unsigned int do_action, std::string command_to_send);

    void send_command();
    void open_traffic_monitor();
    void stop_monitor();
    void check_packet();
};

#endif