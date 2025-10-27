#pragma once


namespace utils {

    // Video demo / infinite writer
    void vid();

    // Disk IO wrappers
    void put();
    void get();

    // Low-level C-linked IO (implemented elsewhere)
    extern "C" void read();
    extern "C" void write();

    // Utilities
    void blink();
    char strcmp(char* sou, char* dest);
    void strEval(char* textEntry);
    void getCommand(char* entry, char* output);
    void getParameter(char* entry, char* output);
    int toInt(char* text);

    // Shared data (defined in utils.cpp)
    extern char* TM_START;
}