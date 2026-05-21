#ifndef ADMINISTRATION_PANEL_NETWORK_H
#define ADMINISTRATION_PANEL_NETWORK_H

#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include "network_packet.h"
#include "safe_queue.h"

namespace pcpp {
    class PcapLiveDevice;
    class RawPacket;
}

class Network_Traffic {
private:
    std::thread monitor;
    std::atomic<bool> is_monitoring;
    Safe_Queue<std::shared_ptr<CapturedPacket>>& packet_queue;
    std::string interface_name;
    pcpp::PcapLiveDevice* device;

    void capture();
    static void onPacketArrives(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* dev, void* cookie);
    void handlePacket(pcpp::RawPacket* rawPacket);

public:
    Network_Traffic() = delete;
    Network_Traffic(const Network_Traffic&) = delete;
    Network_Traffic& operator=(const Network_Traffic&) = delete;
    Network_Traffic(Safe_Queue<std::shared_ptr<CapturedPacket>>& queue, const std::string& iface);
    ~Network_Traffic();

    void start_monitoring();
    void stop_monitoring();
};

#endif // ADMINISTRATION_PANEL_NETWORK_H