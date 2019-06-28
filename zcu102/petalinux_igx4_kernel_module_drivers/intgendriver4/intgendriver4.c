/*
     Author: Kris Nikov - kris.nikov@bris.ac.uk
     Date: 05 Feb 2019
     Description: Kernel module driver for interrupt generator 4
*/
/*  intgendriver4.c - The simplest kernel module.

* Copyright (C) 2013 - 2016 Xilinx, Inc
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program. If not, see <http://www.gnu.org/licenses/>.

*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/moduleparam.h>

/*Driver specific libraries*/
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/sched.h>

/* Standard module information, edit as appropriate */
MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Dr Kris Nikov");
MODULE_DESCRIPTION
    ("intgendriver4 - Interrupt generator driver module.");

#define DRIVER_NAME "intgendriver4"

/*Driver-specific defines*/
#define fpga_status 0x10
#define SUCCESS 	0

/*Even more specific defines*/
#define MAJOR_NUM 103
#define MINOR_NUM 0

/*Interrupt-specific defines*/
#define MB_TASK_DONE 92
#define IOCTL_WAIT_INTERRUPT	_IOR(MAJOR_NUM, 0, char *)	/*wait for interrupt*/
#define IOCTL_SET_WOFFSET	_IOR(MAJOR_NUM, 1, char *)
#define IOCTL_SET_ROFFSET	_IOR(MAJOR_NUM, 2, char *)

//Enable printk debig statements
int DBG;
module_param(DBG, int, S_IRUSR|S_IWUSR); //passing the value to the module as argument allows flexibility when we load the driver module from userspace. Values are 0 - OFF and N - ON
//Make the Reg0 address global so we can print in DBG statements
int *intr0 = 0;

//Enable data passing from userspace for driver debug
struct ioctl_arguments {
     unsigned int spmm_workload;
};
struct ioctl_arguments args;

//Enable timing information for wait_event_interruptible_timeout so that the thread does not stall if IRQ (accelerator) is too fast.
int TIMEOUT; //Maximum number of seconds for the thread to remain in wait queue. Default is 15s.
module_param(TIMEOUT, int, S_IRUSR|S_IWUSR); //passing the value to the module as argument allows flexibility when we load the driver module from userspace. Vlaues are 0 - no timeout and N - N seconds timeout

/*Wait queue definitions*/
static DECLARE_WAIT_QUEUE_HEAD(wq);
struct semaphore mr_sem;
static int flag = 0;

u32 write_offset;     // write offset
u32 read_offset;	//read offset

uint intgendriver4_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t intgendriver4_write(struct file *filp, char *buf, size_t count, loff_t *f_pos);
int intgendriver4_open(struct inode *inode, struct file *filp);
int intgendriver4_release(struct inode *inode, struct file *filp);
long intgendriver4_ioctl(struct file *file,	/* ditto */
                         unsigned int ioctl_num,	/* number and param for ioctl */
                         unsigned long ioctl_param); //data from userspace

struct intgendriver4_local {
	dev_t devno;
	int irq;
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;
	struct cdev cdev;
     u32 writers;
};


//////////////////////////////////////////////////////////////////////////////
//
// ISR
//
/////////////////////////////////////////////////////////////////////////////

static irqreturn_t my_mb_task_done (int irq, void *lp) {
     /*Handle the interrupt status register*/
     intr0 = (int*)((struct intgendriver4_local*) lp)->base_addr;
     if (DBG) {
          printk(KERN_DEBUG "Interrupt: %d received!\n", irq );
          printk(KERN_DEBUG "Reg0 at %x\n", intr0);
          printk(KERN_DEBUG "Reg0 set at %d\n", *intr0);
     }
     *intr0 = 0; //this should reset the interrupt flag in reg0
     (DBG) && (printk(KERN_DEBUG "Reg0 reset at %x\n", *intr0));

     // wake up user space process
	(DBG) && (printk(KERN_DEBUG "WaitQueue flag set at %d\n", flag));
	flag = 1;
	if (DBG) {
          printk(KERN_DEBUG "WaitQueue flag updated at %d\n", flag);
          printk(KERN_DEBUG "Waking up process ...\n");
     }
     wake_up_interruptible(&wq);
     return IRQ_HANDLED;
}

