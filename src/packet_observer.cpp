#include "../include/packet_observer.h"
#include <sstream>

StatisticsObserver::StatisticsObserver(std::string observerName)
    : name(std::move(observerName)) {}

void StatisticsObserver::onPacketCaptured(const CapturedPacket& packet) {
    stats.update(packet);
}

std::string StatisticsObserver::toString() const {
    std::ostringstream oss;
    oss << "[StatisticsObserver: " << name << "]\n" << stats;
    return oss.str();
}

AlertObserver::AlertObserver(std::string observerName, size_t threshold)
    : name(std::move(observerName)), packetThreshold(threshold) {}

void AlertObserver::onPacketCaptured(const CapturedPacket& packet) {
    ++currentCount;
    if (currentCount >= packetThreshold) {
        std::ostringstream oss;
        oss << "ALERT [" << name << "]: threshold=" << packetThreshold
            << " reached | proto=" << packet.getProtocol()
            << " src=" << packet.getSourceIP();
        alertQueue.push(oss.str());
        currentCount = 0;
    }
}

std::string AlertObserver::toString() const {
    std::ostringstream oss;
    oss << "[AlertObserver: " << name
        << ", threshold=" << packetThreshold
        << ", pending alerts=" << alertQueue.size() << "]";
    return oss.str();
}
