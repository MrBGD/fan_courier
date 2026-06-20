#include "../include/panel.h"

int Panel::totalPanelsCreated = 0;

Panel::Panel() {
    ++totalPanelsCreated;
}

Panel::Panel(bool run) : is_running(run) {
    ++totalPanelsCreated;
}

void Panel::shutdown() {
    is_running = false;
}

void Panel::display(std::ostream& os) const {
    os << "[" << panelLabel() << "] running=" << std::boolalpha << is_running << " | ";
    printDetails(os);
}
