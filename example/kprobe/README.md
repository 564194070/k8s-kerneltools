# Kprobe结构体和API介绍

## struct kprobe

- struct hlist_node hlist：被用于kprobe全局hash，索引值为被探测点的地址；
- struct list_head list：用于链接同一被探测点的不同探测kprobe；
- kprobe_opcode_t \*addr：被探测点的地址；
- const char \*symbol_name：被探测函数的名字；
- unsigned int offset：被探测点在函数内部的偏移，用于探测函数内部的指令，如果该值为0表示函数的入口；
- kprobe_pre_handler_t pre_handler：在被探测点指令执行之前调用的回调函数；
- kprobe_post_handler_t post_handler：在被探测指令执行之后调用的回调函数；
- kprobe_fault_handler_t fault_handler：在执行pre_handler、post_handler或单步执行被探测指令时出现内存异常则会调用该回调函数；
- kprobe_break_handler_t break_handler：在执行某一kprobe过程中触发了断点指令后会调用该函数，用于实现jprobe；
- kprobe_opcode_t opcode：保存的被探测点原始指令；
- struct arch_specific_insn ainsn：被复制的被探测点的原始指令，用于单步执行，架构强相关（可能包含指令模拟函数）；
- u32 flags：状态标记。

## API接口

int register_kprobe(struct kprobe \*kp)      			//向内核注册kprobe探测点
void unregister_kprobe(struct kprobe \*kp)   			//卸载kprobe探测点
int register_kprobes(struct kprobe \*\*kps, int num)     	//注册探测函数向量，包含多个探测点
void unregister_kprobes(struct kprobe \*\*kps, int num)  	//卸载探测函数向量，包含多个探测点
int disable_kprobe(struct kprobe \*kp)       			//临时暂停指定探测点的探测
int enable_kprobe(struct kprobe \*kp)        			//恢复指定探测点的探测
