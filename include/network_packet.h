#ifndef ADMINISTRATION_PANEL_NETWORK_PACKET_H
#define ADMINISTRATION_PANEL_NETWORK_PACKET_H

#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <ostream>

class CapturedPacket {
protected:
    std::string sourceIP;
    std::string destIP;
    uint16_t    sourcePort{0};
    uint16_t    destPort{0};
    std::string protocol;
    size_t      packetLength{0};
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint8_t> rawData;

public:
    CapturedPacket();
    CapturedPacket(std::string srcIP, std::string dstIP,
                   uint16_t srcPort, uint16_t dstPort,
                   std::string proto, size_t len);
    CapturedPacket(const CapturedPacket& other);
    CapturedPacket& operator=(CapturedPacket other);
    virtual ~CapturedPacket();

    friend void swap(CapturedPacket& a, CapturedPacket& b) noexcept;

    [[nodiscard]] virtual std::unique_ptr<CapturedPacket> clone() const;


    [[nodiscard]] const std::string& getSourceIP()   const { return sourceIP; }
    [[nodiscard]] const std::string& getDestIP()     const { return destIP; }
    [[nodiscard]] uint16_t           getSourcePort() const { return sourcePort; }
    [[nodiscard]] uint16_t           getDestPort()   const { return destPort; }
    [[nodiscard]] const std::string& getProtocol()   const { return protocol; }
    [[nodiscard]] size_t             getPacketLength() const { return packetLength; }
    [[nodiscard]] std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    [[nodiscard]] const std::vector<uint8_t>& getRawData() const { return rawData; }


    void setSourceIP(std::string ip)   { sourceIP   = std::move(ip); }
    void setDestIP(std::string ip)     { destIP     = std::move(ip); }
    void setSourcePort(uint16_t port)  { sourcePort = port; }
    void setDestPort(uint16_t port)    { destPort   = port; }
    void setProtocol(std::string p)    { protocol   = std::move(p); }
    void setPacketLength(size_t len)   { packetLength = len; }
    void setTimestamp(std::chrono::system_clock::time_point ts) { timestamp = ts; }
    void setRawData(std::vector<uint8_t> d) { rawData = std::move(d); }

    [[nodiscard]] virtual std::string getType()   const;
    [[nodiscard]] virtual std::string toString()  const;

    friend std::ostream& operator<<(std::ostream& os, const CapturedPacket& p) {
        os << p.toString();
        return os;
    }
};



class HTTPCapturedPacket : public CapturedPacket {
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
    std::string userAgent;
    std::string contentType;
    int         statusCode{0};
    std::string statusMessage;
    size_t      contentLength{0};
    bool        isRequest{true};

public:
    HTTPCapturedPacket();
    HTTPCapturedPacket(std::string method_, std::string uri_, std::string host_ = "");

    [[nodiscard]] std::unique_ptr<CapturedPacket> clone() const override;
    [[nodiscard]] std::string getType()   const override;
    [[nodiscard]] std::string toString()  const override;
    [[nodiscard]] bool isSuccessful()     const;

    [[nodiscard]] const std::string& getMethod()  const { return method; }
    [[nodiscard]] const std::string& getUri()     const { return uri; }
    [[nodiscard]] const std::string& getHost()    const { return host; }
    [[nodiscard]] int  getStatusCode()            const { return statusCode; }
    [[nodiscard]] bool getIsRequest()             const { return isRequest; }

    void setMethod(std::string m)        { method        = std::move(m); }
    void setUri(std::string u)           { uri           = std::move(u); }
    void setVersion(std::string v)       { version       = std::move(v); }
    void setHost(std::string h)          { host          = std::move(h); }
    void setUserAgent(std::string ua)    { userAgent     = std::move(ua); }
    void setContentType(std::string ct)  { contentType   = std::move(ct); }
    void setStatusCode(int code)         { statusCode    = code; }
    void setStatusMessage(std::string m) { statusMessage = std::move(m); }
    void setContentLength(size_t len)    { contentLength = len; }
    void setIsRequest(bool req)          { isRequest     = req; }
};

class DNSCapturedPacket : public CapturedPacket {
    uint16_t    transactionId{0};
    std::string queryName;
    std::string queryType;
    std::vector<std::string> answers;
    bool        isQuery{true};
    uint16_t    queryCount{0};
    uint16_t    answerCount{0};

public:
    DNSCapturedPacket();
    DNSCapturedPacket(uint16_t transId, bool isQuery_,
                      std::string queryName_, std::string queryType_ = "A");

    [[nodiscard]] std::unique_ptr<CapturedPacket> clone() const override;
    [[nodiscard]] std::string getType()   const override;
    [[nodiscard]] std::string toString()  const override;

    [[nodiscard]] uint16_t    getTransactionId()  const { return transactionId; }
    [[nodiscard]] const std::string& getQueryName() const { return queryName; }
    [[nodiscard]] const std::string& getQueryType() const { return queryType; }
    [[nodiscard]] const std::vector<std::string>& getAnswers() const { return answers; }
    [[nodiscard]] bool     getIsQuery()    const { return isQuery; }
    [[nodiscard]] uint16_t getQueryCount() const { return queryCount; }
    [[nodiscard]] uint16_t getAnswerCount() const { return answerCount; }

