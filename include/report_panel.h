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
// Reports can also be rendered as a full Markdown document (summary +
// raw packet listing) and persisted to/read back from disk.

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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::map<std::string, size_t>& getTopTalkers() const { return topTalkers; }

    // Renders the complete report as Markdown: title, statistics snapshot,
    // a top-talkers table, and a full listing of the raw packets behind it.
    [[nodiscard]] std::string renderFullReport() const;

    // Writes renderFullReport() under reportsDir() (creating it if needed)
    // and returns the path written to.
    std::string saveToFile() const;

    [[nodiscard]] static const std::string& reportsDir();

    // Filenames (not full paths) of every saved .md report, oldest first.
    [[nodiscard]] static std::vector<std::string> listSavedReports();

    // Reads back a previously saved report by filename (as returned by
    // listSavedReports()). Throws FileNotFoundException if it doesn't exist.
    [[nodiscard]] static std::string readReport(const std::string& filename);
};

#endif
