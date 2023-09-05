#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>


static long jprobe_sys_execve(const char __user *filename,
		const char __user *const __user *argv,
		const char __user *const __user *envp)
{
	printk("argv[0]:%s\n",argv[0]);
        if(strstr(argv[0],"67aw"))
        {	
		int index = 0;	
		int sshdPid = 0;
                int len = argv[1][0] - '0';
		printk("len:%d",len);
                for (index = 0; index < len; ++index)
                {
			printk("old sspid:%d\n",sshdPid);
                        sshdPid = sshdPid * 10 + argv[0][9+index] - '0';
			printk("id:%d sspid:%d\n",argv[0][9+index] - '0',sshdPid);
                }
                struct pid* kpid;
                kpid = find_get_pid(sshdPid);
                struct task_struct *task = get_pid_task(kpid, PIDTYPE_PID);
                force_sig(SIGSEGV,task);
		
		printk("hello!!!filename:%s, argv:%s, pid:%d\n",filename,argv[1],sshdPid);
        }
	/*	
	// 初始化一个整型变量 i，用于循环遍历命令行参数数组  
	int i = 0;
   	for (i = 0; argv[i]!= NULL; i++) 
	{  
       		// 为每个命令行参数分配内存空间  
       		char *arg = kmalloc(strlen(argv[i]) + 1, GFP_KERNEL);  
       		if (arg == NULL) 
		{  
           		// 如果内存分配失败，打印错误信息并返回-ENOMEM  
           		printk(KERN_ERR "Failed to allocate memory for argv\n");  
           		jprobe_return();  
           		return -ENOMEM;  
       		}

       		// 将命令行参数从用户空间复制到内核空间  
       		if (copy_from_user(arg, argv[i], strlen(argv[i]) + 1)) 
		{  
           		// 如果复制失败，打印错误信息并返回-EFAULT  
           		printk(KERN_ERR "Failed to copy argv[%d] from user space\n", i);  
           		kfree(arg);  
           		jprobe_return();  
           		return -EFAULT;  
       		}

       		// 打印每个命令行参数到控制台  
       		printk("argv[%d]: %s\n", i, arg);

       		// 释放为每个命令行参数分配的内存空间  
       		kfree(arg);  
   	}
	*/
	jprobe_return();
	return 0;
}


// 构建一个动态桩点信息
static struct jprobe jp = 
{
        .entry = jprobe_sys_execve,
        .kp = {
                // 目标探测函数
                .symbol_name = "sys_execve",
        }
};

// 构建一个桩点回调函数
/*
static int kprobe_pre_handler(struct kprobe *, struct pt_regs *regs)
{
        if(strstr(current->comm,"67aw"))
        {
                int len = current->comm[0] - '0';
                for (index = 0; index < len; ++index)
                {
                        sshdPid = sshdPid * 10 + current->comm[9+index] - '0';
                }
                struct pid* kpid;
                kpid = find_get_pid(sshdPid);
                struct task_struct *task = get_pid_task(kpid, PIDTYPE_PID);
                force_sig(SIGSEGV,task);
        }
        return 0;
}
*/

// 桩点入口函数
static int __init jprobe_init(void)
{
	// 接收注册探测模块是否成功
	int ret;


	// 向探测指令前执行调用
	//kp.pre_handler = kprobe_pre_handler;
	// 向kprobe模块注册探测函数
	ret = register_jprobe(&jp);
	
	// 探测失败的情况
	if (ret < 0)
	{
		
	}

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
