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
    std::unique_ptr<Panel>      activePanel;
    Safe_Queue<std::shared_ptr<CapturedPacket>> shared_queue;
    std::unique_ptr<Network_Traffic>            traffic_monitor;
    CaptureStatistics                           statistics;
    std::vector<std::shared_ptr<CapturedPacket>> capturedHistory;
    std::vector<std::shared_ptr<PacketObserver>> observers;

    [[nodiscard]] std::string panelLabel() const override { return "Dashboard"; }
    void printDetails(std::ostream& os) const override;

public:
    explicit Dashboard(std::unique_ptr<User> curr_user);
    Dashboard(const Dashboard& other);
    Dashboard& operator=(Dashboard other);
    ~Dashboard() override = default;

    friend void swap(Dashboard& a, Dashboard& b) noexcept;

    void open_traffic_monitor();
    void stop_monitor();
    void check_packet();
    void show_statistics() const;


    void runCommand(std::string cmd);

    // Builds a traffic report through a ReportPanel instance, saves it to
    // disk as Markdown, and reports the path it was written to.
    void generateReport(std::string title);

    // Lists previously saved reports and lets the user pick one to print.
    // Static: it only delegates to ReportPanel's static file helpers and
    // touches no Dashboard instance state.
    static void viewSavedReports();

    void addObserver(std::shared_ptr<PacketObserver> obs);
    void removeObserver(const std::string& name);
    void showObservers() const;

    // Theme-specific Panel action: delegates to whichever sub-panel is active.
    void execute() override;
    [[nodiscard]] std::unique_ptr<Panel> clone() const override;
};

#endif
