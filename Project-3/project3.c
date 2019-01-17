/*
	Çağatay Küpeli
	Emre Gürçay
	CS 342
	Project 3
	project.c - Kernel module for project 3
 */
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <linux/vmalloc.h>
#include <asm/pgtable.h>

//License
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Çağatay & Emre");

//Global variables
static int my_arg = 0;

/*
	S_IRUSR: read, write, execute/search by owner
	S_IWUSR: write permission, owner
	S_IRGRP: read permission, group
	S_IWGRP: write permission, group
*/

module_param(my_arg, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

static void is_stack(struct vm_area_struct *vma)
{
	if(vma->vm_flags & VM_STACK){
		printk("Stack Segment start = 0x%lx, end = 0x%lx, size is = %lu \n",vma->vm_start, vma->vm_end,(vma->vm_end - vma->vm_start));
	}
}

static void content(struct task_struct *task){
	struct mm_struct *mm;
	mm = task->mm;
	struct vm_area_struct *vma = 0;


	printk("Code  Segment start = 0x%lx, end = 0x%lx, size is = %lu \n", mm->start_code, mm->end_code, (mm->end_code - mm->start_code));
	printk("Data  Segment start = 0x%lx, end = 0x%lx, size is = %lu \n", mm->start_data, mm->end_data , (mm->end_data - mm->start_data));
	for (vma = mm->mmap ; vma ; vma = vma->vm_next) {
		is_stack(vma);
	}
	printk("Heap  Segment start = 0x%lx, end = 0x%lx, size is = %lu \n", mm->start_brk, mm->brk, ( mm->brk - mm->start_brk));
	printk("Main Arguments start = 0x%lx, end = 0x%lx, size is = %lu \n", mm->arg_start, mm->arg_end , (mm->arg_end - mm->arg_start));
	printk("Enviroment Variables start = 0x%lx, end = 0x%lx, size is = %lu \n", mm->env_start, mm->env_end , (mm->env_end - mm->env_start));
	printk("Number of frames used(rss) = %lu\n",get_mm_rss(mm));
	printk("Total virtual memory used = %lu\n", mm->total_vm);

	int i = 0;
	while(i < 512){
		if(mm->pgd[i].pgd != 0){
			printk("---------------------------------");
			printk("Page Table Content is = 0x%lx\n", mm->pgd[i].pgd);
			printk("P %i\n", (mm->pgd[i].pgd && 0x0000000000000001));
			printk("R/W %i\n", (mm->pgd[i].pgd && 0x0000000000000002));
			printk("U/S %i\n", (mm->pgd[i].pgd && 0x0000000000000004));
			printk("PWT %i\n", (mm->pgd[i].pgd && 0x0000000000000008));
			printk("PCD %i\n", (mm->pgd[i].pgd && 0x0000000000000010));
			printk("A %i\n", (mm->pgd[i].pgd && 0x0000000000000020));
			printk("PS %i\n", (mm->pgd[i].pgd && 0x0000000000000080));
			printk("---------------------------------");
		}
		i++;
	}
}


static int __init project_3_init(void)
{
	//Variables
	int found = 0; //Default false

	printk(KERN_INFO "Enter Project 3\n");
	if(my_arg < 1){
		printk(KERN_INFO "Enter a valid argument\n");
	}
	else{
		struct task_struct *task = current;
		rcu_read_lock();


		for_each_process(task){
			if(task->pid == my_arg){
				printk(KERN_INFO "Pid is found\n");
				found = 1;
				printk(KERN_INFO "Name of the process is: %s\n",(task->comm));
				content(task);
			}
		}

		if(found == 0){
			printk(KERN_INFO "No process with the given id exist!\n");
		}

		rcu_read_unlock();
	}

	return 0;
}

static void __exit project_3_exit(void)
{
	printk(KERN_INFO "Exit Project 3\n");
}

module_init(project_3_init);
module_exit(project_3_exit);
