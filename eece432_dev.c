/* Prototype module for device driver assignment */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>	
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include "ioctl_test.h"
#include <linux/ioctl.h>
#include <linux/interrupt.h>
/* Prototypes - this would normally go in a .h file */
static int eece432_open( struct inode*, struct file* );
static int eece432_release( struct inode*, struct file* );
static ssize_t eece432_read( struct file*, char*, size_t, loff_t* );
static ssize_t eece432_write( struct file*, const char*, size_t, loff_t* );
// int eece432_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
long eece432_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#define DEVICE_NAME "eece432_dev" /* Dev name as it appears in /proc/devices */
#define MAJOR_NUMBER 254
#define MIN_MINOR_NUMBER 0
#define MAX_MINOR_NUMBER 1
#define BUF_LEN 80
struct cdev my_cdev;
static char message[BUF_LEN];
static char *msg;
static short size_of_message;
#define DEVICE_COUNT 2
static int driving_mode;
static int Device_Open = 0;
static char velocity[BUF_LEN];
static char location[BUF_LEN];
/* end of what really should have been in a .h file */

/* file operations struct */
DECLARE_WAIT_QUEUE_HEAD(name);
static struct proc_dir_entry *Our_Proc_File;

static struct file_operations eece432_fops = {
	.owner   = THIS_MODULE,
	.read    = eece432_read,
	.write   = eece432_write,
	.open    = eece432_open,
	.release = eece432_release,
    .unlocked_ioctl   = eece432_ioctl
};

/* called when module is loaded */
int eece432_init_module( void ) {
	/* initialization code belongs here */
	dev_t devnumber = MKDEV(MAJOR_NUMBER, MIN_MINOR_NUMBER);
	register_chrdev_region(devnumber, DEVICE_COUNT, DEVICE_NAME);
	cdev_init(&my_cdev, &eece432_fops);
	my_cdev.owner = THIS_MODULE;
	int err_code = cdev_add(&my_cdev, devnumber, DEVICE_COUNT);
	if(err_code < 0){
		printk("ERR");
		return -1;
	}
	
	try_module_get(THIS_MODULE);
	printk(KERN_INFO "eece432: Hello from your device!\n");
	return 0;
}

/* Called when module is unloaded */
void eece432_cleanup_module( void ) {
	/* clean up code belongs here */
	dev_t devnumber = MKDEV(MAJOR_NUMBER, MIN_MINOR_NUMBER);
	
	printk(KERN_INFO "eece432: Module unloaded.\n");
	unregister_chrdev_region(devnumber, DEVICE_COUNT);
	cdev_del(&my_cdev);
}



/* Called when a process tries to open the device file */
static int eece432_open( struct inode *inode, struct file *filp ) {
	/* device claiming code belongs here */
	static int counter = 0;
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	try_module_get(THIS_MODULE);
	printk(KERN_INFO "eece432: in open.\n");
	
	return 0;
}


/* Called when a process closes the device file. */
static int eece432_release( struct inode *inode, struct file *filp ) {
	/* device release code belongs here */
	Device_Open--;
	printk(KERN_INFO "eece432: in release.\n");
	module_put(THIS_MODULE);
	return 0;
}


/* Called when a process, which already opened the dev file, attempts to read from it. */
static ssize_t eece432_read( struct file *filp,
    char *buf,      /* The buffer to fill with data     */
    size_t count,   /* The max number of bytes to read  */
    loff_t *f_pos )  /* The offset in the file           */
{
	printk(KERN_INFO "eece432: in read.\n");

	/* read code belongs here */
	int bytes_read = 0;
	printk(KERN_INFO "copied %s", message);
	printk(KERN_INFO "velocity is %s", velocity);
	printk(KERN_INFO "location is %s", location);
	int error_count = copy_to_user(buf,message,size_of_message);
	printk(KERN_INFO "sent %d characters to user\n", size_of_message);
	return (size_of_message=0);
}

/* Called when a process writes to dev file */
static ssize_t eece432_write( struct file *filp,
    const char *buf,/* The buffer to get data from      */
    size_t count,   /* The max number of bytes to write */
    loff_t *f_pos )  /* The offset in the file           */
{
	printk(KERN_INFO "eece432: in write.\n");
	size_of_message = count;
	if(size_of_message > BUF_LEN){
		size_of_message = BUF_LEN;
	}
	/* write code belongs here */	
	if(copy_from_user(message, buf, size_of_message))
		return -EFAULT;
	printk(KERN_INFO "received %d chars from the user\n", size_of_message);	
	if(message[0] == 'l'){			//if writing the location
		copy_from_user(location, buf+1,size_of_message-1);
		printk(KERN_INFO "copied location %s from the user\n", location);	
	}
	else if(message[0] == 'v'){
		copy_from_user(velocity, buf+1,size_of_message-1);
		printk(KERN_INFO "copied velocity %s from the user\n", velocity);	
	}

	return size_of_message; //return number of bytes written
}

/* called by system call icotl */ 
long eece432_ioctl(
	// struct inode *inode, 
    struct file *filp, 
    unsigned int cmd,   /* command passed from the user */
    unsigned long arg ) /* argument of the command */
{
	/* ioctl code belongs here */
	printk(KERN_INFO "eece432: ioctl called.\n");
	switch(cmd){
	case EN_DRV_SAFE:
		driving_mode = 1;
		printk(KERN_INFO "eece432: ioctl safe mode called.\n");
		disable_irq(1);
		return 1;
		break;
	case DIS_DRV_SAFE:
		driving_mode = 0;
		printk(KERN_INFO "eece432: ioctl unsafe mode called.\n");
		enable_irq(1);
		return 0;
		break;
	default:
		return -ENOTTY;
	}
//	return 0; //has to be changed
}

module_init( eece432_init_module );
module_exit( eece432_cleanup_module );

MODULE_AUTHOR( "NADER" );
MODULE_LICENSE( "GPL" );
