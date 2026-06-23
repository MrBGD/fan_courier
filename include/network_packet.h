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
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getDestIP()     const { return destIP; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint16_t           getSourcePort() const { return sourcePort; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint16_t           getDestPort()   const { return destPort; }
    [[nodiscard]] const std::string& getProtocol()   const { return protocol; }
    [[nodiscard]] size_t             getPacketLength() const { return packetLength; }
    [[nodiscard]] std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::vector<uint8_t>& getRawData() const { return rawData; }


    void setSourceIP(std::string ip)   { sourceIP   = std::move(ip); }
    void setDestIP(std::string ip)     { destIP     = std::move(ip); }
    // cppcheck-suppress unusedFunction
    void setSourcePort(uint16_t port)  { sourcePort = port; }
    // cppcheck-suppress unusedFunction
    void setDestPort(uint16_t port)    { destPort   = port; }
    void setProtocol(std::string p)    { protocol   = std::move(p); }
    void setPacketLength(size_t len)   { packetLength = len; }
    void setTimestamp(std::chrono::system_clock::time_point ts) { timestamp = ts; }
    // cppcheck-suppress unusedFunction
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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getMethod()  const { return method; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getUri()     const { return uri; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getHost()    const { return host; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] int  getStatusCode()            const { return statusCode; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] bool getIsRequest()             const { return isRequest; }

    // cppcheck-suppress unusedFunction
    void setMethod(std::string m)        { method        = std::move(m); }
    // cppcheck-suppress unusedFunction
    void setUri(std::string u)           { uri           = std::move(u); }
    // cppcheck-suppress unusedFunction
    void setVersion(std::string v)       { version       = std::move(v); }
    // cppcheck-suppress unusedFunction
    void setHost(std::string h)          { host          = std::move(h); }
    // cppcheck-suppress unusedFunction
    void setUserAgent(std::string ua)    { userAgent     = std::move(ua); }
    // cppcheck-suppress unusedFunction
    void setContentType(std::string ct)  { contentType   = std::move(ct); }
    // cppcheck-suppress unusedFunction
    void setStatusCode(int code)         { statusCode    = code; }
    // cppcheck-suppress unusedFunction
    void setStatusMessage(std::string m) { statusMessage = std::move(m); }
    // cppcheck-suppress unusedFunction
    void setContentLength(size_t len)    { contentLength = len; }
    // cppcheck-suppress unusedFunction
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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint16_t    getTransactionId()  const { return transactionId; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getQueryName() const { return queryName; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getQueryType() const { return queryType; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::vector<std::string>& getAnswers() const { return answers; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] bool     getIsQuery()    const { return isQuery; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint16_t getQueryCount() const { return queryCount; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint16_t getAnswerCount() const { return answerCount; }

    // cppcheck-suppress unusedFunction
    void setTransactionId(uint16_t id)   { transactionId = id; }
    // cppcheck-suppress unusedFunction
    void setQueryName(std::string n)     { queryName = std::move(n); }
    // cppcheck-suppress unusedFunction
    void setQueryType(std::string t)     { queryType = std::move(t); }
    // cppcheck-suppress unusedFunction
    void addAnswer(const std::string& a) { answers.push_back(a); }
    // cppcheck-suppress unusedFunction
    void setIsQuery(bool q)              { isQuery = q; }
    // cppcheck-suppress unusedFunction
    void setQueryCount(uint16_t c)       { queryCount = c; }
    // cppcheck-suppress unusedFunction
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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint32_t getSequenceNumber() const { return sequenceNumber; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint32_t getAckNumber()      const { return ackNumber; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint16_t getWindowSize()     const { return windowSize; }
    [[nodiscard]] size_t   getPayloadLength()  const { return payloadLength; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] bool getSyn() const { return syn; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] bool getAck() const { return ack; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] bool getFin() const { return fin; }

    // cppcheck-suppress unusedFunction
    void setSequenceNumber(uint32_t s) { sequenceNumber = s; }
    // cppcheck-suppress unusedFunction
    void setAckNumber(uint32_t a)      { ackNumber      = a; }
    // cppcheck-suppress unusedFunction
    void setWindowSize(uint16_t w)     { windowSize     = w; }
    void setPayloadLength(size_t l)    { payloadLength  = l; }
    // cppcheck-suppress unusedFunction
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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getPayloadLength() const { return payloadLength; }
    // cppcheck-suppress unusedFunction
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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint8_t     getIcmpType()        const { return type; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] uint8_t     getIcmpCode()        const { return code; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getTypeDescription() const { return typeDescription; }

    // cppcheck-suppress unusedFunction
    void setIcmpType(uint8_t t)          { type            = t; }
    // cppcheck-suppress unusedFunction
    void setIcmpCode(uint8_t c)          { code            = c; }
    // cppcheck-suppress unusedFunction
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

    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getTotalPackets() const { return totalPackets; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getTotalBytes()   const { return totalBytes; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getTcpPackets()   const { return tcpPackets; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getUdpPackets()   const { return udpPackets; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getHttpPackets()  const { return httpPackets; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] size_t getDnsPackets()   const { return dnsPackets; }
    // cppcheck-suppress unusedFunction
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
