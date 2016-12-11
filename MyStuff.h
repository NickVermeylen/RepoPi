/*Library MyStuff.h by Nick Vermeylen
		7/10/2016					*/

#include <bcm2835.h>
#include <stdio.h>

void TSL2561_init(uint8_t address, uint8_t command);
uint16_t TSL2561_Read_Word_i2c(uint8_t address, uint8_t command);
volatile uint8_t safeshutdown;

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
	printf("\nVeilig Afgesloten\n")
}