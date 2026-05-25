#include "../include/network_packet.h"
#include <sstream>
#include <iomanip>


CapturedPacket::CapturedPacket()
    : timestamp(std::chrono::system_clock::now()) {}

CapturedPacket::CapturedPacket(std::string srcIP, std::string dstIP,
                               uint16_t srcPort, uint16_t dstPort,
                               std::string proto, size_t len)
    : sourceIP(std::move(srcIP)), destIP(std::move(dstIP)),
      sourcePort(srcPort), destPort(dstPort),
      protocol(std::move(proto)), packetLength(len),
      timestamp(std::chrono::system_clock::now()) {}

CapturedPacket::CapturedPacket(const CapturedPacket& other)
    : sourceIP(other.sourceIP), destIP(other.destIP),
      sourcePort(other.sourcePort), destPort(other.destPort),
      protocol(other.protocol), packetLength(other.packetLength),
      timestamp(other.timestamp), rawData(other.rawData) {}

void swap(CapturedPacket& a, CapturedPacket& b) noexcept {
    using std::swap;
    swap(a.sourceIP,     b.sourceIP);
    swap(a.destIP,       b.destIP);
    swap(a.sourcePort,   b.sourcePort);
    swap(a.destPort,     b.destPort);
    swap(a.protocol,     b.protocol);
    swap(a.packetLength, b.packetLength);
    swap(a.timestamp,    b.timestamp);
    swap(a.rawData,      b.rawData);
}

CapturedPacket& CapturedPacket::operator=(CapturedPacket other) {
    swap(*this, other);
    return *this;
}

CapturedPacket::~CapturedPacket() = default;

std::unique_ptr<CapturedPacket> CapturedPacket::clone() const {
    return std::make_unique<CapturedPacket>(*this);
}

std::string CapturedPacket::getType() const { return "Generic"; }

std::string CapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[26];
#ifdef _WIN32
    ctime_s(buf, sizeof(buf), &time);
#else
    ctime_r(&time, buf);
#endif
    buf[24] = '\0';
    oss << "[" << buf << "] " << protocol << ": "
        << sourceIP << ":" << sourcePort << " -> "
        << destIP   << ":" << destPort
        << " (" << packetLength << " bytes)";
    return oss.str();
}

HTTPCapturedPacket::HTTPCapturedPacket() : CapturedPacket() {
    protocol = "HTTP";
}

HTTPCapturedPacket::HTTPCapturedPacket(std::string method_, std::string uri_, std::string host_)
    : CapturedPacket("", "", 0, 0, "HTTP", 0),
      method(std::move(method_)), uri(std::move(uri_)), host(std::move(host_)) {}

std::unique_ptr<CapturedPacket> HTTPCapturedPacket::clone() const {
    return std::make_unique<HTTPCapturedPacket>(*this);
}

std::string HTTPCapturedPacket::getType() const { return "HTTP"; }

bool HTTPCapturedPacket::isSuccessful() const {
    return statusCode >= 200 && statusCode < 300;
}

std::string HTTPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[26];
#ifdef _WIN32
    ctime_s(buf, sizeof(buf), &time);
#else
    ctime_r(&time, buf);
#endif
    buf[24] = '\0';
    oss << "[" << buf << "] HTTP ";
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

DNSCapturedPacket::DNSCapturedPacket() : CapturedPacket() {
    protocol = "DNS";
}

DNSCapturedPacket::DNSCapturedPacket(uint16_t transId, bool isQuery_,
                                     std::string queryName_, std::string queryType_)
    : CapturedPacket("", "", 0, 0, "DNS", 0),
      transactionId(transId), queryName(std::move(queryName_)),
      queryType(std::move(queryType_)), isQuery(isQuery_) {}

std::unique_ptr<CapturedPacket> DNSCapturedPacket::clone() const {
    return std::make_unique<DNSCapturedPacket>(*this);
}

std::string DNSCapturedPacket::getType() const { return "DNS"; }

std::string DNSCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[26];
#ifdef _WIN32
    ctime_s(buf, sizeof(buf), &time);
#else
    ctime_r(&time, buf);
#endif
    buf[24] = '\0';
    oss << "[" << buf << "] DNS "
        << (isQuery ? "Query" : "Response") << " ID:" << transactionId
        << " " << queryName << " (" << queryType << ") from " << sourceIP;
    if (!isQuery && !answers.empty()) {
        oss << " -> ";
        for (size_t i = 0; i < answers.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << answers[i];
        }
    }
    return oss.str();
}

TCPCapturedPacket::TCPCapturedPacket() : CapturedPacket() {
    protocol = "TCP";
}

TCPCapturedPacket::TCPCapturedPacket(uint32_t seqNum, uint32_t ackNum, uint16_t winSize)
    : CapturedPacket("", "", 0, 0, "TCP", 0),
      sequenceNumber(seqNum), ackNumber(ackNum), windowSize(winSize) {}

