#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

SYSCALL_DEFINE1(poke, void*, p) {
		
	unsigned long addr = (unsigned long)p;
	printk("ADDRESS LOOKING FOR IS %lu",addr);

	struct task_struct *ts = get_current();
	struct mm_struct *mm = ts->mm;
	struct vm_area_struct *loc = mm->mmap;
	
	while (loc->vm_next) {
		printk("THIS VMA IS FROM %lu TO %lu",loc->vm_start,loc->vm_end);
		if ((unsigned long)loc->vm_start <= addr && (unsigned long)loc->vm_end >= addr) {
			printk("ADDRESS WAS FOUND");
			return (unsigned short)loc->vm_flags;
		}
		loc = loc->vm_next;
	}

	// unsigned long found_addr = (unsigned long)(find_vma(ts->mm,addr))->vm_flags;
	// printk("VMA FOUND IS %lu \n",found_addr);
	// return -2 if not found
	return -2;
}
