#include "../include/report_panel.h"
#include "../include/Exceptions.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
    // Keeps generated filenames filesystem-safe on every OS.
    std::string sanitizeForFilename(std::string text) {
        for (char& c : text) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_') {
                c = '_';
            }
        }
        return text.empty() ? "report" : text;
    }

    std::string timestampTag() {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tmBuf{};
#ifdef _WIN32
        localtime_s(&tmBuf, &now);
#else
        localtime_r(&now, &tmBuf);
#endif
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tmBuf);
        return buf;
    }
}

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

std::string ReportPanel::renderFullReport() const {
    std::ostringstream out;
    out << "# Traffic Report: " << reportTitle << "\n\n";
    out << "- Generated: " << timestampTag() << "\n";
    out << "- Tracked source IPs: " << topTalkers.size() << "\n\n";

    out << "## Summary\n\n```\n" << snapshot << "\n```\n\n";

    out << "## Top Talkers\n\n";
    out << "| Source IP | Packets |\n|---|---|\n";
    for (const auto& [ip, count] : topTalkers) {
        out << "| " << ip << " | " << count << " |\n";
    }

    out << "\n## Raw Traffic (" << packets.size() << " packets)\n\n";
    if (packets.empty()) {
        out << "_No packets were captured in this session._\n";
    }
    for (const auto& p : packets) {
        if (p) out << "- " << *p << "\n";
    }
    return out.str();
}

std::string ReportPanel::saveToFile() const {
    std::filesystem::create_directories(reportsDir());

    const std::string filename = timestampTag() + "_" + sanitizeForFilename(reportTitle) + ".md";
    const std::string path = reportsDir() + "/" + filename;

    std::ofstream out(path);
    if (!out) {
        throw AppException("Could not write report file: " + path);
    }
    out << renderFullReport();
    return path;
}

const std::string& ReportPanel::reportsDir() {
    static const std::string dir = "reports";
    return dir;
}

std::vector<std::string> ReportPanel::listSavedReports() {
    std::vector<std::string> names;
    if (!std::filesystem::exists(reportsDir())) {
        return names;
    }

    for (const auto& entry : std::filesystem::directory_iterator(reportsDir())) {
        if (entry.is_regular_file() && entry.path().extension() == ".md") {
            names.push_back(entry.path().filename().string());
        }
    }
    std::sort(names.begin(), names.end());  // timestamp prefix -> chronological order
    return names;
}

std::string ReportPanel::readReport(const std::string& filename) {
    const std::string path = reportsDir() + "/" + filename;
    std::ifstream in(path);
    if (!in) {
        throw FileNotFoundException(path);
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}
