#include "ata_driver.hpp"
#include "debug.hpp"
#include "utils.hpp"

extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" char read_port(unsigned short port);
extern "C" void fake_outsw(unsigned short port, unsigned int addr, void* count);

namespace ata_driver
{
static inline void rep_insw(unsigned short port, unsigned int length, void *dest) {
  asm volatile ("cld\n"
      "rep insw\n" :
   : "c" (length),
     "d" (port),
     "D" (dest)
   : "memory", "cc");
}

    void ata_cache_flush()
    {
        // Comando FLUSH CACHE (0xE7)
        write_port(0x1F7, 0xE7);
    }

    void write_buffer(unsigned int lba, void *buffer, unsigned int sector_count)
    {
        unsigned char status;

        write_port(0x1F6, 0xE0 | ((lba >> 24) & 0x0F)); // drive + modo LBA + bits 24–27 del LBA
        write_port(0x1F2, sector_count);

        write_port(0x1F3, (unsigned char)(lba & 0xFF));
        write_port(0x1F4, (unsigned char)((lba >> 8) & 0xFF));
        write_port(0x1F5, (unsigned char)((lba >> 16) & 0xFF));

        write_port(0x1F7, 0x30);

        for (int s = 0; s < sector_count; s++)
        {

            for (; ((status & 0x88) != 0x08) && !(status & 1); status = read_port(0x1f7));

            if(status & 1)
            {
                // Error
                char errorMsg[] = "Error occurred while writing to disk\n";
                debug::serial_write_string(errorMsg);
                char sectorNum[10];
                utils::toString(sector_count, sectorNum);
                debug::serial_write_string("Sector: ");
                debug::serial_write_string(sectorNum);
                return;
            }


            fake_outsw(0x1F0, 256, buffer + (s << 9));
        }

        ata_cache_flush();

    }

    int read_sectors(int lba, int sectors, char *dst)
    {
        if (lba & 0xF0000000)
            return -1; // overflow LBA28

        if (sectors == 0)
            return -2;

        unsigned char head = (unsigned char)((lba >> 24) & 0x0F);
        unsigned char drive_head = (unsigned char)(0xE0 | head); // master
        write_port(0x1F6, drive_head);

        write_port(0x1F2, sectors);

        write_port(0x1F3, (unsigned char)(lba & 0xFF));         // LBA 0..7
        write_port(0x1F4, (unsigned char)((lba >> 8) & 0xFF));  // LBA 8..15
        write_port(0x1F5, (unsigned char)((lba >> 16) & 0xFF)); // LBA 16..23

        while (read_port(0x1F7) & 0x80);

        write_port(0x1F7, 0x20);

        for(unsigned int i = 0; i < sectors; i++){
            while(read_port(0x1F7) & 0x80);

            rep_insw(0x1f0, 256, dst + i * 512);
        }

        return 0;
    }

    
}
