#include "ata_driver.hpp"

extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" char read_port(unsigned short port);

namespace ata_driver
{
    void write_buffer(int sector, char *buffer)
    {
        unsigned short sec = sector;
        unsigned short cyl = sector / 36;
        unsigned short head = (sector / 18) % 2;
        unsigned short sect = (sector % 18) + 1;

        write_port(0x1F2, 1);                    // Number of sectors to write
        write_port(0x1F3, sect & 0xFF);          // Sector number
        write_port(0x1F4, cyl & 0xFF);           // Cylinder low byte
        write_port(0x1F5, (cyl >> 8) & 0xFF);    // Cylinder high byte
        write_port(0x1F6, (head & 0x0F) | 0xE0); // Drive and head
        write_port(0x1F7, 0x30);                 // Command: Write sectors

        // Wait for the drive to be ready
        while ((read_port(0x1F7) & 0x08) == 0)
            ;

        // Write data
        for (int i = 0; i < 512; i++)
        {
            write_port(0x1F0, buffer[i]);
        }
    }

    int read_sectors(int lba, int sectors, char *dst)
    {
        if (lba & 0xF0000000)
            return -1; // overflow LBA28

        if (sectors == 0)
            return -2;

        // Seleccionar drive/head: 0xE0 = 1110 0000b (LBA mode, master)
        unsigned char head = (unsigned char)((lba >> 24) & 0x0F);
        unsigned char drive_head = (unsigned char)(0xE0 | head); // master
        write_port(0x1F6, drive_head);

        // Numero de sectores (CL)
        write_port(0x1F2, sectors);

        // LBA low/mid/high (bytes 0..23)
        write_port(0x1F3, (unsigned char)(lba & 0xFF));         // LBA 0..7
        write_port(0x1F4, (unsigned char)((lba >> 8) & 0xFF));  // LBA 8..15
        write_port(0x1F5, (unsigned char)((lba >> 16) & 0xFF)); // LBA 16..23

        // Comando READ SECTORS with retry (0x20)
        write_port(0x1F7, 0x20);

        // Polling: esperar DRQ (bit 3) = 1 y BSY = 0
        while(read_port(0x1F7) & 0x80); // Esperar BSY = 0

        // Transferir: cada sector = 256 palabras (16-bit)
        // Usamos operaciones de 8bit (read_port) dos veces para formar la palabra
        unsigned short *dst_words = (unsigned short *)dst;
        unsigned int total_words = 256u * (unsigned int)sectors;
        for (unsigned int i = 0; i < total_words; ++i)
        {
            unsigned char low = read_port(0x1F0);  // baja
            unsigned char high = read_port(0x1F0); // alta
            dst_words[i] = (unsigned short)low | ((unsigned short)high << 8);
        }

        return 0;
    }
}
