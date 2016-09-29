#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>

#define AANTAL 8
#define PIN1 RPI_GPIO_P1_11
#define PIN2 RPI_GPIO_P1_12
#define PIN3 RPI_GPIO_P1_13
#define PIN4 RPI_GPIO_P1_15
#define PIN5 RPI_GPIO_P1_16
#define PIN6 RPI_GPIO_P1_18
#define PIN7 RPI_GPIO_P1_08
#define PIN8 RPI_GPIO_P1_07

int main(int argc, char **argv)
{
	if(!bcm2835_init())
	{
		return 1;
	}

	uint8_t leds[AANTAL] = {PIN1,PIN2,PIN3,PIN4,PIN5,PIN6,PIN7,PIN8};

	int j;
	for(j=0;j<8;j++)
	{
		bcm2835_gpio_fsel(leds[j], BCM2835_GPIO_FSEL_OUTP);
	}

	while(1)
	{
		bcm2835_gpio_write(leds[0], HIGH);
		int i = 0;
		for(i = 1; i < 8; i++)
		{
		bcm2835_gpio_write(leds[i-1], LOW);
		bcm2835_delay(500);
		bcm2835_gpio_write(leds[i], HIGH);
		bcm2835_delay(500);
		}
		bcm2835_gpio_write(leds[7], LOW);
	}

	bcm2835_close();
	return 0;
}