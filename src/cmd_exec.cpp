#include "../include/cmd_exec.h"
#include "../include/Exceptions.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace {
    std::string toLower(std::string text) {
        std::transform(text.begin(), text.end(), text.begin(),
                        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return text;
    }
}

CmdExec::CmdExec(std::string cmd, std::vector<std::shared_ptr<CapturedPacket>> packets)
    : Panel(true), command(std::move(cmd)), targetPackets(std::move(packets)) {}

std::string CmdExec::runCount(const std::string& protocolFilter) const {
    if (protocolFilter.empty()) {
        return "total=" + std::to_string(targetPackets.size());
    }
    const auto matches = std::count_if(targetPackets.begin(), targetPackets.end(),
        [&protocolFilter](const std::shared_ptr<CapturedPacket>& p) {
            return p && toLower(p->getProtocol()) == toLower(protocolFilter);
        });
    return protocolFilter + "=" + std::to_string(matches);
}

std::string CmdExec::runList(const std::string& protocolFilter) const {
    std::ostringstream oss;
    size_t shown = 0;
    for (const auto& p : targetPackets) {
        if (!p) continue;
        if (!protocolFilter.empty() && toLower(p->getProtocol()) != toLower(protocolFilter)) continue;
        if (shown > 0) oss << "; ";
        oss << *p;
        if (++shown >= 5) break;
    }
    return shown == 0 ? "(no matching packets)" : oss.str();
}

void CmdExec::execute() {
    ++executionCount;

    std::istringstream iss(command);
    std::string verb, arg;
    iss >> verb >> arg;
    verb = toLower(verb);

    if (verb == "count") {
        lastResult = runCount(arg);
    } else if (verb == "list") {
        lastResult = runList(arg);
    } else if (verb == "clear") {
        targetPackets.clear();
        lastResult = "history cleared";
    } else {
        throw UnknownCommandException(command);
    }
}

void CmdExec::printDetails(std::ostream& os) const {
    os << "command='" << command << "' runs=" << executionCount
       << " tracked_packets=" << targetPackets.size()
       << " last_result='" << lastResult << "'";
}

std::unique_ptr<Panel> CmdExec::clone() const {
    return std::make_unique<CmdExec>(*this);
}
