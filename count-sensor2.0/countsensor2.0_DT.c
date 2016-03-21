#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kobject.h>
#include <linux/time.h>

#define DEBOUNCE_TIME 200
#define DRV_NAME	"ircounter"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Giovanni Bauermeister based on Derek Molloy");
MODULE_DESCRIPTION("A GPIO Counter Sensor Kernel Module");
MODULE_VERSION("0.1");

static bool isRising = 1;
module_param(isRising, bool, S_IRUGO);
MODULE_PARM_DESC(isRising, " Rising edge = 1 (default), Falling edge = 0");

static unsigned int gpioSensor = 39; //SODIMM_55
module_param(gpioSensor, uint, S_IRUGO);
MODULE_PARM_DESC(gpioSensor, " GPIO Sensor number (default=39)");

static char   gpioName[8] = "gpioXXX";
static int    irqNumber;
static int    numberPieces = 0;
static bool   isDebounce = 0;
static struct timespec ts_last, ts_current, ts_diff;

static irq_handler_t  gpiosensor_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

//callback function to output the counter value
static ssize_t numberPieces_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
   return sprintf(buf, "%d\n", numberPieces);
}

static ssize_t numberPieces_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
   sscanf(buf, "%du", &numberPieces);
   return count;
}

static ssize_t lastTime_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
   return sprintf(buf, "%.2lu:%.2lu:%.2lu:%.9lu \n", (ts_last.tv_sec/3600)%24,
          (ts_last.tv_sec/60) % 60, ts_last.tv_sec % 60, ts_last.tv_nsec );
}

static ssize_t diffTime_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
   return sprintf(buf, "%lu.%.9lu\n", ts_diff.tv_sec, ts_diff.tv_nsec);
}

static ssize_t isDebounce_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
   return sprintf(buf, "%d\n", isDebounce);
}

static ssize_t isDebounce_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
   unsigned int temp;
   sscanf(buf, "%du", &temp);                // use a 600-80temp varable for correct int->bool
   gpio_set_debounce(gpioSensor,0);
   isDebounce = temp;
   if(isDebounce) { gpio_set_debounce(gpioSensor, DEBOUNCE_TIME);
      printk(KERN_INFO "COUNT_SENSOR: Debounce on\n");
   }
   else { gpio_set_debounce(gpioSensor, 0);  // set the debounce time to 0
      printk(KERN_INFO "COUNT_SENSOR: Debounce off\n");
   }
   return count;
}

static struct kobj_attribute count_attr = __ATTR(numberPieces, 0660, numberPieces_show, numberPieces_store);
static struct kobj_attribute debounce_attr = __ATTR(isDebounce, 0660, isDebounce_show, isDebounce_store);

static struct kobj_attribute time_attr  = __ATTR_RO(lastTime);
static struct kobj_attribute diff_attr  = __ATTR_RO(diffTime);

static struct attribute *count_sensor_attrs[] = {
	&count_attr.attr,
	&time_attr.attr,
	&diff_attr.attr,
	&debounce_attr.attr,
	NULL,
};

static struct attribute_group attr_group = {
      .name  = gpioName,                 ///< The name is generated in ebbButton_init()
      .attrs = count_sensor_attrs,                ///< The attributes array defined just above
};

static struct kobject *gpiosensor_kobj;

static int __init gpiosensor_init(void){
	int result = 0;
	unsigned long IRQflags = IRQF_TRIGGER_RISING;
	
	printk(KERN_INFO "COUNT_SENSOR: Initializing the Count Sensor LKM\n");
	sprintf(gpioName, "gpio%d", gpioSensor);
	
	gpiosensor_kobj = kobject_create_and_add("gpiosensor", kernel_kobj->parent);
   	if(!gpioSensor){
      	printk(KERN_ALERT "COUNT_SENSOR: failed to create kobject mapping\n");
      	return -ENOMEM;
   	}
   	result = sysfs_create_group(gpiosensor_kobj, &attr_group);
   	if(result) {
      printk(KERN_ALERT "COUNT_SENSOR: failed to create sysfs group\n");
      kobject_put(gpiosensor_kobj);                          
      return result;
   	}
   	getnstimeofday(&ts_last);
	ts_diff = timespec_sub(ts_last, ts_last);

	gpio_request(gpioSensor, "sysfs");
	gpio_direction_input(gpioSensor);
	gpio_set_debounce(gpioSensor, DEBOUNCE_TIME);
	gpio_export(gpioSensor, false);
	
	printk(KERN_INFO "COUNT_SENSOR: The button state is currently: %d\n", gpio_get_value(gpioSensor));
	irqNumber = gpio_to_irq(gpioSensor);
	printk(KERN_INFO "COUNT_SENSOR: The button is mapped to IRQ: %d\n", irqNumber);

	if(!isRising){                           // If the kernel parameter isRising=0 is supplied
      IRQflags = IRQF_TRIGGER_FALLING;      // Set the interrupt to be on the falling edge
   	}
   	
   	result = request_irq(irqNumber,             
                        (irq_handler_t) gpiosensor_irq_handler, 
                        IRQflags,              
                        "gpiosensor_handler",  
                        NULL);    
    return result;
}

static void __exit gpiosensor_exit(void){
	printk(KERN_INFO "COUNT_SENSOR: The sensor counted %d pieces\n", numberPieces);
	kobject_put(gpiosensor_kobj);
	free_irq(irqNumber, NULL);
	gpio_unexport(gpioSensor);
	gpio_free(gpioSensor);
	printk(KERN_INFO "COUNT_SENSOR: The Count Sensor LKM was unloaded!\n");
}

static irq_handler_t gpiosensor_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
	getnstimeofday(&ts_current);
	ts_diff = timespec_sub(ts_current, ts_last);
	ts_last = ts_current;
	printk(KERN_INFO "COUNT_SENSOR: The sensor state is currently: %d\n", gpio_get_value(gpioSensor));
	numberPieces++;
	return (irq_handler_t) IRQ_HANDLED;
}

module_init(gpiosensor_init);
module_exit(gpiosensor_exit);


















