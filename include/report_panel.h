#pragma once
#ifndef ADMINISTRATION_PANEL_REPORT_PANEL_H
#define ADMINISTRATION_PANEL_REPORT_PANEL_H

#include "panel.h"
#include "network_packet.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Builds a small traffic report (top talkers by source IP) out of a
// snapshot of statistics and a set of previously captured packets.

class ReportPanel : public Panel {
    std::string                                  reportTitle;
    CaptureStatistics                            snapshot;
    std::vector<std::shared_ptr<CapturedPacket>> packets;
    std::map<std::string, size_t>                topTalkers;

    [[nodiscard]] std::string panelLabel() const override { return "Report"; }
    void printDetails(std::ostream& os) const override;

    void computeTopTalkers();

public:
    ReportPanel(std::string title, CaptureStatistics stats,
                std::vector<std::shared_ptr<CapturedPacket>> pkts);
    ~ReportPanel() override = default;

    void execute() override;
    [[nodiscard]] std::unique_ptr<Panel> clone() const override;

    [[nodiscard]] const std::map<std::string, size_t>& getTopTalkers() const { return topTalkers; }
};

#endif
