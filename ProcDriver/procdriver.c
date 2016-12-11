#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/of.h>

 #define MODULE_NAME "procdriver: "

//#define PERI_BASE_ADDRESS 0x3F000000  //--> uit /dev/mem van de pi nrml uitlezen met fuctie maar blijft toch altyd hetzelfde 
//#define PERI_SIZE 0x01000000
#define GPIO_BASE_ADDRESS 0x200000		//GPIO BASE ADDRESS
#define FNTSEL_OFFSET 0x0004			//Commands gevonden in datasheet
#define GPSET0 0x001c					//Commands gevonden in datasheet
#define GPCLR0 0x0028					//Commands gevonden in datasheet

//Alle globale variabelen
static volatile uint32_t * periBase;
static volatile uint32_t * gpio;
static volatile uint32_t * gpioPtr;

static const uint8_t gpselReg[8] = {1,1,2,2,2,2,1,0};
static const uint8_t gpselBit[8] = {21,24,21,6,9,12,12,12};
static const uint8_t gpioPin[8] = {17,18,27,22,23,24,14,4};

static char terminal[9] = {"00000000"};

static int led_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello procdriver!\nGive the 8bits plz: %s\n", terminal);
  return 0;
}
static int led_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, led_proc_show, NULL);
}

//proc write functie
ssize_t led_proc_write(struct file * file, const char __user * user, size_t size, loff_t * loff)
{
	if(size >= 8)
	{
		char kernel[50];
		uint8_t i;

		copy_from_user(kernel, user, size);

		for(i = 0; i < 8; i++)
		{
			terminal[i] = kernel[i];
			if(kernel[i] == '1')
			{
				gpioPtr = gpio + GPSET0/4;
				*gpioPtr = 1 << gpioPin[i];
			}
			else
			{
				gpioPtr = gpio + GPCLR0/4;
				*gpioPtr = 1 << gpioPin[i];
			}
		}
		terminal[i] = '/0';
	}
	else
	{
		printk("There were no 8bits found as input!");
	}

	return size;
}

//Init functie voor de LEDS
void initLed(void)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		gpioPtr = gpio + ((FNTSEL_OFFSET * gpselReg[i])/4);
		*gpioPtr = *gpioPtr | (1 << gpselBit[i]);
		*gpioPtr = *gpioPtr & (~(1 << (gpselBit[i] + 1)) & ~(1 << (gpselBit[i] + 2)));
	}
}

//Map functie voor de peripheralbase
void initIO(void)
{
	struct device_node * socNode;
	uint32_t data[3] = {0};

	socNode = of_find_node_by_name(NULL, "soc");

	if(of_property_read_u32_array(socNode, "ranges", data, 3) == 0)
	{
		if(periBase = ioremap(data[1], data[2]) != NULL)
		{
			gpio = periBase + GPIO_BASE_ADDRESS/4; 		// /4 pointer aritmatic
			initLed();
		}
		else
		{
			printk("Mapping the I/O FAILED...");
		}
	}
	else
	{
		printk("Reading the socNode FAILED...");
	}
}

//Unmap functie voor de peripheralbase
void uninitIO(void)
{
	iounmap(periBase);
}

static const struct file_operations led_proc_fops = {
  .owner = THIS_MODULE,
  .open = led_proc_open,
  .read = seq_read,
  .write = led_proc_write,
  .llseek = seq_lseek,
  .release = single_release,
};
static int __init led_proc_init(void) {
  proc_create("procdriver", 0, NULL, &led_proc_fops);		//if ProcCreate fails *Condom* was in place ... (Humor)
  printk(KERN_DEBUG MODULE_NAME "init procdriver!\n");
  initIO();
  return 0;
}
static void __exit led_proc_exit(void) {
  remove_proc_entry("procdriver", NULL);
  uninitIO();
  printk(KERN_DEBUG MODULE_NAME "exit procdriver!\n");
}
MODULE_LICENSE("GPL");
module_init(led_proc_init);
module_exit(led_proc_exit);
