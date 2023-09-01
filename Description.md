# 基于4.4.131版本设计的云原生场景下的工具集合

## kprobe动态插桩技术
### kprobe: 自定义回调函数，动态插入探测点，内核执行到探测函数时，会调用回调函数。不需要编译内核或模块，重新启动设备等。

- kprobe:提供三种调用回调方式
	- 探测点调用前: pre_handler
	- 探测点调用后: post_handler
	- 内存访问出错: fault_handler
- jprobe: 获取被探测函数的入参值
- kretprobe: 获取被探测函数的返回值

### 注意事项: 

- kprobe可以在一个探测点注册多个，jprobe不行
- 不允许使用jprobe回调函数和kprobe_handler回调函数作为被探测点
- kernel/kprobes.c arch/\*/kernel/kprobes.c do_page_fault notifier_call_chain 不允许被探测
- 内联函数作为探测点，无法保证对该函数的所有实例都注册探测点，gcc会优化
- 有能力修改被探测函数的运行上下文
- 会自动避免处理探测函数时再次调用其他探测点的回调函数，会增加kprobe结构体中nmissed字段数值
- 注册和注销过程中不会使用mutex锁和动态申请内存
- kprobe回调函数运行期间关闭内核抢占，关闭中断，所以回调函数不能调用放弃CPU的函数，信号量、mutex。
- kretprobe通过替换返地址位预定义的trampoline地址实现，栈回溯和gcc内嵌函数\_builtin\_return_address()调用将返回trampoline的地址而不是探测地址
- 函数调用次数和返回次数不同，kretprobe可能不会达到预期结果
- 当进入会退出一个函数时，CPU运行在非当前任务所有的栈，对其注册kretprobe会导致不可预料结果，x86_64 注册\_switch_to()

### kprobe原理
1. 验证被探测地址合法性
2. 保存被探测指令
3. 替换被探测指令
4. 启动kprobe探测
5. 触发int3(未定义指令)异常
6. 调用pre_handler回调
7. 设置(模拟)单步执行
8. 触发debug异常
9. 调用post_handler回调
10. 恢复正常上下文
11. 结束

- 用户注册一个探测点后，kprobe先备份探测点对应指令，原始指令入口替换成断点指令
- CPU流程执行到断点指令时，触发trap，保存当前CPU寄存器信息并调用对应trap处理函数，设置kprobe调用状态，调用用户注册的pre_handler回调函数，kprobe向函数传递注册的struct kprobe结构地址和保存的CPU寄存器信息
- kprobe单步执行前面拷贝被探测指令，异常处理流程中模拟函数执行
- 单步执行完成后，kprobe执行用户注册的post_handler回调函数
- 回到被探测指令之后，正常执行

##
