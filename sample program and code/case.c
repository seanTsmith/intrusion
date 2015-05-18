//AEWIN CaseOpen Program(Dos Version)
#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#define PM_BASE		0xFED80000+0x300+0x5A

static inline unsigned char breadb(unsigned long addr);
inline void bwriteb(unsigned char value, unsigned long addr);
void help();

int main(int argc, char *argv[])
{
	int data_rw8;

	if (argc!=2){
		help();
		return;
	}

	data_rw8 = breadb(PM_BASE);

	if(strcmp(argv[1], "-r") == 0){
		data_rw8 = breadb(PM_BASE);
		if(data_rw8&0x04){
			printf("Status failed!\n");
		}
		else{
			printf("Status OK!\n");
		}
	}
	else if(strcmp(argv[1], "-c") == 0){
		data_rw8 = breadb(PM_BASE);
		data_rw8 |= 0x02;
		bwriteb(data_rw8, PM_BASE);
		delay(1);
		data_rw8 &= 0xFD;
		bwriteb(data_rw8, PM_BASE);
		printf("Status clear\n");
	}
	else{
		help();
	}

	return;
}

void help()
{
	printf("AEWIN Case open Program\n");
	printf("Usage:\n");
	printf("GPIO -c (Clear status)\n");
	printf("GPIO -r (Read status)\n");
}

static inline unsigned char breadb(unsigned long addr)
{
	void *virtualAddress = NULL;
	unsigned long pageaddr = 0;
	unsigned long size = 0x1000;
	unsigned long offset = 0;
	unsigned char value = 0;

	pageaddr = addr&0xFFFFF000;
	offset = addr&0x00000FFF;

	if (__djgpp_map_physical_memory(virtualAddress, size, pageaddr) == 0){
		value = *((unsigned char *)((unsigned long)virtualAddress + offset));
	}
	else{
		value = 0xFF;
	}
	return value;
}

inline void bwriteb(unsigned char value, unsigned long addr)
{
	void *virtualAddress = NULL;
	unsigned long pageaddr = 0;
	unsigned long size = 0x1000;
	unsigned long offset = 0;
	unsigned char * address = NULL;

	pageaddr = addr&0xFFFFF000;
	offset = addr&0x00000FFF;

	if (__djgpp_map_physical_memory(virtualAddress, size, pageaddr) == 0){
		address = (unsigned char *)((unsigned long)virtualAddress + offset);
		*address = value;
	}
}
