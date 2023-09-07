#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>


static long callback()
{
	jprobe_return();
	return 0;
}


// 构建一个动态桩点信息
static struct jprobe jp = 
{
        .entry = callback,
        .kp = {
                // 目标探测函数
                .symbol_name = "syscall",
        }
};


// 桩点入口函数
static int __init jprobe_init(void)
{
	// 接收注册探测模块是否成功
	int ret;


	// 向jprobe模块注册探测函数
	ret = register_jprobe(&jp);
	
	// 探测失败的情况
	if (ret < 0)
	{
		printk("sys_execve jprobe register failed");		
	}
	printk("sys_execve jprobe register sucessful");

	return 0;

}

// 桩点出口函数
static void __exit jprobe_exit(void)
{
	unregister_jprobe(&jp);
	printk(KERN_INFO "kprobe exit");
}

module_init(jprobe_init)
module_exit(jprobe_exit)
MODULE_LICENSE("GPL");
