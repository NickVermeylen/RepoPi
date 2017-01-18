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

static const uint8_t gpselReg = 1;
static const uint8_t gpselBit = 21;
static const uint8_t gpioPin = 17;

//static char terminal;

static int led_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello procdriver!\n");
  return 0;
}
static int led_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, led_proc_show, NULL);
}

//proc write functie
ssize_t led_proc_write(struct file * file, const char __user * user, size_t size, loff_t * loff)
{
	if(size >= 1)
	{
		char buffer[10];
		
		//printk("Please give the set bit: ");
		copy_from_user(buffer, user, size);

		//terminal = buffer;
		if(buffer[0] == '1')
		{
			gpioPtr = gpio + GPSET0/4;
			*gpioPtr = 1 << gpioPin;
			printk("Set the led.\n");
		}
		else
		{
			gpioPtr = gpio + GPCLR0/4;
			*gpioPtr = 1 << gpioPin;
			printk("Reset the led.\n");
		}
		//terminal = '\0';
	}
	else
	{
		printk("The input was to big!\n");
	}

	return size;
}

//Init functie voor de LEDS
void initLed(void)
{
	gpioPtr = gpio + ((FNTSEL_OFFSET * gpselReg)/4);
	*gpioPtr = *gpioPtr | (1 << gpselBit);
	*gpioPtr = *gpioPtr & (~(1 << (gpselBit + 1)) & ~(1 << (gpselBit + 2)));
}

//Map functie voor de peripheralbase
void initIO(void)
{
	struct device_node * socNode;
	uint32_t data[3] = {0};

	socNode = of_find_node_by_name(NULL, "soc");

	if(of_property_read_u32_array(socNode, "ranges", data, 3) == 0)
	{
		if((periBase = ioremap(data[1], data[2])) != NULL)
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
