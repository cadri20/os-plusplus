#pragma once

extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" char read_port(unsigned short port);

namespace ata_driver
{
    void write_buffer(unsigned int lba, void *buffer, unsigned int sector_count);
    int read_sectors(int lba, int sectors, char *dst);
}