#ifndef ADMINISTRATION_PANEL_NETWORK_PACKET_H
#define ADMINISTRATION_PANEL_NETWORK_PACKET_H

#include <string>
#include <chrono>
#include <vector>
#include <memory>

// Base packet class with common fields
struct CapturedPacket {
    std::string sourceIP;
    std::string destIP;
    uint16_t sourcePort;
    uint16_t destPort;
    std::string protocol;
    size_t packetLength;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> rawData;

    CapturedPacket();
    virtual ~CapturedPacket() = default;

    [[nodiscard]] virtual std::string getType() const;
    [[nodiscard]] std::chrono::system_clock::time_point getTimestamp() const;
    [[nodiscard]] virtual std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, const CapturedPacket& p) {
        os << p.toString();
        return os;
    }
};

struct HTTPCapturedPacket : public CapturedPacket {
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
    std::string userAgent;
    std::string contentType;
    int statusCode;
    std::string statusMessage;
    size_t contentLength;
    bool isRequest;

    HTTPCapturedPacket();
    std::string getType() const override;
    bool isSuccessful() const;
    std::string toString() const override;
};

struct DNSCapturedPacket : public CapturedPacket {
    uint16_t transactionId;
    std::string queryName;
    std::string queryType;
    std::vector<std::string> answers;
    bool isQuery;
    uint16_t queryCount;
    uint16_t answerCount;

    DNSCapturedPacket();
    std::string getType() const override;
    std::string toString() const override;
};

struct TCPCapturedPacket : public CapturedPacket {
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    uint16_t windowSize;
    bool syn;
    bool ack;
    bool fin;
    bool rst;
    bool psh;
    bool urg;
    size_t payloadLength;

    TCPCapturedPacket();
    std::string getType() const override;
    std::string getFlagsString() const;
    bool isConnectionEstablishment() const;
    bool isConnectionEstablishmentAck() const;
    bool isConnectionTermination() const;
    std::string toString() const override;
};

struct UDPCapturedPacket : public CapturedPacket {
    size_t payloadLength;

    UDPCapturedPacket();
    std::string getType() const override;
    std::string toString() const override;
};

struct ICMPCapturedPacket : public CapturedPacket {
    uint8_t type;
    uint8_t code;
    std::string typeDescription;

    ICMPCapturedPacket();
    std::string getType() const override;
    std::string toString() const override;
};

struct CaptureStatistics {
    size_t totalPackets;
    size_t tcpPackets;
    size_t udpPackets;
    size_t httpPackets;
    size_t dnsPackets;
    size_t icmpPackets;
    size_t otherPackets;
    size_t totalBytes;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastPacketTime;

    CaptureStatistics();
    void update(const CapturedPacket& packet);
    double getPacketsPerSecond() const;
    double getBytesPerSecond() const;
    std::string toString() const;
};


#endif //ADMINISTRATION_PANEL_NETWORK_PACKET_H