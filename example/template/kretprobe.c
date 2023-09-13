#i`nclude <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>


// 自定义数据结构
struct test = 
{
	int test;
};


static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	// 预置回调处理函数
}

static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	// 结束回调处理函数
}


static struct kretprobe kret = 
{
	.handler = ret_handler,
	.entry_handler = entry_handler,
	.data_size = sizeof(struct test),
	.maxactive = 20,
	.kp = {
        	// 目标探测函数
        	symbol_name = "syscall",
        }
};

// 桩点入口函数
static int __init kretprobe_init(void)
{
	// 接收注册探测模块是否成功
	int ret;


	// 向jprobe模块注册探测函数
	ret = register_kretprobe(&kret);
	
	// 探测失败的情况
	if (ret < 0)
	{
		printk("sys_execve kretprobe register failed");		
	}
	printk("sys_execve kretprobe register sucessful");

	return 0;

}

// 桩点出口函数
static void __exit kretprobe_exit(void)
{
	unregister_kretprobe(&kret);
	printk(KERN_INFO "kprobe exit");
}

module_init(kretprobe_init)
module_exit(kretprobe_exit)
MODULE_LICENSE("GPL");
