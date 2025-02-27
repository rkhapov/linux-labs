#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/pid.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/mm_types.h>
#include <linux/printk.h>

MODULE_DESCRIPTION("List current process vmas");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

static int target_pid = 0;

module_param(target_pid, int, 0000);
MODULE_PARM_DESC(target_pid, "A target process pid");

noinline void print_mmap(struct task_struct*);
noinline struct task_struct *find_task_by_pid(int);

noinline void print_mmap(struct task_struct *p) {
	printk(KERN_INFO "Process pid: %d, comm = %s\n", p->pid, p->comm);

	struct mm_struct *mm = get_task_mm(p);
	if (mm == NULL) {
		printk(KERN_INFO "No task mm set\n");
		return;
	}
	struct vm_area_struct *vma;
	VMA_ITERATOR(vmi, mm, 0);

	mmap_read_lock(mm);
	for_each_vma(vmi, vma) {
		printk(KERN_INFO "[%lx..%lx)", vma->vm_start, vma->vm_end);
	}
	mmap_read_unlock(mm);

	mmput(mm);
}

noinline struct task_struct *find_task_by_pid(int pid) {
	struct task_struct *p;
	for_each_process(p) {
		if (p->pid == pid) {
			return p;
		}
	}

	return NULL;
}

static int my_proc_init(void)
{
	if (target_pid <= 0) {
		printk(KERN_INFO "Target pid is invalid\n");
		return 0;
	}

	struct task_struct *t = find_task_by_pid(target_pid);
	if (t != NULL) {
		print_mmap(t);
	} else {
		printk(KERN_INFO "Can't find task with pid %d\n", target_pid);
	}

	return 0;
}

static void my_proc_exit(void)
{
	pr_debug("BB");
}

module_init(my_proc_init);
module_exit(my_proc_exit);