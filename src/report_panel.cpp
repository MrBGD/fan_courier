#include "../include/report_panel.h"

ReportPanel::ReportPanel(std::string title, CaptureStatistics stats,
                         std::vector<std::shared_ptr<CapturedPacket>> pkts)
    : Panel(true), reportTitle(std::move(title)),
      snapshot(std::move(stats)), packets(std::move(pkts)) {}

void ReportPanel::computeTopTalkers() {
    topTalkers.clear();
    for (const auto& p : packets) {
        if (!p) continue;
        ++topTalkers[p->getSourceIP().empty() ? "(unknown)" : p->getSourceIP()];
    }
}

void ReportPanel::execute() {
    computeTopTalkers();
}

void ReportPanel::printDetails(std::ostream& os) const {
    os << "title='" << reportTitle << "' | " << snapshot
       << " tracked_sources=" << topTalkers.size();
}

std::unique_ptr<Panel> ReportPanel::clone() const {
    return std::make_unique<ReportPanel>(*this);
}
