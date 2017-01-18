#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char **argv)
{

	uint8_t data[2];
	uint16_t data_main;
	uint8_t read_command = 0xAC;

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
	uint8_t sendData[2] = {0xA0,0x03};
	bcm2835_i2c_setSlaveAddress(0x29);
	bcm2835_i2c_write((char *)sendData,2);

	//bcm2835_i2c_setSlaveAddress(0x29); // write slave addres 

	//sensor init


	bcm2835_i2c_setSlaveAddress(0x29);
	bcm2835_i2c_write((char*)&read_command,1); //
	bcm2835_i2c_read((char*)data, 2); //write slave addres
	
	
	data_main = (data[1] << 8) + data[0];
	printf("%d\n",data_main);

	bcm2835_i2c_end();
	bcm2835_close();
	return 0;
}