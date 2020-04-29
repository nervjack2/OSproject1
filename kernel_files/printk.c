#include<linux/kernel.h>
#include<linux/linkage.h>


asmlinkage void sys_printk(char *msg){
	printk("[Project1] %s",msg);
}
