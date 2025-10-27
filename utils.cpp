#include "ata_driver.hpp"
#include "utils.hpp"
#include "screen.hpp"

int blockAddr;
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

			while (i < 1000)
			{
				At[i] = parameter[0]; // Fill with J
				i++;
			}
			At[i] = '\0'; // Null character

			put(); // Writes to Hard disk

			i = 0;
			while (i < 1000)
			{
				At[i] = 0; // Clears the content
				i++;
				blockAddr++;
			}
		}
		else if (strcmp(CMD, cmd9))
		{
			blockAddr = 0;
			int numSectors = toInt(parameter);
			ata_driver::read_sectors(blockAddr, numSectors, At);
			screen::printBuffer(At, numSectors * 512);
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
		ata_driver::write_buffer(blockAddr, At, 2);
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
}
