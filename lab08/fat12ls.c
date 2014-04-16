/* fat12ls.c 
 *
 *  Displays the files in the root sector of an MSDOS floppy disk
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#define SIZE 32      /* size of the read buffer */
#define ROOTSIZE 256 /* max size of the root directory */
//define PRINT_HEX   // un-comment this to print the values in hex for debugging

struct BootSector {
	unsigned char sName[9];            // The name of the volume
	unsigned short iBytesSector;        // Bytes per Sector

	unsigned char iSectorsCluster;     // Sectors per Cluster
	unsigned short iReservedSectors;    // Reserved sectors
	unsigned char iNumberFATs;         // Number of FATs

	unsigned short iRootEntries;        // Number of Root Directory entries
	unsigned short iLogicalSectors;     // Number of logical sectors
	unsigned char xMediumDescriptor;   // Medium descriptor

	unsigned short iSectorsFAT;         // Sectors per FAT
	unsigned short iSectorsTrack;       // Sectors per Track
	unsigned short iHeads;              // Number of heads

	unsigned short iHiddenSectors;      // Number of hidden sectors
};

void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[]);
//  Pre: Calculated directory offset and number of directory entries
// Post: Prints the filename, time, date, attributes and size of each entry

unsigned short endianSwap(unsigned char one, unsigned char two);
//  Pre: Two initialized characters
// Post: The characters are swapped (two, one) and returned as a short

void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[]);
//  Pre: An initialized BootSector struct and a pointer to an array
//       of characters read from a BootSector
// Post: The BootSector struct is filled out from the buffer data

char * toDOSName(char string[], unsigned char buffer[], int offset);
//  Pre: String is initialized with at least 12 characters, buffer contains
//       the directory array, offset points to the location of the filename
// Post: fills and returns a string containing the filename in 8.3 format

char * parseAttributes(char string[], unsigned char key);
//  Pre: String is initialized with at least five characters, key contains
//       the byte containing the attribue from the directory buffer
// Post: fills the string with the attributes

char * parseTime(char string[], unsigned short usTime);
//  Pre: string is initialzied for at least 9 characters, usTime contains
//       the 16 bits used to store time
// Post: string contains the formatted time

char * parseDate(char string[], unsigned short usDate);
//  Pre: string is initialized for at least 13 characters, usDate contains
//       the 16 bits used to store the date
// Post: string contains the formatted date

int roundup512(int number);
// Pre: initialized integer
// Post: number rounded up to next increment of 512

void printBootSector(struct BootSector * pBootS);
// Pre: A filled BootSector struct
// Post: The information about the boot sector prints to the console

// reads the boot sector and root directory
int main(int argc, char * argv[]) {
	int pBootSector = 0;
	unsigned char buffer[SIZE];
	unsigned char rootBuffer[ROOTSIZE * 32];
	struct BootSector sector;
	int iRDOffset = 0;

	// Check for argument
	if (argc < 2) {
		printf("Specify boot sector\n");
		exit(1);
	}

	// Open the file and read the boot sector
	pBootSector = open(argv[1], O_RDONLY);
	read(pBootSector, buffer, SIZE);

	// Decode the boot Sector
	decodeBootSector(&sector, buffer);

	// Calculate the location of the root directory
	iRDOffset = (1 + (sector.iSectorsFAT * sector.iNumberFATs))
			* sector.iBytesSector;

	// Read the root directory into buffer
	lseek(pBootSector, iRDOffset, SEEK_SET);
	read(pBootSector, rootBuffer, ROOTSIZE);
	close(pBootSector);
	printBootSector(&sector);

	// Parse the root directory
	parseDirectory(iRDOffset, sector.iRootEntries, rootBuffer);

} // end main

// Converts two characters to an unsigned short with two, one
unsigned short endianSwap(unsigned char one, unsigned char two) {
	// Set to the MSB
	short ret = two;
	// Shift MSB to actual MSB position
	ret = ret << 8;
	// OR with LSB
	ret |= one;
	return ret;
}

// Fills out the BootSector Struct from the buffer
void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[]) {
	int i = 3;  // Skip the first 3 bytes

	// Get the name
	while (i < 11) {
		pBootS->sName[i - 3] = buffer[i];
		i++;
	}
	// Null terminate
	pBootS->sName[8] = 0;

	// Read bytes/sector and convert to big endian
	pBootS->iBytesSector = endianSwap(buffer[0x0B], buffer[0x0C]);

	// Read sectors/cluster, Reserved sectors and Number of Fats
	pBootS->iSectorsCluster = buffer[0x0D];
	pBootS->iReservedSectors = endianSwap(buffer[0x0E], buffer[0x0F]);
	pBootS->iNumberFATs = buffer[0x10];

	// Read root entries, logical sectors and medium descriptor
	pBootS->iRootEntries = endianSwap(buffer[0x11], buffer[0x12]);
	pBootS->iLogicalSectors = endianSwap(buffer[0x13], buffer[0x14]);
	pBootS->xMediumDescriptor = buffer[0x15];

	// Read and covert sectors/fat, sectors/track, and number of heads
	pBootS->iSectorsFAT = endianSwap(buffer[0x16], buffer[0x17]);
	pBootS->iSectorsTrack = endianSwap(buffer[0x18], buffer[0x19]);
	pBootS->iHeads = endianSwap(buffer[0x1A], buffer[0x1B]);

	// Read hidden sectors
	pBootS->iHiddenSectors = endianSwap(buffer[0x1C], buffer[0x1D]);

	return;
}

