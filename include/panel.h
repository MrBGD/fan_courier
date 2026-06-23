#pragma once
#ifndef ADMINISTRATION_PANEL_H
#define ADMINISTRATION_PANEL_H

#include <iostream>
#include <memory>
#include <string>
#include "Exceptions.h"


class Panel {
protected:
    bool is_running = false;

    Panel();
    explicit Panel(bool run);

private:
    static int totalPanelsCreated;

    // NVI hooks: every derived panel customizes *what* gets printed,
    // never *how* the stream operator is wired up.
    [[nodiscard]] virtual std::string panelLabel() const = 0;
    virtual void printDetails(std::ostream& os) const = 0;

public:
    virtual ~Panel() = default;
    Panel(const Panel&) = default;
    Panel& operator=(const Panel&) = default;

    void shutdown();
    // cppcheck-suppress unusedFunction
    [[nodiscard]] bool isRunning() const { return is_running; }

    // Theme-specific action every concrete panel must provide.
    virtual void execute() = 0;

    [[nodiscard]] virtual std::unique_ptr<Panel> clone() const = 0;

    // Public, non-virtual entry point for display (Non-Virtual Interface idiom).
    void display(std::ostream& os) const;

    [[nodiscard]] static int getTotalPanelsCreated() { return totalPanelsCreated; }

    friend std::ostream& operator<<(std::ostream& os, const Panel& p) {
        p.display(os);
        return os;
    }
};

#endif
