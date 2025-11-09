#pragma once

extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" char read_port(unsigned short port);

namespace debug {
    int is_transmit_empty();
    void serial_write_char(char a);
    void serial_write_string(const char* str);
    void log (const char* str, int size);
    void serial_init();
}