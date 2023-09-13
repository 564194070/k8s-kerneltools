#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>


// 构建一个动态桩点信息
static struct kprobe kp = 
{
	// 目标探测函数
	.symbol_name = "syscall",
};


static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	// 探测指令执行前预置工作
}

static void handler_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	// 探测指令执行后工作
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	// 生成异常调用此处理函数
}


// 桩点入口函数
static int __init kprobe_init(void)
{

	// 接收注册探测模块是否成功
	int ret;
	// 注册kprobe的回调函数
	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;
	// 向jprobe模块注册探测函数
	ret = register_kprobe(&kp);
	
	// 探测失败的情况
	if (ret < 0)
	{
		printk("sys_execve kprobe register failed");		
	}
	printk("sys_execve kprobe register sucessful");

	return 0;

}

// 桩点出口函数
static void __exit kprobe_exit(void)
{
	unregister_kprobe(&kp);
	printk(KERN_INFO "kprobe exit");
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
