#include "../include/network_packet.h"
#include <sstream>
#include <iomanip>

CapturedPacket::CapturedPacket()
    : sourcePort(0), destPort(0), packetLength(0), timestamp(std::chrono::system_clock::now()) {}

std::string CapturedPacket::getType() const { return "Generic"; }

std::chrono::system_clock::time_point CapturedPacket::getTimestamp() const { return timestamp; }

std::string CapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char timeStr[26];
    #ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &time);
    #else
    ctime_r(&time, timeStr);
    #endif
    timeStr[24] = '\0';

    oss << "[" << timeStr << "] "
        << protocol << ": "
        << sourceIP << ":" << sourcePort << " -> "
        << destIP << ":" << destPort
        << " (" << packetLength << " bytes)";
    return oss.str();
}


HTTPCapturedPacket::HTTPCapturedPacket()
    : statusCode(0), contentLength(0), isRequest(true) {
    protocol = "HTTP";
}

std::string HTTPCapturedPacket::getType() const { return "HTTP"; }

bool HTTPCapturedPacket::isSuccessful() const {
    return statusCode >= 200 && statusCode < 300;
}

std::string HTTPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char timeStr[26];
    #ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &time);
    #else
    ctime_r(&time, timeStr);
    #endif
    timeStr[24] = '\0';

    oss << "[" << timeStr << "] HTTP ";
    if (isRequest) {
        oss << method << " " << host << uri << " " << version;
    } else {
        oss << statusCode << " " << statusMessage;
    }
    oss << " | " << sourceIP << ":" << sourcePort
        << " -> " << destIP << ":" << destPort
        << " (" << packetLength << " bytes)";

    return oss.str();
}

DNSCapturedPacket::DNSCapturedPacket()
    : transactionId(0), isQuery(true), queryCount(0), answerCount(0) {
    protocol = "DNS";
}

std::string DNSCapturedPacket::getType() const { return "DNS"; }

std::string DNSCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char timeStr[26];
    #ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &time);
    #else
    ctime_r(&time, timeStr);
    #endif
    timeStr[24] = '\0';

    oss << "[" << timeStr << "] DNS "
        << (isQuery ? "Query" : "Response") << " "
        << "ID:" << transactionId << " "
        << queryName << " (" << queryType << ") "
        << "from " << sourceIP;

    if (!isQuery && !answers.empty()) {
        oss << " -> ";
        for (size_t i = 0; i < answers.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << answers[i];
        }
    }
    return oss.str();
}

TCPCapturedPacket::TCPCapturedPacket()
    : sequenceNumber(0), ackNumber(0), windowSize(0),
      syn(false), ack(false), fin(false),
      rst(false), psh(false), urg(false), payloadLength(0) {
    protocol = "TCP";
}

std::string TCPCapturedPacket::getType() const { return "TCP"; }

std::string TCPCapturedPacket::getFlagsString() const {
    std::string flags;
    if (syn) flags += "SYN ";
    if (ack) flags += "ACK ";
    if (fin) flags += "FIN ";
    if (rst) flags += "RST ";
    if (psh) flags += "PSH ";
    if (urg) flags += "URG ";
    if (!flags.empty()) flags.pop_back();
    return flags.empty() ? "NONE" : flags;
}

bool TCPCapturedPacket::isConnectionEstablishment() const { return syn && !ack; }
bool TCPCapturedPacket::isConnectionEstablishmentAck() const { return syn && ack; }
bool TCPCapturedPacket::isConnectionTermination() const { return fin; }

std::string TCPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char timeStr[26];
    #ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &time);
    #else
    ctime_r(&time, timeStr);
    #endif
    timeStr[24] = '\0';

    oss << "[" << timeStr << "] TCP "
        << "[" << getFlagsString() << "] "
        << sourceIP << ":" << sourcePort << " -> "
        << destIP << ":" << destPort
        << " SEQ:" << sequenceNumber << " ACK:" << ackNumber
        << " WIN:" << windowSize
        << " LEN:" << payloadLength;
    return oss.str();
}

UDPCapturedPacket::UDPCapturedPacket() : payloadLength(0) { protocol = "UDP"; }
std::string UDPCapturedPacket::getType() const { return "UDP"; }

std::string UDPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char timeStr[26];
    #ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &time);
    #else
    ctime_r(&time, timeStr);
    #endif
    timeStr[24] = '\0';

    oss << "[" << timeStr << "] UDP "
        << sourceIP << ":" << sourcePort << " -> "
        << destIP << ":" << destPort
        << " LEN:" << payloadLength
        << " (" << packetLength << " bytes)";
    return oss.str();
}

ICMPCapturedPacket::ICMPCapturedPacket() : type(0), code(0) {
    protocol = "ICMP";
    sourcePort = 0;
    destPort = 0;
}
std::string ICMPCapturedPacket::getType() const { return "ICMP"; }

std::string ICMPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char timeStr[26];
    #ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &time);
    #else
    ctime_r(&time, timeStr);
    #endif
    timeStr[24] = '\0';

    oss << "[" << timeStr << "] ICMP "
        << "Type:" << static_cast<int>(type)
        << " Code:" << static_cast<int>(code);

    if (!typeDescription.empty()) {
        oss << " (" << typeDescription << ")";
    }

    oss << " " << sourceIP << " -> " << destIP
        << " (" << packetLength << " bytes)";
    return oss.str();
}

CaptureStatistics::CaptureStatistics()
    : totalPackets(0), tcpPackets(0), udpPackets(0),
      httpPackets(0), dnsPackets(0), icmpPackets(0),
      otherPackets(0), totalBytes(0),
      startTime(std::chrono::system_clock::now()),
      lastPacketTime(std::chrono::system_clock::now()) {}

void CaptureStatistics::update(const CapturedPacket& packet) {
    totalPackets++;
    totalBytes += packet.packetLength;
    lastPacketTime = packet.timestamp;

    if (packet.protocol == "TCP") tcpPackets++;
    else if (packet.protocol == "UDP") udpPackets++;
    else if (packet.protocol == "HTTP") httpPackets++;
    else if (packet.protocol == "DNS") dnsPackets++;
    else if (packet.protocol == "ICMP") icmpPackets++;
    else otherPackets++;
}

double CaptureStatistics::getPacketsPerSecond() const {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(lastPacketTime - startTime).count();
    return duration > 0 ? static_cast<double>(totalPackets) / duration : 0.0;
}

double CaptureStatistics::getBytesPerSecond() const {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(lastPacketTime - startTime).count();
    return duration > 0 ? static_cast<double>(totalBytes) / duration : 0.0;
}

std::string CaptureStatistics::toString() const {
    std::ostringstream oss;
    oss << "=== Capture Statistics ===\n"
        << "Total Packets: " << totalPackets << "\n"
        << "  TCP: " << tcpPackets << "\n"
        << "  UDP: " << udpPackets << "\n"
        << "  HTTP: " << httpPackets << "\n"
        << "  DNS: " << dnsPackets << "\n"
        << "  ICMP: " << icmpPackets << "\n"
        << "  Other: " << otherPackets << "\n"
        << "Total Bytes: " << totalBytes << "\n"
        << "Packets/sec: " << std::fixed << std::setprecision(2)
        << getPacketsPerSecond() << "\n"
        << "Bytes/sec: " << getBytesPerSecond() << "\n";
    return oss.str();
}