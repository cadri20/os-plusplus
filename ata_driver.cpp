#include "ata_driver.hpp"

extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" char read_port(unsigned short port);

namespace ata_driver
{
    void write_buffer(unsigned int lba, char *buffer, unsigned int sector_count)
    {
        unsigned char status;

    // --- Configurar dirección LBA ---
    write_port(0x1F6, 0xE0 | ((lba >> 24) & 0x0F)); // drive + modo LBA + bits 24–27 del LBA

    // Número de sectores a escribir
    write_port(0x1F2, sector_count);

    // Bits 0–7 del LBA
    write_port(0x1F3, (unsigned char)(lba & 0xFF));

    // Bits 8–15 del LBA
    write_port(0x1F4, (unsigned char)((lba >> 8) & 0xFF));

    // Bits 16–23 del LBA
    write_port(0x1F5, (unsigned char)((lba >> 16) & 0xFF));

    // Comando: WRITE SECTORS (0x30)
    write_port(0x1F7, 0x30);

    // --- Para cada sector a escribir ---
    for (int s = 0; s < sector_count; s++) {

        // Esperar hasta que el disco esté listo (DRQ=1 y BSY=0)
        do {
            status = read_port(0x1F7);
        } while (!(status & 0x08)); // esperar a que DRQ esté activo

        // --- Escribir los 256 words (512 bytes) del sector ---
        for (int i = 0; i < 256; i++) {
            unsigned short data = buffer[s * 256 + i];
            __asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(0x1F0));
        }
    }

    // --- Esperar a que el disco termine ---
    do {
        status = read_port(0x1F7);
    } while (status & 0x80); // esperar a que BSY=0
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
        while (read_port(0x1F7) & 0x80)
            ; // Esperar BSY = 0

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
