/*Mega Man 2 (Hirotomo Nakamura - Giraffe Soft) (GB/GBC) to MIDI converter*/
/*Also works with other games with audio by Giraffe Soft*/
/*By Will Trowbridge*/
/*Portions based on code by ValleyBell*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define bankSize 16384

FILE* rom, * txt;
long bank;
long offset;
long tablePtrLoc;
long tableOffset;
long sfxPtrLoc;
long sfxOffset;
int i, j;
char outfile[1000000];
int format = 0;
int cmdFmt = 0;
int songNum;
long seqPtrs[4];
long songPtr;
long bankAmt;
int seqDiff = 0;
int foundTable = 0;
int songTempo = 0;
int highestSeq = 0;

unsigned static char* romData;

const char TableFind1[5] = { 0x87, 0x5F, 0x16, 0x00, 0x21 };
const char TableFind2[7] = { 0x87, 0xCF, 0x2A, 0x66, 0x6F, 0xAF, 0xE0 };
const char TableFind3[5] = { 0x17, 0x5F, 0x16, 0x00, 0x21 };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
void song2txt(int songNum, long ptrs[4]);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

int main(int args, char* argv[])
{
	printf("Mega Man 2 (Hirotomo Nakamura - Giraffe Soft) (GB/GBC) to TXT converter\n");
	if (args != 3)
	{
		printf("Usage: MM22TXT <rom> <bank>\n");
		return -1;
	}
	else
	{
		if ((rom = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			if ((rom = fopen(argv[1], "rb")) == NULL)
			{
				printf("ERROR: Unable to open file %s!\n", argv[1]);
				exit(1);
			}
			else
			{
				if ((rom = fopen(argv[1], "rb")) == NULL)
				{
					printf("ERROR: Unable to open file %s!\n", argv[1]);
					exit(1);
				}
				else
				{
					bank = strtol(argv[2], NULL, 16);
					if (bank != 1)
					{
						bankAmt = bankSize;
					}
					else
					{
						bankAmt = 0;
					}
				}

				if (bank != 1)
				{
					fseek(rom, ((bank - 1) * bankSize), SEEK_SET);
					romData = (unsigned char*)malloc(bankSize);
					fread(romData, 1, bankSize, rom);
					fclose(rom);
				}

				else
				{
					fseek(rom, ((bank - 1) * bankSize * 2), SEEK_SET);
					romData = (unsigned char*)malloc(bankSize * 2);
					fread(romData, 1, bankSize * 2, rom);
					fclose(rom);
				}

				/*Try to search the bank for song table loader*/
				for (i = 0; i < bankSize; i++)
				{
					if ((!memcmp(&romData[i], TableFind1, 5)))
					{
						if (foundTable == 0)
						{
							tablePtrLoc = bankAmt + i + 5;
							printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
							tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
							printf("Song table starts at 0x%04x...\n", tableOffset);
							foundTable = 1;
						}
						else if (foundTable == 1)
						{
							sfxPtrLoc = bankAmt + i + 5;
							printf("Found pointer to sound effects table at address 0x%04x!\n", sfxPtrLoc);
							sfxOffset = ReadLE16(&romData[sfxPtrLoc - bankAmt]);
							printf("Sound effects table starts at 0x%04x...\n", sfxOffset);
						}
					}
				}


				/*Alternate method - Ayakashi no Shiro*/
				for (i = 0; i < bankSize; i++)
				{
					if ((!memcmp(&romData[i], TableFind2, 7)) && foundTable == 0)
					{
						tablePtrLoc = bankAmt + i - 2;
						printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
						printf("Song table starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						break;
					}
				}

				/*Alternate method - Rentaiou/The Shinri Game 1/2*/
				for (i = 0; i < bankSize; i++)
				{
					if ((!memcmp(&romData[i], TableFind3, 5)) && foundTable == 0)
					{
						tablePtrLoc = bankAmt + i + 5;
						printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
						printf("Song table starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
						break;
					}
				}

				i = tableOffset - bankAmt;
				songNum = 1;
				while ((ReadLE16(&romData[i]) < bankSize * 2) && i != (sfxOffset - bankAmt))
				{
					songPtr = ReadLE16(&romData[i]);
					printf("Song %i: 0x%04X\n", songNum, songPtr);

					seqPtrs[0] = ReadLE16(&romData[songPtr - bankAmt]);
					printf("Channel 1: 0x%04X\n", seqPtrs[0]);
					seqPtrs[1] = ReadLE16(&romData[songPtr + 2 - bankAmt]);
					printf("Channel 2: 0x%04X\n", seqPtrs[1]);
					seqPtrs[2] = ReadLE16(&romData[songPtr + 4 - bankAmt]);
					printf("Channel 3: 0x%04X\n", seqPtrs[2]);
					seqPtrs[3] = ReadLE16(&romData[songPtr + 6 - bankAmt]);
					printf("Channel 4: 0x%04X\n", seqPtrs[3]);
					song2txt(songNum, seqPtrs);
					i += 2;
					songNum++;
				}
				printf("The operation was successfully completed!\n");
			}
		}
	}
}

