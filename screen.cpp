#include "screen.hpp"
#include "utils.hpp"

namespace screen
{
    int CELL = 0;

    void cls()
    {
        int i = 0;
        CELL = 0;
        while (i < (2 * 80 * 25))
        {
            *(utils::TM_START + i) = ' '; // Clear screen
            i += 2;
        }
    }

    void setMonitorColor(char Color)
    {
        int i = 1;
        while (i < (2 * 80 * 25))
        {
            *(utils::TM_START + i) = Color;
            i += 2;
        }
    }

    void printString(char *cA)
    {
        int i = 0;
        while (*(cA + i) != '\0')
        {
            printChar(*(cA + i));
            i++;
        }
    }

    void printChar(char c)
    {
        if (CELL == 2 * 80 * 25)
            scroll();
        if (c == '\n')
        {
            CELL = ((CELL + 160) - (CELL % 160));
            return;
        }
        *(utils::TM_START + CELL) = c;
        CELL += 2;
    }

    void scroll()
    {
        int i = 160, y = 0;
        while (i < 2 * 80 * 25)
        {
            *(utils::TM_START + y) = *(utils::TM_START + i);
            i += 2;
            y += 2;
        }
        CELL = 2 * 80 * 24;
        i = 0;
        while (i < 160)
        {
            *(utils::TM_START + CELL + i) = ' ';
            i += 2;
        }
    }

    void printColorString(char *c, char co)
    {
        int i = 0;
        while (*(c + i) != '\0')
        {
            printColorChar(*(c + i), co);
            i++;
        }
    }

    void printColorChar(char c, char co)
    {
        if (CELL == 2 * 80 * 25)
            scroll();
        if (c == '\n')
        {
            CELL = ((CELL + 160) - (CELL % 160));
            return;
        }
        *(utils::TM_START + CELL) = c;
        *(utils::TM_START + CELL + 1) = co;
        CELL += 2;
    }

    void blink()
    {
        setMonitorColor(0x59);
        int TIME_OUT = 0x10fffff;
        while (--TIME_OUT)
            ;
        setMonitorColor(0xa5);
    }

    void printBuffer(char* buffer, int size){
        for(int i = 0; i < size; i++){
            printChar(buffer[i]);
        }
    }
}