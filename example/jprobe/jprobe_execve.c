#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

/*
	功能简介：通过sys_execve插桩，比对程序名，在执行含有关键字的特定的程序时，通过阅读入参，杀死对应的进程
	触发时机: ./jprobe_test 5_test_12345
	触发条件: 可执行程序名中含有test 参数列表中第一个参数意义如下 5(进程号有几位)_test(验证字符串)_12345(进程PID)

*/

static long jprobe_sys_execve(const char __user *filename,
							  const char __user *const __user *argv,
							  const char __user *const __user *envp)
{
	if (strstr(argv[0], "test"))
	{
		int index = 0;					 // 遍历参数列表的索引下标
		int sshdPid = 0;				 // 被杀进程的PID信息
		int len = argv[1][0] - '0';		 // 进程PID长度
		struct pid *kpid = NULL;		 // 保存目标进程的PID信息
		struct task_struct *task = NULL; // 保存目标进程的任务信息

		// 遍历argv[1] 读取
		for (index = 0; index < len; ++index)
		{
			sshdPid = sshdPid * 10 + argv[0][7 + index] - '0';
		}

		kpid = find_get_pid(sshdPid);
		task = get_pid_task(kpid, PIDTYPE_PID);
		// 发送信号关闭目标任务
		force_sig(SIGSEGV, task);

		printk("filename:%s, argv:%s, pid:%d closed!\n", filename, argv[1], sshdPid);
	}
	// 返回系统调用
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
		}};

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
