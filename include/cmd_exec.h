#pragma once
#ifndef ADMINISTRATION_PANEL_CMD_EXEC_H
#define ADMINISTRATION_PANEL_CMD_EXEC_H

#include "panel.h"
#include "network_packet.h"
#include <memory>
#include <string>
#include <vector>

// Fourth class derived from Panel, added on top of the existing
// Login / Dashboard / ReportPanel trio without altering them.

class CmdExec : public Panel {
    std::string                                  command;
    std::vector<std::shared_ptr<CapturedPacket>> targetPackets;
    std::string                                  lastResult;
    int                                           executionCount{0};

    [[nodiscard]] std::string panelLabel() const override { return "CmdExec"; }
    void printDetails(std::ostream& os) const override;

    [[nodiscard]] std::string runCount(const std::string& protocolFilter) const;
    [[nodiscard]] std::string runList(const std::string& protocolFilter) const;

public:
    explicit CmdExec(std::string cmd, std::vector<std::shared_ptr<CapturedPacket>> packets = {});
    ~CmdExec() override = default;

    void execute() override;
    [[nodiscard]] std::unique_ptr<Panel> clone() const override;

    // cppcheck-suppress unusedFunction
    [[nodiscard]] const std::string& getLastResult()     const { return lastResult; }
    // cppcheck-suppress unusedFunction
    [[nodiscard]] int                getExecutionCount() const { return executionCount; }
};

#endif
