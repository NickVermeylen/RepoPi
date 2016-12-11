#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

volatile uint8_t safeshutdown;

void sighandler(int);

int main(int argc, char **argv)
{
	uint8_t read_msb;
	uint8_t read_lsb;
	uint16_t read;
	float temp;

	signal(SIGINT,sighandler);

	if(!bcm2835_init())
	{
		printf("FAILED");
		return 1;
	}
	if(!bcm2835_spi_begin())
	{
		printf("Spi init FAILED!\n");
		return 1;
	}
	//init tempsensor
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS1);

	bcm2835_spi_transfer(0x08);
	bcm2835_spi_transfer(0x80);
	//init end
	while(!safeshutdown)
	{
		bcm2835_spi_transfer(0x50);
		bcm2835_delay(300);

		read_msb = bcm2835_spi_transfer(0xFF);
		read_lsb = bcm2835_spi_transfer(0xFF);
		read = (read_msb << 8)+ read_lsb;

		temp = ((float)read)/128.0;
		printf("%f\n", temp);
	}

	bcm2835_spi_end();
	bcm2835_close();
	return 0;
}
void sighandler(int signum)
{
	safeshutdown = 1;
	printf("\nVeilig Afgesloten\n");
}