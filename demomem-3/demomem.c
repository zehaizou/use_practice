#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define BUF_SIZE   1024
#define MAJOR_NUM  168
#define MINOR_NUM  0

struct demo_device{
    char *buffer;
    struct cdev cdev;
    struct class *cls;
    struct device *device;
};

struct demo_device *demo_dev;

static int demo_open(struct inode *node, struct file *file)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    file->private_data = (void *)demo_dev;
    return 0;
}	

static int demo_release(struct inode *node, struct file *file)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    return 0;
}

static ssize_t demo_read(struct file *file, char __user *buf, size_t size, loff_t *pos)
{
    int ret;
    int read_bytes;
    struct demo_device *demo = file->private_data;
    char *kbuf = demo->buffer + *pos;

    printk(KERN_INFO "Enter: %s\n",__func__);
    if(*pos >= BUF_SIZE) 
        return 0;

    if(size > (BUF_SIZE-*pos))
        read_bytes = BUF_SIZE - *pos;
    else
        read_bytes = size;

    ret = copy_to_user(buf, kbuf, read_bytes);
    if(ret != 0)
        return -EFAULT;
    *pos += read_bytes;

    return read_bytes;
}

static ssize_t demo_write(struct file *file, const char __user *buf, size_t size, loff_t *pos)
{
    int ret;
    int write_bytes;
    struct demo_device *demo = file->private_data;
    char *kbuf = demo->buffer + *pos;

    printk(KERN_INFO "Enter: %s\n",__func__);
    if(*pos >= BUF_SIZE) 
        return 0;

    if(size > (BUF_SIZE-*pos))
        write_bytes = BUF_SIZE - *pos;
    else
        write_bytes = size;

    ret = copy_from_user(kbuf, buf, write_bytes);
    if(ret != 0)
        return -EFAULT;
    *pos += write_bytes;

    return write_bytes;
}

static struct file_operations demo_opration = {
    .open = demo_open,
    .release = demo_release,
    .read = demo_read,
    .write = demo_write,
};

static int __init demo_init(void)
{    
    int ret = -1;
    dev_t dev_no;

    demo_dev = (struct demo_device *)kmalloc(sizeof(struct demo_device), GFP_KERNEL);
    if(!demo_dev) {
        printk(KERN_ERR "failed to malloc demo_device\n");
        ret = -ENOMEM;
        goto ERROR_MALLOC_DEVICE;
    }

    demo_dev->buffer = (char *)kmalloc(BUF_SIZE,GFP_KERNEL);
    if(!demo_dev->buffer) {
        printk(KERN_ERR "malloc %d bytes failed\n", BUF_SIZE);
        ret = -ENOMEM;
        goto ERROR_MALLOC_BUFFER;
    }
    memset(demo_dev->buffer, 0x00, BUF_SIZE);

    dev_no = MKDEV(MAJOR_NUM,MINOR_NUM);
    printk(KERN_INFO "Enter: %s\n",__func__);
    //init for demo char
    cdev_init(&demo_dev->cdev,&demo_opration);
    //register device number
    ret = register_chrdev_region(dev_no,1,"demomem");
    if(ret<0) {
        ret = alloc_chrdev_region(&dev_no,0,1,"demomem");
        if(ret < 0) { 
            printk(KERN_ERR "failed to register device number\n");
            goto ERROR_CHRDEV_REGION;
        }
    }
    ret = cdev_add(&demo_dev->cdev,dev_no,1);
    if(ret<0) {
        printk(KERN_ERR "cdev add failed\n");
        goto ERROR_CDEV_ADD;
    } 
    //create a demomem class in /sys/class/demo/
    demo_dev->cls = class_create(THIS_MODULE,"demo");
    if(IS_ERR(demo_dev->cls)) {
        ret = PTR_ERR(demo_dev->cls);
        goto ERROR_CLASS_CEATE;
    } 
    //create a demomem device in /sys/class/demo/demomem
    demo_dev->device = device_create(demo_dev->cls,NULL,dev_no,NULL,"demomem");
    if(IS_ERR(demo_dev->device)) {
        ret = PTR_ERR(demo_dev->device);
        goto ERROR_DEVICE_CREATE;
    }
    printk(KERN_INFO "demo init done\n");
   
    return 0;
ERROR_DEVICE_CREATE:
    class_destroy(demo_dev->cls);
ERROR_CLASS_CEATE:
    cdev_del(&demo_dev->cdev);
ERROR_CDEV_ADD:
    unregister_chrdev_region(dev_no,1);
ERROR_CHRDEV_REGION:
    kfree(demo_dev->buffer);
    demo_dev->buffer = NULL;
ERROR_MALLOC_BUFFER:
    kfree(demo_dev);
    demo_dev = NULL;
ERROR_MALLOC_DEVICE:
    return ret;
}

static void __exit demo_exit(void)
{
    printk(KERN_INFO "Enter: %s\n",__func__);
    cdev_del(&demo_dev->cdev);
    unregister_chrdev_region(MKDEV(MAJOR_NUM,MINOR_NUM),1);
    
    kfree(demo_dev);
    demo_dev = NULL;
    kfree(demo_dev->buffer);
    demo_dev->buffer = NULL;
    printk(KERN_INFO "demo exit done\n");
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_AUTHOR("ZZH");
MODULE_LICENSE("GPL");
