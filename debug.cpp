extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" char read_port(unsigned short port);

namespace debug{
    void serial_init(){
        write_port(0x3F8 + 1, 0x00);    // Disable all interrupts
        write_port(0x3F8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        write_port(0x3F8 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        write_port(0x3F8 + 1, 0x00);    //                  (hi byte)
        write_port(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
        write_port(0x3F8 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        write_port(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    }

    int is_transmit_empty() {
        return read_port(0x3F8 + 5) & 0x20;
    }

    void serial_write_char(char a) {
        while (!is_transmit_empty());
        write_port(0x3F8, a);
    }

    void serial_write_string(const char* str) {
        while (*str) {
            serial_write_char(*str++);
        }
    }

    void log (const char* str, int size){
        for (int i = 0; i < size; i++){
            serial_write_char(str[i]);
        }
    }
}