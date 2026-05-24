#include "../include/network.h"
#include "../include/Exceptions.h"
#include <iostream>
#include <chrono>
#include "PcapLiveDeviceList.h"
#include "PcapLiveDevice.h"
#include "Packet.h"
#include "IPv4Layer.h"
#include "TcpLayer.h"
#include "UdpLayer.h"

Network_Traffic::Network_Traffic(Safe_Queue<std::shared_ptr<CapturedPacket>>& queue,
                                 const std::string& iface)
    : is_monitoring(false), packet_queue(queue), interface_name(iface), device(nullptr) {}

Network_Traffic::~Network_Traffic() {
    stop_monitoring();
}

void Network_Traffic::start_monitoring() {
    if (!is_monitoring) {
        is_monitoring = true;
        monitor = std::thread(&Network_Traffic::capture, this);
    }
}

void Network_Traffic::stop_monitoring() {
    is_monitoring = false;
    if (monitor.joinable())
        monitor.join();
}

void Network_Traffic::capture() {
    auto& devList = pcpp::PcapLiveDeviceList::getInstance();
    device = devList.getPcapLiveDeviceByName(interface_name);

    if (!device || !device->open()) {
        std::cerr << "Failed to open interface: " << interface_name << "\n";
        is_monitoring = false;
        return;
    }

    device->startCapture(onPacketArrives, this);
    while (is_monitoring) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    device->stopCapture();
    device->close();
}

void Network_Traffic::onPacketArrives(pcpp::RawPacket* rawPacket,
                                      pcpp::PcapLiveDevice*,
                                      void* cookie) {
    static_cast<Network_Traffic*>(cookie)->handlePacket(rawPacket);
}

void Network_Traffic::handlePacket(pcpp::RawPacket* rawPacket) {
    pcpp::Packet parsed(rawPacket);

    auto pkt = std::make_shared<CapturedPacket>();
    pkt->setTimestamp(std::chrono::system_clock::now());
    pkt->setPacketLength(static_cast<size_t>(rawPacket->getRawDataLen()));

    if (auto* ipv4 = parsed.getLayerOfType<pcpp::IPv4Layer>()) {
        pkt->setSourceIP(ipv4->getSrcIPAddress().toString());
        pkt->setDestIP(ipv4->getDstIPAddress().toString());
    }

    if (parsed.getLayerOfType<pcpp::TcpLayer>()) {
        pkt->setProtocol("TCP");
    } else if (parsed.getLayerOfType<pcpp::UdpLayer>()) {
        pkt->setProtocol("UDP");
    } else {
        pkt->setProtocol("Other");
    }

    packet_queue.push(pkt);
}
