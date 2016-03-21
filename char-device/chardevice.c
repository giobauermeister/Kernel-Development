/*	This is a kernel character device 
*	Based on Derek Molloy tutorial
*	http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
*
*	Author: Giovanni Bauermeister
*/

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function

#define DEVICE_NAME "chardevice"
#define CLASS_NAME "char"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Giovanni Bauermeister");
MODULE_DESCRIPTION("A simple linux character device for Colibri VF61");
MODULE_VERSION("0.1");

static int				majorNumber;
static char				message[256] = {0};
static short  			size_of_message;
static int    			numberOpens = 0;
static struct class*	chardeviceClass  = NULL;
static struct device*	chardeviceDevice = NULL;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init chardevice_init(void){

	printk(KERN_INFO "CharDevice: Initializing the Character Device LKM\n");

	// Dynamic major number allocation for the device
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
		printk(KERN_ALERT "CharDevice: Failed to register a major number\n");
		return majorNumber;		
	}
	printk(KERN_INFO "CharDevice: registered correctly with major number %d\n ", majorNumber);

	// Register the device class
	chardeviceClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(chardeviceClass)){
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class");	
	}
	printk(KERN_INFO "CharDevice: device class registered correctly\n");
	
	// Register the device driver
	chardeviceDevice = device_create(chardeviceClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(chardeviceDevice)){
		class_destroy(chardeviceClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(chardeviceDevice);
	}
	printk(KERN_INFO "CharDevice: device driver created correctly\n");
	return 0;
}
static void __exit chardevice_exit(void){
	device_destroy(chardeviceClass, MKDEV(majorNumber, 0));
	class_unregister(chardeviceClass);
	class_destroy(chardeviceClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "CharDevice: Unloaded the Character Device LKM\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
	numberOpens++;
	printk(KERN_INFO "CharDevice: Device has been opened %d time(s)\n", numberOpens);
	return 0;
}	   

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	error_count = copy_to_user(buffer, message, size_of_message);
	
	if (error_count==0){
		printk(KERN_INFO "CharDevice: Sent %d characters to the user\n", size_of_message);
		return (size_of_message=0);
	}
	else {
		printk(KERN_INFO "CharDevice: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	sprintf(message, "%s(%d letters)", buffer, len);
	size_of_message = strlen(message);
	printk(KERN_INFO "CharDevice: Received %d characters from the user\n", len);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "CharDevice: Device successfully closed\n");
	return 0;
}

module_init(chardevice_init);
module_exit(chardevice_exit);


















