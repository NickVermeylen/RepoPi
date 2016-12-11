//Nick Vermeylen 	20/10/2016
//Blinky by own user-space driver

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

//danku Oli #noGoogle
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <bcm2835.h> //enkel voor de delay functies

#define PERI_BASE_ADDRESS 0x3F000000  //--> uit /dev/mem van de pi nrml uitlezen met fuctie maar blijft toch altyd hetzelfde 
#define PERI_SIZE 0x01000000
#define GPIO_BASE_ADDRESS 0x200000
#define FNTSEL_OFFSET 0x0004
#define GPSET0 0x001c
#define GPCLR0 0x0028

#define LED 17

static void *mapmem(const char *msg, size_t size, int fd, off_t off);

//Function for nice shutdown on ctrl+c
volatile uint8_t quit = 1;
void stopHandler()
{
	quit = 0;
}

int main(int argc, char **argv)
{
	//Nice shutdown on ctrl+c
	signal(SIGINT, stopHandler);

	int memory;
	uint32_t * vmem;
	uint32_t * gpio;
	volatile uint32_t * gpioPtr;		//volatile zodat programma steeds verplicht w om te kijken of var veranderd is ipv const te maken
	if(geteuid() == 0)
	{
		if((memory = open("/dev/mem", O_RDWR | O_SYNC)) < 0)			// /dev/mem openen en int terug geven vanwaar die file staat
		{
			fprintf(stderr, "init: Unable to open /dev/mem: %s\n", strerror(errno));
			exit(1);
		}

		vmem = mapmem("gpio",PERI_SIZE, memory, (uint32_t)PERI_BASE_ADDRESS);		// /dev/mem geheugenplaatsten mappen in userspace
		if(vmem == MAP_FAILED) exit(1);

		//pointer naar peripheral blok
		gpio = vmem + GPIO_BASE_ADDRESS/4;

		gpioPtr = gpio + FNTSEL_OFFSET/4;		//springen in het gpio blok naar het function select register
												//pin 17 --> ledje als output geselecteerd
		//
		// 001 sturen naar de 23-21 bits van dit register
		//
		__sync_synchronize();
		*gpioPtr = *gpioPtr & ~(1<<22) & ~(1<<23);
		*gpioPtr = *gpioPtr | (1 << 21);
		__sync_synchronize();
		//
		//led0 (pin17) staat nu als output geinit
		//
		while(quit)
		{
			//LED AAN
			gpioPtr = gpio + GPSET0/4;		//springen naar het set register
			*gpioPtr = *gpioPtr | (1 << 17);		//17e bit = pin17 setten
			delay(400);

			//LED UIT
			gpioPtr = gpio + GPCLR0/4; 		//springen naar het clear register
			*gpioPtr = *gpioPtr | (1 << 17);
			delay(400);
		}
	}
	else
	{
		printf("PLEASE USE MOTHERF*CKING SUDO!\n");
	}
	munmap((void**) &vmem, PERI_SIZE);
	close(memory);

	bcm2835_close();

	return 0;
}
static void *mapmem(const char *msg, size_t size, int fd, off_t off)
{
	void *map = mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, off);
	if(map == MAP_FAILED)
	{
		fprintf(stderr, "init: %s mmap failed: %s\n", msg, strerror(errno));
	}
	return map;
}