// iterates through the directory to display filename, time, date,
// attributes and size of each directory entry to the console
void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[]) {
	int i = 0;
	char string[13];

	// Display table header with labels
	printf("Filename\tAttrib\tTime\t\tDate\t\tSize\n");

	// loop through directory entries to print information for each
	for (i = 0; i < (iEntries); i = i + 32) {
		if (1) {
			// Display filename
			printf("%s\t", toDOSName(string, buffer, 0x00*i));
			// Display Attributes
			printf("%s\t", parseAttributes(string, 0x0b*i));
			// Display Time
			printf("%s\t", parseTime(string, 0x16*i));
			// Display Date
			printf("%s\t", parseDate(string, 0x18*i));
			// Display Size
			printf("%d\n", 0x1c);
		}
	}

	// Display key
	printf("(R)ead Only (H)idden (S)ystem (A)rchive\n");
} // end parseDirectory()

// Parses the attributes bits of a file
char * parseAttributes(char string[], unsigned char key) {
	int i = 0;
	if (key &= 0b1) {
		string[i] = 'R';
		i++;
	}
	if (key &= 0b10) {
		string[i] = 'H';
		i++;
	}
	if (key &= 0b100) {
		string[i] = 'S';
		i++;
	}
	if (key &= 0b100000) {
		string[i] = 'A';
		i++;
	}
	//Null terminate
	string[i] = '\0';
	return string;
} // end parseAttributes()

// Decodes the bits assigned to the time of each file
char * parseTime(char string[], unsigned short usTime) {
	unsigned char hour = 0x00, min = 0x00, sec = 0x00;

	//printf("time: %x", usTime);
	// Grab the uppermost 5 bits
	hour = usTime >> 11;

	// Grab the middle 4 bits for the month
	min = usTime >> 5;
	min &= 0b111111;

	// Grab the lowest 5 bits for the day
	sec = usTime;
	sec &= 0b11111;
	// Multiply by two
	sec = sec << 1;

	sprintf(string, "%02i:%02i:%02i", hour, min, sec);

	return string;

} // end parseTime()

// Decodes the bits assigned to the date of each file
char * parseDate(char string[], unsigned short usDate) {
	unsigned char month = 0x00, day = 0x00;
	unsigned short year = 0x0000;

	//printf("date: %x", usDate);

	// Grab the uppermost 9 bits
	year = usDate >> 7;
	// Add 1980 for epoch
	year += 1980;

	// Grab the middle 4 bits for the month
	month = usDate >> 5;
	month &= 0b1111;

	// Grab the lowest 5 bits for the day
	day = usDate;
	day &= 0b11111;

	sprintf(string, "%d/%d/%d", year, month, day);

	return string;

} // end parseDate()

// Formats a filename string as DOS (adds the dot to 8-dot-3)
char * toDOSName(char string[], unsigned char buffer[], int offset) {
	int i;
	for (i = 0; i < 8; i++) {
		string[i] = buffer[i];
	}
	string[8] = '.';
	for (i = 8; i < 11; i++) {
		string[i + 1] = buffer[i];
	}
	return string;
} // end toDosNameRead-Only Bit

int roundup512(int number) {
	// Add 512 for the round up instead of down
	number += 512;
	// Shift right by 9
	number = number >> 9;
	// Shift left by 9 - this truncates the lower 9 bits
	number = number << 9;
	return number;
}

// Displays the BootSector information to the console
void printBootSector(struct BootSector * pBootS) {
#ifndef PRINT_HEX
	printf("                    Name:   %s\n", pBootS->sName);
	printf("            Bytes/Sector:   %i\n", pBootS->iBytesSector);
	printf("         Sectors/Cluster:   %i\n", pBootS->iSectorsCluster);
	printf("        Reserved Sectors:   %i\n", pBootS->iReservedSectors);
	printf("          Number of FATs:   %i\n", pBootS->iNumberFATs);
	printf("  Root Directory entries:   %i\n", pBootS->iRootEntries);
	printf("         Logical sectors:   %i\n", pBootS->iLogicalSectors);
	printf("       Medium descriptor:   0x%04x\n", pBootS->xMediumDescriptor);
	printf("             Sectors/FAT:   %i\n", pBootS->iSectorsFAT);
	printf("           Sectors/Track:   %i\n", pBootS->iSectorsTrack);
	printf("         Number of heads:   %i\n", pBootS->iHeads);
	printf("Number of Hidden Sectors:   %i\n", pBootS->iHiddenSectors);
#else
	printf("                    Name:   %s\n", pBootS->sName);
	printf("            Bytes/Sector:   0x%04x\n", pBootS->iBytesSector);
	printf("         Sectors/Cluster:   0x%02x\n", pBootS->iSectorsCluster);
	printf("        Reserved Sectors:   0x%04x\n", pBootS->iReservedSectors);
	printf("          Number of FATs:   0x%02x\n", pBootS->iNumberFATs);
	printf("  Root Directory entries:   0x%04x\n", pBootS->iRootEntries);
	printf("         Logical sectors:   0x%04x\n", pBootS->iLogicalSectors);
	printf("       Medium descriptor:   0x%02x\n", pBootS->xMediumDescriptor);
	printf("             Sectors/FAT:   0x%04x\n", pBootS->iSectorsFAT);
	printf("           Sectors/Track:   0x%04x\n", pBootS->iSectorsTrack);
	printf("         Number of heads:   0x%04x\n", pBootS->iHeads);
	printf("Number of Hidden Sectors:   0x%04x\n", pBootS->iHiddenSectors);
#endif
	return;
}