    void setTransactionId(uint16_t id)   { transactionId = id; }
    void setQueryName(std::string n)     { queryName = std::move(n); }
    void setQueryType(std::string t)     { queryType = std::move(t); }
    void addAnswer(const std::string& a) { answers.push_back(a); }
    void setIsQuery(bool q)              { isQuery = q; }
    void setQueryCount(uint16_t c)       { queryCount = c; }
    void setAnswerCount(uint16_t c)      { answerCount = c; }
};

class TCPCapturedPacket : public CapturedPacket {
    uint32_t sequenceNumber{0};
    uint32_t ackNumber{0};
    uint16_t windowSize{0};
    bool syn{false}, ack{false}, fin{false}, rst{false}, psh{false}, urg{false};
    size_t payloadLength{0};

public:
    TCPCapturedPacket();
    TCPCapturedPacket(uint32_t seqNum, uint32_t ackNum, uint16_t winSize);

    [[nodiscard]] std::unique_ptr<CapturedPacket> clone() const override;
    [[nodiscard]] std::string getType()   const override;
    [[nodiscard]] std::string toString()  const override;
    [[nodiscard]] std::string getFlagsString()          const;
    [[nodiscard]] bool isConnectionEstablishment()      const;
    [[nodiscard]] bool isConnectionEstablishmentAck()   const;
    [[nodiscard]] bool isConnectionTermination()        const;

    [[nodiscard]] uint32_t getSequenceNumber() const { return sequenceNumber; }
    [[nodiscard]] uint32_t getAckNumber()      const { return ackNumber; }
    [[nodiscard]] uint16_t getWindowSize()     const { return windowSize; }
    [[nodiscard]] size_t   getPayloadLength()  const { return payloadLength; }
    [[nodiscard]] bool getSyn() const { return syn; }
    [[nodiscard]] bool getAck() const { return ack; }
    [[nodiscard]] bool getFin() const { return fin; }

    void setSequenceNumber(uint32_t s) { sequenceNumber = s; }
    void setAckNumber(uint32_t a)      { ackNumber      = a; }
    void setWindowSize(uint16_t w)     { windowSize     = w; }
    void setPayloadLength(size_t l)    { payloadLength  = l; }
    void setFlags(bool s, bool a, bool f, bool r, bool p, bool u) {
        syn = s; ack = a; fin = f; rst = r; psh = p; urg = u;
    }
};

class UDPCapturedPacket : public CapturedPacket {
    size_t payloadLength{0};

public:
    UDPCapturedPacket();
    explicit UDPCapturedPacket(size_t payloadLen_);

    [[nodiscard]] std::unique_ptr<CapturedPacket> clone() const override;
    [[nodiscard]] std::string getType()  const override;
    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] size_t getPayloadLength() const { return payloadLength; }
    void setPayloadLength(size_t l) { payloadLength = l; }
};

class ICMPCapturedPacket : public CapturedPacket {
    uint8_t     type{0};
    uint8_t     code{0};
    std::string typeDescription;

public:
    ICMPCapturedPacket();
    ICMPCapturedPacket(uint8_t type_, uint8_t code_, std::string desc_ = "");

    [[nodiscard]] std::unique_ptr<CapturedPacket> clone() const override;
    [[nodiscard]] std::string getType()  const override;
    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] uint8_t     getIcmpType()        const { return type; }
    [[nodiscard]] uint8_t     getIcmpCode()        const { return code; }
    [[nodiscard]] const std::string& getTypeDescription() const { return typeDescription; }

    void setIcmpType(uint8_t t)          { type            = t; }
    void setIcmpCode(uint8_t c)          { code            = c; }
    void setTypeDescription(std::string d) { typeDescription = std::move(d); }
};


class CaptureStatistics {
    size_t totalPackets{0};
    size_t tcpPackets{0};
    size_t udpPackets{0};
    size_t httpPackets{0};
    size_t dnsPackets{0};
    size_t icmpPackets{0};
    size_t otherPackets{0};
    size_t totalBytes{0};
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastPacketTime;

public:
    CaptureStatistics();
    explicit CaptureStatistics(std::chrono::system_clock::time_point start);

    void update(const CapturedPacket& packet);

    [[nodiscard]] double getPacketsPerSecond() const;
    [[nodiscard]] double getBytesPerSecond()   const;
    [[nodiscard]] std::string toString()       const;

    [[nodiscard]] size_t getTotalPackets() const { return totalPackets; }
    [[nodiscard]] size_t getTotalBytes()   const { return totalBytes; }
    [[nodiscard]] size_t getTcpPackets()   const { return tcpPackets; }
    [[nodiscard]] size_t getUdpPackets()   const { return udpPackets; }
    [[nodiscard]] size_t getHttpPackets()  const { return httpPackets; }
    [[nodiscard]] size_t getDnsPackets()   const { return dnsPackets; }
    [[nodiscard]] size_t getIcmpPackets()  const { return icmpPackets; }

    friend std::ostream& operator<<(std::ostream& os, const CaptureStatistics& s) {
        os << s.toString();
        return os;
    }
};


template<typename PacketType>
std::vector<std::shared_ptr<PacketType>> filterByType(
    const std::vector<std::shared_ptr<CapturedPacket>>& packets)
{
    std::vector<std::shared_ptr<PacketType>> result;
    for (const auto& p : packets) {
        if (auto typed = std::dynamic_pointer_cast<PacketType>(p)) {
            result.push_back(typed);
        }
    }
    return result;
}

#endif
