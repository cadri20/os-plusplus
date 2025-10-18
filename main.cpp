#include "utils.hpp"
#include "screen.hpp"

#define PIC1_C 0x20
#define PIC1_D 0x21
#define PIC2_C 0xa0
#define PIC2_D 0xa1

#define ICW1_DEF 0x10
#define ICW1_ICW4 0x01
#define ICW4_x86 0x01

void scroll();

void initIDT();
extern "C" void loadIdt();
extern "C" void isr1_handler();
void handleKeypress(int);
void pressed(char);
void picRemap();

unsigned char inportb(unsigned short);
void outportb(unsigned short , unsigned char);

char COMMAND[21];
int i = 0;

struct IDT_ENTRY{
    unsigned short base_Lower;
    unsigned short selector;
    unsigned char zero;
    unsigned char flags;
    unsigned short base_Higher;
};

struct IDT_ENTRY idt[256];
extern unsigned int isr1;
unsigned int base;

extern "C" void start(){
	utils::TM_START = (char*) 0xb8000;
	base = (unsigned int)&isr1;

	screen::cls();
	screen::setMonitorColor(0xa5);

	char Welcome[] = "Welcome To OS0 : Copyright 2021\n";
	char Welcome2[] = "Command Line Version 1.0.0.0\n\n";
	char OSM[] = "OS0 > ";

	screen::printString(Welcome);
	screen::printString(Welcome2);
	screen::printColorString(OSM , 0xa8);

	initIDT();
}

void initIDT(){
	idt[1].base_Lower = (base & 0xFFFF);
	idt[1].base_Higher = (base >> 16) & 0xFFFF;
	idt[1].selector = 0x08;
	idt[1].zero = 0;
	idt[1].flags = 0x8e;

	picRemap();

	outportb(0x21 , 0xfd);
	outportb(0xa1 , 0xff);

	loadIdt();
}

unsigned char inportb(unsigned short _port){
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(unsigned short _port, unsigned char _data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

extern "C" void isr1_handler(){
	handleKeypress(inportb(0x60));
	outportb(0x20 , 0x20);
	outportb(0xa0 , 0x20);
}

void handleKeypress(int code){
	char OSM[] = "\nOS0 > ";
	char Scancode[] = {
		0 , 0 , '1' , '2' ,
		'3' , '4' , '5' , '6' , 
		'7' , '8' , '9' , '0' , 
		'-' , '=' , 0 , 0 , 'Q' , 
		'W' , 'E' , 'R' , 'T' , 'Y' ,
		'U' , 'I' , 'O' , 'P' , '[' , ']' , 
		0 , 0 , 'A' , 'S' , 'D' , 'F' , 'G' , 
		'H' , 'J' , 'K' , 'L' , ';' , '\'' , '`' , 
		0 , '\\' , 'Z' , 'X' , 'C' , 'V' , 'B' , 'N' , 'M' ,
		',' , '.' , '/' , 0 , '*' , 0 , ' '
	};
	
	if(code == 0x1c){
		COMMAND[i] = '\0';
		i = 0;
		utils::strEval(COMMAND);
		screen::printString(OSM);
	}
	else if(code < 0x3a)
		pressed(Scancode[code]);
}

void pressed(char key){
	if(i != 20){
		COMMAND[i] = key;
		i++;
		screen::printChar(key);
	}
	else{
		screen::blink();
	}
}

void picRemap(){
	unsigned char a , b;
	a = inportb(PIC1_D);
	b = inportb(PIC2_D);

	outportb(PIC1_C , ICW1_DEF | ICW1_ICW4);
	outportb(PIC2_C , ICW1_DEF | ICW1_ICW4);

	outportb(PIC1_D , 0);
	outportb(PIC2_D , 8);

	outportb(PIC1_D , 4);
	outportb(PIC2_D , 2);

	outportb(PIC1_D , ICW4_x86);
	outportb(PIC2_D , ICW4_x86);

	outportb(PIC1_D , a);
	outportb(PIC2_D , b);

}