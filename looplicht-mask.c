#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>

#define AANTAL 8
#define STANDEN 8
#define PIN1 RPI_V2_GPIO_P1_11
#define PIN2 RPI_V2_GPIO_P1_12
#define PIN3 RPI_V2_GPIO_P1_13
#define PIN4 RPI_V2_GPIO_P1_15
#define PIN5 RPI_V2_GPIO_P1_16
#define PIN6 RPI_V2_GPIO_P1_18
#define PIN7 RPI_V2_GPIO_P1_08
#define PIN8 RPI_V2_GPIO_P1_07

int main(int argc, char **argv)
{
	if(!bcm2835_init())
	{
		return 1;
	}

	uint32_t mask[STANDEN] = {
		1 << PIN1,
		1 << PIN2,
		1 << PIN3,
		1 << PIN4,
		1 << PIN5,
		1 << PIN6,
		1 << PIN7,
		1 << PIN8,
	};
	uint8_t leds[AANTAL] = {PIN1,PIN2,PIN3,PIN4,PIN5,PIN6,PIN7,PIN8};

	//initialisatie van de leds
	int j;
	for(j=0;j<8;j++)
	{
		bcm2835_gpio_fsel(leds[j], BCM2835_GPIO_FSEL_OUTP);
	}

	while(1)
	{
		int i;
		for(i = 0; i < STANDEN; i++)
		{
			bcm2835_gpio_write_mask (mask[i]);
		}
	}	

	bcm2835_close();
	return 0;
}