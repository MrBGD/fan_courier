#ifndef ADMINISTRATION_PANEL_DASHBOARD_PANEL_H
#define ADMINISTRATION_PANEL_DASHBOARD_PANEL_H

#include "panel.h"
#include "user.h"
#include "network.h"
#include "safe_queue.h"
#include "network_packet.h"
#include "packet_observer.h"
#include <memory>
#include <vector>
#include <string>
#include <ostream>



class Dashboard : public Panel {
    std::unique_ptr<User>       current_user;
    std::string                 command;
    Safe_Queue<std::shared_ptr<CapturedPacket>> shared_queue;
    std::unique_ptr<Network_Traffic>            traffic_monitor;
    CaptureStatistics                           statistics;
    std::vector<std::shared_ptr<CapturedPacket>> capturedHistory;
    std::vector<std::shared_ptr<PacketObserver>> observers;

public:
    Dashboard(std::unique_ptr<User> curr_user, std::string cmd);

    void open_traffic_monitor();
    void stop_monitor();
    void check_packet();
    void show_statistics() const;
    void send_command();


    void addObserver(std::shared_ptr<PacketObserver> obs);
    void removeObserver(const std::string& name);
    void showObservers() const;

    friend std::ostream& operator<<(std::ostream& os, const Dashboard& d);
};

#endif
