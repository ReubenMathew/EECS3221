
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

// printk is sent to stdout as well as kernel log buffer where it can be read with `dmesg`


int init(void){
    printk(KERN_INFO "LOADING KERNEL MODULE\n");   
    printk()
    return 0; // the module entryh point must return 0 to represent success, any other val means failure 
}

void exit(void){
    printk(KERN_INFO "REMOVING KERNEL MODULE\n");
    return;
}

int failure(void){
    printk(KERN_INFO "KERNEL MODULE ERROR!\n");
    return -1;
}

module_init(init);
module_exit(exit);
