set -e
mkdir -p bin

echo "Compilando ensambladores"
nasm boot.asm -f bin -o bin/bootsect.bin
nasm kernel_entry.asm -f elf -o bin/kernel_entry.o
nasm idt.asm -f elf -o bin/idt.o
nasm ata.asm -f elf -o bin/ata.o


echo "Compilando kernel en C"
g++ -m32 -ffreestanding -c main.cpp -o bin/kernel.o
g++ -m32 -ffreestanding -c utils.cpp -o bin/utils.o
g++ -m32 -ffreestanding -c screen.cpp -o bin/screen.o
g++ -m32 -ffreestanding -c ata_driver.cpp -o bin/ata_driver.o
g++ -m32 -ffreestanding -c debug.cpp -o bin/debug.o
echo "Enlazando todo"
ld -m elf_i386 -o bin/kernel.elf -Ttext 0x1000 bin/kernel_entry.o bin/kernel.o bin/utils.o bin/screen.o bin/ata_driver.o bin/idt.o bin/ata.o bin/debug.o
objcopy -O binary -j .text bin/kernel.elf bin/kernel.bin
cat bin/bootsect.bin bin/kernel.bin > bin/os-image.bin
qemu-system-i386 -drive format=raw,file=bin/os-image.bin,snapshot=off,cache=writeback -serial stdio