std::unique_ptr<CapturedPacket> TCPCapturedPacket::clone() const {
    return std::make_unique<TCPCapturedPacket>(*this);
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

bool TCPCapturedPacket::isConnectionEstablishment()    const { return syn && !ack; }
bool TCPCapturedPacket::isConnectionEstablishmentAck() const { return syn &&  ack; }
bool TCPCapturedPacket::isConnectionTermination()      const { return fin; }

std::string TCPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[26];
#ifdef _WIN32
    ctime_s(buf, sizeof(buf), &time);
#else
    ctime_r(&time, buf);
#endif
    buf[24] = '\0';
    oss << "[" << buf << "] TCP [" << getFlagsString() << "] "
        << sourceIP << ":" << sourcePort << " -> "
        << destIP   << ":" << destPort
        << " SEQ:" << sequenceNumber << " ACK:" << ackNumber
        << " WIN:" << windowSize << " LEN:" << payloadLength;
    return oss.str();
}

UDPCapturedPacket::UDPCapturedPacket() : CapturedPacket() {
    protocol = "UDP";
}

UDPCapturedPacket::UDPCapturedPacket(size_t payloadLen_)
    : CapturedPacket("", "", 0, 0, "UDP", 0), payloadLength(payloadLen_) {}

std::unique_ptr<CapturedPacket> UDPCapturedPacket::clone() const {
    return std::make_unique<UDPCapturedPacket>(*this);
}

std::string UDPCapturedPacket::getType() const { return "UDP"; }

std::string UDPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[26];
#ifdef _WIN32
    ctime_s(buf, sizeof(buf), &time);
#else
    ctime_r(&time, buf);
#endif
    buf[24] = '\0';
    oss << "[" << buf << "] UDP "
        << sourceIP << ":" << sourcePort << " -> "
        << destIP   << ":" << destPort
        << " LEN:" << payloadLength << " (" << packetLength << " bytes)";
    return oss.str();
}

ICMPCapturedPacket::ICMPCapturedPacket() : CapturedPacket() {
    protocol = "ICMP";
}

ICMPCapturedPacket::ICMPCapturedPacket(uint8_t type_, uint8_t code_, std::string desc_)
    : CapturedPacket("", "", 0, 0, "ICMP", 0),
      type(type_), code(code_), typeDescription(std::move(desc_)) {}

std::unique_ptr<CapturedPacket> ICMPCapturedPacket::clone() const {
    return std::make_unique<ICMPCapturedPacket>(*this);
}

std::string ICMPCapturedPacket::getType() const { return "ICMP"; }

std::string ICMPCapturedPacket::toString() const {
    std::ostringstream oss;
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buf[26];
#ifdef _WIN32
    ctime_s(buf, sizeof(buf), &time);
#else
    ctime_r(&time, buf);
#endif
    buf[24] = '\0';
    oss << "[" << buf << "] ICMP Type:" << static_cast<int>(type)
        << " Code:" << static_cast<int>(code);
    if (!typeDescription.empty()) oss << " (" << typeDescription << ")";
    oss << " " << sourceIP << " -> " << destIP
        << " (" << packetLength << " bytes)";
    return oss.str();
}

CaptureStatistics::CaptureStatistics()
    : startTime(std::chrono::system_clock::now()),
      lastPacketTime(std::chrono::system_clock::now()) {}

CaptureStatistics::CaptureStatistics(std::chrono::system_clock::time_point start)
    : startTime(start), lastPacketTime(start) {}

void CaptureStatistics::update(const CapturedPacket& packet) {
    totalPackets++;
    totalBytes    += packet.getPacketLength();
    lastPacketTime = packet.getTimestamp();

    const std::string& proto = packet.getProtocol();
    if      (proto == "TCP")  tcpPackets++;
    else if (proto == "UDP")  udpPackets++;
    else if (proto == "HTTP") httpPackets++;
    else if (proto == "DNS")  dnsPackets++;
    else if (proto == "ICMP") icmpPackets++;
    else                      otherPackets++;
}

double CaptureStatistics::getPacketsPerSecond() const {
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        lastPacketTime - startTime).count();
    return secs > 0 ? static_cast<double>(totalPackets) / secs : 0.0;
}

double CaptureStatistics::getBytesPerSecond() const {
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        lastPacketTime - startTime).count();
    return secs > 0 ? static_cast<double>(totalBytes) / secs : 0.0;
}

std::string CaptureStatistics::toString() const {
    std::ostringstream oss;
    oss << "=== Capture Statistics ===\n"
        << "  Total packets : " << totalPackets << "\n"
        << "    TCP  : " << tcpPackets  << "\n"
        << "    UDP  : " << udpPackets  << "\n"
        << "    HTTP : " << httpPackets << "\n"
        << "    DNS  : " << dnsPackets  << "\n"
        << "    ICMP : " << icmpPackets << "\n"
        << "    Other: " << otherPackets << "\n"
        << "  Total bytes   : " << totalBytes << "\n"
        << "  Packets/sec   : "
        << std::fixed << std::setprecision(2) << getPacketsPerSecond() << "\n"
        << "  Bytes/sec     : "
        << std::fixed << std::setprecision(2) << getBytesPerSecond()   << "\n";
    return oss.str();
}