struct file_operations intgendriver4_fops = {
     .owner = THIS_MODULE,
     .read = (void*)intgendriver4_read,
     .write = (void*)intgendriver4_write,
     .open = intgendriver4_open,
	.unlocked_ioctl = intgendriver4_ioctl,
     .release = intgendriver4_release
};

static int intgendriver4_probe(struct platform_device *pdev) {
	struct resource *r_irq; /* Interrupt resources */
	struct resource *r_mem; /* IO mem resources */
	struct device *dev = &pdev->dev;
	struct intgendriver4_local *lp = NULL;
	int *intr2;

	int status;
	int rc = 0;
	
	dev_info(dev, "intgendriver4 Device Tree Probing\n");

	/* Get iospace for the device */
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r_mem) {
		dev_err(dev, "invalid address\n");
		return -ENODEV;
	}
	
	lp = (struct intgendriver4_local *) kmalloc(sizeof(struct intgendriver4_local), GFP_KERNEL);
	if (!lp) {
		dev_err(dev, "Cound not allocate intgendriver4 device\n");
		return -ENOMEM;
	}
	
	dev_set_drvdata(dev, lp);
	
	lp->mem_start = r_mem->start;
	lp->mem_end = r_mem->end;

	if (!request_mem_region(lp->mem_start,lp->mem_end - lp->mem_start + 1,DRIVER_NAME)) {
		dev_err(dev, "Couldn't lock memory region at %p\n",(void *)lp->mem_start);
		rc = -EBUSY;
		goto error1;
	}

	lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	if (!lp->base_addr) {
		dev_err(dev, "intgendriver4: Could not allocate iomem\n");
		rc = -EIO;
		goto error2;
	}

	/* Get IRQ for the device */
	r_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!r_irq) {
		dev_info(dev, "no IRQ found\n");
		dev_info(dev, "intgendriver4 at 0x%08x mapped to 0x%08x\n",(unsigned int __force)lp->mem_start,(unsigned int __force)lp->base_addr);
		return 0;
	} 
	lp->irq = r_irq->start;
	
	rc = request_irq(lp->irq, &my_mb_task_done, 0, DRIVER_NAME, lp);
	if (rc) {
		dev_err(dev, "testmodule: Could not allocate interrupt %d.\n",lp->irq);
		goto error3;
	}
	
	lp->devno = MKDEV(MAJOR_NUM,MINOR_NUM);
	dev_info(dev,"devno is 0x%0x, pdev id is %d\n", lp->devno, MINOR_NUM);	
	
	status= register_chrdev(MAJOR_NUM, DRIVER_NAME, &intgendriver4_fops);
	if(status <0) {
          printk("register_Drv\n");
     }
	
	cdev_init(&lp->cdev, &intgendriver4_fops);
	lp->cdev.owner = THIS_MODULE;
	lp->cdev.ops = &intgendriver4_fops;
	cdev_add(&lp->cdev, lp->devno,1);
	if(status < 0) {
		printk("Problem in registering Major\n");
     }
	
	dev_info(dev,"intgendriver4 at 0x%08x mapped to 0x%08x, irq=%d\n",(unsigned int __force)lp->mem_start,(unsigned int __force)lp->base_addr,lp->irq);
	printk(KERN_DEBUG "Interrupt4 ready!\n");
	printk(KERN_DEBUG "Arguments used: DBG=%i; TIMEOUT(s)=%i;\n", DBG, TIMEOUT);
	return 0;
     
     error3:
          free_irq(lp->irq, lp);
     error2:
          release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
     error1:
          kfree(lp);
          dev_set_drvdata(dev, NULL);
          return rc;
}

static int intgendriver4_remove(struct platform_device *pdev) {
	struct device *dev = &pdev->dev;
	struct intgendriver4_local *lp = dev_get_drvdata(dev);
	cdev_del(&lp->cdev);
	unregister_chrdev(lp->devno, DRIVER_NAME);
	free_irq(lp->irq, lp);
	release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	kfree(lp);
	dev_set_drvdata(dev, NULL);
	return 0;
}

