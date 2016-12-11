#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

void TSL2561_init(uint8_t address, uint8_t command);
uint16_t TSL2561_Read_Word_i2c(uint8_t address, uint8_t command);
volatile uint8_t safeshutdown;

int main(int argc, char **argv)
{


	uint16_t data;
	uint8_t read_command = 0xAC;

	signal(SIGINT,sighandler);

	if(!bcm2835_init())
	{
		return 1;
	}
	if(!bcm2835_i2c_begin())
	{
		printf("FAILED!! \n");
		return 1;
	}
	
	//Senor init moest nog wakker worden
	TSL2561_init(0x29);

	while(1)
	{

		data = TSL2561_Read_Word_i2c(0x29, read_command);

		printf("%d\n",data_main);
		bcm2835_delay(800);
	}
	bcm2835_i2c_end();
	bcm2835_close();
	return 0;
}
void TSL2561_init(uint8_t address)
{
	uint8_t sendData[2] = {0xA0,0x03};
	bcm2835_i2c_setSlaveAddress(address);
	bcm2835_i2c_write((char *)sendData,2);
}
uint16_t TSL2561_Read_Word_i2c(uint8_t address, uint8_t command)
{
	uint8_t data[2];
	uint16_t data_main;

	//bcm2835_i2c_setSlaveAddress(0x29); // write slave addres 
	bcm2835_i2c_setSlaveAddress(0x29);
	bcm2835_i2c_write((char*)&read_command,1); //read command en aantal bytes -> 1byte
	bcm2835_i2c_read((char*)data, 2); //write slave addres
		
		
	data_main = (data[1] << 8) + data[0];

	return data_main;
}
void sighandler(int signum)
{
	safeshutdown = 1;
	printf("\nVeilig Afgesloten\n");
}