#pragma once

#include "utils.hpp"

namespace screen
{
    extern int CELL;

    void cls();
    void setMonitorColor(char Color);

    void printString(char *cA);
    void printChar(char c);

    void scroll();

    void printColorString(char *c, char co);
    void printColorChar(char c, char co);
    void blink();
    void printBuffer(char* buffer, int size);
}