uint intgendriver4_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	struct intgendriver4_local *dev = filp->private_data;
	return dev->base_addr;
}

long intgendriver4_ioctl(struct file *file,	/* ditto */
                         unsigned int ioctl_num,	/* number and param for ioctl */
                         unsigned long ioctl_param) {
	switch (ioctl_num) {
		case IOCTL_WAIT_INTERRUPT:
               if (DBG) {
                    printk(KERN_DEBUG "Process %i (%s) is going to sleep!\n",current->pid, current->comm);
                    printk(KERN_DEBUG "Reg0 at %x\n", intr0);
                    printk(KERN_DEBUG "Reg0 set at %d\n", *intr0);
                    printk(KERN_DEBUG "WaitQueue flag set at %d\n", flag);
                    copy_from_user((char *)&args, (char *)ioctl_param, sizeof(args));
                    printk(KERN_DEBUG "intgendriver4, workload size set at %d\n", args.spmm_workload);
               }
			if (TIMEOUT) {
				unsigned long stamp_t = TIMEOUT * HZ;
				wait_event_interruptible_timeout(wq, flag != 0, stamp_t); 
			} else {
				wait_event_interruptible(wq, flag != 0);
			}
			if (DBG) {
                    printk(KERN_DEBUG "Process %i (%s) awoken!\n", current->pid, current->comm);
                    printk(KERN_DEBUG "Reg0 at %x\n", intr0);
                    printk(KERN_DEBUG "Reg0 set at %d\n", *intr0);
                    printk(KERN_DEBUG "WaitQueue flag set at %d\n", flag);
                    printk(KERN_DEBUG "intgendriver4, workload size was %d\n", args.spmm_workload);                    
               }
			flag = 0;
			(DBG) && (printk(KERN_DEBUG "WaitQueue flag reset at %d\n", flag));
			break;
		case IOCTL_SET_WOFFSET:
			write_offset = ioctl_param;
			break;
		case IOCTL_SET_ROFFSET:
			read_offset = ioctl_param;
			break;
	}
	return SUCCESS;
}


ssize_t intgendriver4_write( struct file *filp, char *buf, size_t count, loff_t *f_pos) {
 	char *tmp;
  	struct intgendriver4_local *dev = filp->private_data;
	tmp=buf+count-1;
 	copy_from_user(dev->base_addr,tmp,1);
  	return 1;
}

int intgendriver4_release(struct inode *inode, struct file *filp) {
        return 0;
}

int intgendriver4_open(struct inode *inode, struct file *filp) {
	struct intgendriver4_local *dev;
	dev = container_of(inode->i_cdev, struct intgendriver4_local, cdev);
     filp->private_data = dev;       /* For use elsewhere */
     printk(KERN_DEBUG "intgendriver4 open and ready\n");
	intr0 = (int*)dev->base_addr;
     flag = 0;
     if (DBG) {
          printk(KERN_DEBUG "Reg0 at %x\n", intr0);
          printk(KERN_DEBUG "Reg0 set at %d\n", *intr0);
          printk(KERN_DEBUG "WaitQueue flag set at %d\n", flag);
     }
     return 0;
}

static struct of_device_id intgendriver4_of_match[] = {
	{ .compatible = "kris,intgendriver4-1.01.a", },
	{ /* end of list */ },
};
MODULE_DEVICE_TABLE(of, intgendriver4_of_match);

static struct platform_driver intgendriver4_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = intgendriver4_of_match,
	},
	.probe = intgendriver4_probe,
	.remove = intgendriver4_remove,
};

static int __init intgendriver4_init(void) {
	printk(KERN_DEBUG "Hello interrupts!\n");
	return platform_driver_register(&intgendriver4_driver);
}


static void __exit intgendriver4_exit(void) {
	platform_driver_unregister(&intgendriver4_driver);
	printk(KERN_DEBUG "Goodbye interrupts!\n");
}

module_init(intgendriver4_init);
module_exit(intgendriver4_exit);
