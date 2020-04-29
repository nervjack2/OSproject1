#include<linux/linkage.h>
#include<linux/kernel.h>
#include<linux/ktime.h>
#include<linux/timekeeping.h>

asmlinkage unsigned long long int sys_findtime(void){
	struct timespec time;
	getnstimeofday(&time);
	return time.tv_sec*1000000000+time.tv_nsec;
}
