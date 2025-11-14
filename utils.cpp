#include "ata_driver.hpp"
#include "utils.hpp"
#include "screen.hpp"
#include "debug.hpp"

int blockAddr = 3;
char At[1024];

namespace utils
{

	extern "C" void read();
	extern "C" void write();

	char *TM_START;

	char strcmp(char *sou, char *dest)
	{
		int i = 0;
		while (*(sou + i) == *(dest + i))
		{
			if (*(sou + i) == 0 && *(dest + i) == 0)
				return 1;
			i++;
		}
		return 0;
	}

	void strEval(char *textEntry)
	{
		char cmd1[] = "CLS";
		char cmd2[] = "COLORA";
		char cmd3[] = "COLORB";
		char cmd4[] = "COLORC";
		char cmd5[] = "COLORDEF";
		char cmd6[] = "VID";
		char cmd7[] = "HI";
		char cmd8[] = "PUT";
		char cmd9[] = "GET";

		char msg1[] = "\nHELLO , HAVE A GOOD JOURNEY LEARNING\n";

		char CMD[21];
		char parameter[21];
		clearBuffer(parameter , 21);
		getCommand(textEntry, CMD);
		getParameter(textEntry, parameter);

		if (strcmp(CMD, cmd1))
			screen::cls();

		else if (strcmp(CMD, cmd2))
			screen::setMonitorColor(0x3c);

		else if (strcmp(CMD, cmd3))
			screen::setMonitorColor(0x5a);

		else if (strcmp(CMD, cmd4))
			screen::setMonitorColor(0x2a);

		else if (strcmp(CMD, cmd5))
			screen::setMonitorColor(0xa5);

		else if (strcmp(CMD, cmd6))
			vid();
		else if (strcmp(CMD, cmd7))
			screen::printString(msg1);
		else if (strcmp(CMD, cmd8))
		{
			int i = 0;

			char buffer[1024];
			while (i < 1024)
			{
				buffer[i] = 'J'; // Fill with J
				i++;
			}
			buffer[i] = '\0'; // Null character

			char message[] = "\nWriting to disk...\n";
			debug::serial_write_string(message);
			debug::serial_write_string(parameter);
			ata_driver::write_buffer(toInt(parameter), (unsigned short*)buffer, 2);// Writes to Hard disk

			i = 0;
		}
		else if (strcmp(CMD, cmd9))
		{
			blockAddr = 3;
			int numSectors = 2;
			ata_driver::read_sectors(toInt(parameter), numSectors, At);
			screen::printString(At);
		}
	}

	void vid()
	{
		char clr = 'A';
		while (1)
		{
			int i = 0;
			while (i < 2 * 80 * 25)
			{
				*(TM_START + i) = clr;
				clr++;
				i++;
			}
		}
	}

	void put()
	{
		
	}

	void get()
	{
		ata_driver::read_sectors(blockAddr, 1, At);
	}

	void getCommand(char* entry, char* output){
		//get text before space
		int i = 0;
		while(entry[i] != ' ' && entry[i] != '\0'){
			output[i] = entry[i];
			i++;
		}	
	}

	void getParameter(char* entry, char* output){
		//get text after space
		int i = 0, j = 0;
		while(entry[i] != ' ' && entry[i] != '\0'){
			i++;
		}
		if(entry[i] == ' ')
			i++;
		while(entry[i] != '\0'){
			output[j] = entry[i];
			i++;
			j++;
		}
	}

	int toInt(char* text){
		int i = 0, result = 0;
		while(text[i] != '\0'){
			result = result * 10 + (text[i] - '0');
			i++;
		}
		return result;
	}

	void toString(int value, char* buffer){
		int i = 0;
		if(value == 0){
			buffer[i++] = '0';
			buffer[i] = '\0';
			return;
		}
		char temp[10];
		int j = 0;
		while(value > 0){
			temp[j++] = (value % 10) + '0';
			value /= 10;
		}
		j--;
		while(j >= 0){
			buffer[i++] = temp[j--];
		}
		buffer[i] = '\0';
	}

	void clearBuffer(char* str, int size){
		for(int i = 0; i < size; i++){
			str[i] = '\0';
		}
	}
}
