#include "../include/network.h"
#include <iostream>
#include <chrono>
#include "PcapLiveDeviceList.h"
#include "PcapLiveDevice.h"
#include "Packet.h"
#include "IPv4Layer.h"
#include "TcpLayer.h"

Network_Traffic::Network_Traffic(Safe_Queue<std::shared_ptr<CapturedPacket>>& queue, const std::string& iface)
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
        std::cerr << "Failed to find or open interface: " << interface_name << "\n";
        is_monitoring = false;
        return;
    }

    device->startCapture(onPacketArrives, this);
    while (is_monitoring) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (device != nullptr) {
        device->stopCapture();
        device->close();
    }
}

void Network_Traffic::onPacketArrives(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* , void* cookie) {
    Network_Traffic* instance = static_cast<Network_Traffic*>(cookie);
    instance->handlePacket(rawPacket);
}

void Network_Traffic::handlePacket(pcpp::RawPacket* rawPacket) {
    pcpp::Packet parsedPacket(rawPacket);
    auto new_packet = std::make_shared<CapturedPacket>();
    new_packet->timestamp = std::chrono::system_clock::now();
    new_packet->packetLength = rawPacket->getRawDataLen();

    if (auto* ipv4Layer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>()) {
        new_packet->sourceIP = ipv4Layer->getSrcIPAddress().toString();
        new_packet->destIP = ipv4Layer->getDstIPAddress().toString();
    }
    packet_queue.push(new_packet);
}