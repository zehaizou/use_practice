#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define BUF_SIZE   1024
#define MAJOR_NUM  168
#define MINOR_NUM  0

struct demo_device{
    char buffer[BUF_SIZE];
    struct cdev cdev;
};

static struct demo_device demo_dev;

static int demo_open(struct inode *node, struct file *file)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    return 0;
}	

static int demo_release(struct inode *node, struct file *file)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    return 0;
}

static ssize_t demo_read(struct file *file, char __user *buf, size_t size, loff_t *pos)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    return 0;
}

static struct file_operations demo_opration = {
    .open = demo_open,
    .release = demo_release,
    .read = demo_read,
    //.write
};

static int __init demo_init(void)
{    
    int ret = -1;
    dev_t dev_no;

    dev_no = MKDEV(MAJOR_NUM,MINOR_NUM);
    printk(KERN_INFO "Enter: %s\n",__func__);
    //init for demo char
    cdev_init(&demo_dev.cdev,&demo_opration);
    //register device number
    ret = register_chrdev_region(dev_no,1,"demomem");
    if(ret<0) {
        printk(KERN_ERR "failed to register device number\n");
        return ret;
    }
    ret = cdev_add(&demo_dev.cdev,dev_no,1);
    if(ret<0) {
        printk(KERN_ERR "cdev add failed\n");
        unregister_chrdev_region(dev_no,1);
        return ret;
    } 
    printk(KERN_INFO "demo init done\n");
   
    return 0;
}

static void __exit demo_exit(void)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    cdev_del(&demo_dev.cdev);
    unregister_chrdev_region(MKDEV(MAJOR_NUM,MINOR_NUM),1);
    printk(KERN_INFO "demo exit done\n");
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_AUTHOR("ZZH");
MODULE_LICENSE("GPL");
