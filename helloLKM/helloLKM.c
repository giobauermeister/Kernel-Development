/*
*	Eudyptula Challenge Task 1
*	Loadable Kernel Module
*	This module was based on "The Linux Kernel Module Programming Guide"
*	http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Giovanni Bauermeister");

static int __init helloLKM_init(void)
{
	printk(KERN_DEBUG "Hello World!\n");
	return 0;
}

static void __exit helloLKM_exit(void)
{
	printk(KERN_DEBUG "Hello World module unloaded\n");
}

module_init(helloLKM_init);
module_exit(helloLKM_exit);
