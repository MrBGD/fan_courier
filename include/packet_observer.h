#pragma once
#ifndef ADMINISTRATION_PANEL_PACKET_OBSERVER_H
#define ADMINISTRATION_PANEL_PACKET_OBSERVER_H


#include "network_packet.h"
#include "safe_queue.h"
#include <string>
#include <vector>
#include <memory>
#include <ostream>

class PacketObserver {
public:
    virtual ~PacketObserver() = default;
    virtual void onPacketCaptured(const CapturedPacket& packet) = 0;
    [[nodiscard]] virtual std::string getName()   const = 0;
    [[nodiscard]] virtual std::string toString()  const = 0;

    friend std::ostream& operator<<(std::ostream& os, const PacketObserver& obs) {
        os << obs.toString();
        return os;
    }
};


class StatisticsObserver : public PacketObserver {
    std::string       name;
    CaptureStatistics stats;

public:
    explicit StatisticsObserver(std::string observerName);

    void onPacketCaptured(const CapturedPacket& packet) override;

    [[nodiscard]] std::string getName()  const override { return name; }
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] const CaptureStatistics& getStats() const { return stats; }
};


class AlertObserver : public PacketObserver {
    std::string name;
    size_t      packetThreshold;
    size_t      currentCount{0};
    Safe_Queue<std::string> alertQueue;

public:
    explicit AlertObserver(std::string observerName, size_t threshold = 100);

    void onPacketCaptured(const CapturedPacket& packet) override;

    [[nodiscard]] bool   hasAlerts()  const { return !alertQueue.empty(); }
    [[nodiscard]] size_t alertCount() const { return alertQueue.size(); }
    bool popAlert(std::string& out)         { return alertQueue.try_pop(out); }

    [[nodiscard]] std::string getName()  const override { return name; }
    [[nodiscard]] std::string toString() const override;
};

#endif
