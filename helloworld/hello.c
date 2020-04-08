#include <linux/init.h>
#include <linux/module.h>

//enter function
static int __init hello_init(void)
{
    printk(KERN_INFO "Enter : %s\n",__func__);
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Enter : %s\n",__func__);
}

//driver enter and exit
module_init(hello_init);
module_exit(hello_exit);

//remark information
MODULE_AUTHOR("zzh");
//MODULE_LICENSE("");
