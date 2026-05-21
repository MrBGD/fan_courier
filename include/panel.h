#pragma once
#ifndef ADMINISTRATION_PANEL_H
#define ADMINISTRATION_PANEL_H

#include <iostream>
#include <string>
#include "Exceptions.h"

class Panel {
protected:
    bool is_running=false;


public:
    Panel()=default;
    Panel(bool run):is_running(run){}
    virtual ~Panel() = default;

    void shutdown();

};

#endif