void song2txt(int songNum, long ptrs[4])
{
	sprintf(outfile, "song%i.txt", songNum);
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file seqs.txt!\n");
		exit(2);
	}
	else
	{
		unsigned char command[3];
		int curTrack = 0;
		int curNote = 0;
		int curNoteLen = 0;
		int octave = 0;
		int seqEnd = 0;
		int curPos = 0;
		int tempo = 0;
		long jumpPos = 0;
		long jumpPosEnd = 0;
		unsigned char lowNibble = 0;
		unsigned char highNibble = 0;
		for (curTrack == 0; curTrack < 4; curTrack++)
		{
			seqEnd = 0;
			fprintf(txt, "\nChannel %i: \n", curTrack + 1);
			if (ptrs[curTrack] >= bankSize * 2 || ptrs[curTrack] < bankAmt)
			{
				seqEnd = 1;
			}

			curPos = ptrs[curTrack] - bankAmt;

			while (seqEnd == 0)
			{
				command[0] = romData[curPos];
				command[1] = romData[curPos + 1];
				command[2] = romData[curPos + 2];

				/*Set tempo*/
				if (command[0] == 0xC0)
				{
					tempo = command[1];
					fprintf(txt, "Set tempo: %i\n", tempo);
					curPos += 2;
				}

				/*Set duty*/
				else if (command[0] == 0xC1)
				{
					fprintf(txt, "Set duty: %i\n", command[1]);
					curPos += 2;
				}

				/*Set volume/note size*/
				else if (command[0] == 0xC2)
				{
					lowNibble = (command[1] >> 4);
					highNibble = (command[1] & 15);
					fprintf(txt, "Set volume: %i, set note size: %i\n", lowNibble, highNibble);
					curPos += 2;
				}

				/*Jump to loop position*/
				else if (command[0] == 0xC3)
				{
					jumpPos = ReadLE16(&romData[curPos + 1]);
					fprintf(txt, "Go to loop: 0x%04X\n", jumpPos);
					seqEnd = 1;
				}

				/*Set panning?*/
				else if (command[0] == 0xC4)
				{
					fprintf(txt, "Set panning: %i\n", command[1]);
					curPos += 2;
				}

				/*Exit from CD jump*/
				else if (command[0] == 0xC9)
				{
					fprintf(txt, "Return from jump\n");
					curPos++;
				}

				/*Stop channel?*/
				else if (command[0] == 0xCB)
				{
					fprintf(txt, "Stop channel?\n");
					seqEnd = 1;
				}

				/*Jump to position*/
				else if (command[0] == 0xCD)
				{
					jumpPos = ReadLE16(&romData[curPos + 1]);
					fprintf(txt, "Jump to position: 0x%04X\n", jumpPos);
					curPos += 3;
				}

				/*End sequence*/
				else if (command[0] == 0xCE)
				{
					fprintf(txt, "End sequence\n");
					seqEnd = 1;
				}

				/*Disable channel*/
				else if (command[0] == 0xCF)
				{
					fprintf(txt, "Disable channel\n");
					seqEnd = 1;
				}

				/*Hold note*/
				else if (command[0] >= 0xD0 && command[0] < 0xE0)
				{
					highNibble = (command[0] & 15);
					fprintf(txt, "Hold note: %i\n", highNibble);
					curPos++;
				}

				/*Rest*/
				else if (command[0] >= 0xE0 && command[0] < 0xF0)
				{
					highNibble = (command[0] & 15);
					fprintf(txt, "Rest: %i\n", highNibble);
					curPos++;
				}

				/*Set octave*/
				else if (command[0] >= 0xF0 && command[0] <= 0xFF)
				{
					highNibble = (command[0] & 15);
					octave = highNibble;
					fprintf(txt, "Set octave: %i\n", octave);
					curPos++;
				}

				/*Play note*/
				else if (command[0] < 0xC0)
				{
					lowNibble = (command[0] >> 4);
					highNibble = (command[0] & 15);
					curNote = lowNibble;
					curNoteLen = highNibble;
					fprintf(txt, "Play note: %i, length: %i\n", curNote, curNoteLen);
					curPos++;
				}

				/*Unknown command*/
				else
				{
					fprintf(txt, "Unknown command: %01X\n", command[0]);
					curPos++;
				}
			}
		}
		fclose(txt);
	}
}