#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/fd_table.h>
#include <linux/nsproxy.h>
#include <linux/path.h>

MODULE_LICENSE("GPL");

static struct task_struct * kthread = NULL;

static int
thread_fn(void * data){
    struct task_struct * current_task = current;
    struct fs_struct * file_system_struct = current_task->fs;
    struct files_struct * file_table = current_task->files;
    struct nsproxy * namespaces = current->nsproxy;
    printk("fs: %ul\nfiles: %ul\nnsproxy: %ul\n", file_system_struct, file_table, namespaces);
    while(!kthread_should_stop()){
        schedule();
    }
    return 0;
}

static int vfs_init(){

    printk(KERN_INFO "Loaded kernel_memory module\n");
    kthread = kthread_create(thread_fn, NULL, "kthread_vfs");
    if (IS_ERR(kthread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(kthread);
    }
    
    wake_up_process(kthread);

    return 0;

}
static void vfs_exit(){
    kthread_stop(kthread);
    printk(KERN_INFO "Unloaded kernel_memory module\n");

}



module_init(vfs_init);
module_exit(vfs_exit);