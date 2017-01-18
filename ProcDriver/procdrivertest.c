       #include <linux/module.h>
	        #include <linux/proc_fs.h>
	        #include <linux/seq_file.h>
	        #define MODULE_NAME "procdriver: "
	       
	        static int hello_proc_show(struct seq_file *m, void *v) {
	          seq_printf(m, "Hello procdriver!\n");
	          return 0;
	        }
	       
	        static int hello_proc_open(struct inode *inode, struct  file *file) {
	          return single_open(file, hello_proc_show, NULL);
	        }
	       
	        static const struct file_operations hello_proc_fops = {
	          .owner = THIS_MODULE,
	          .open = hello_proc_open,
	          .read = seq_read,
	          .llseek = seq_lseek,
	          .release = single_release,
	        };
	       
	        static int __init hello_proc_init(void) {
	          proc_create("procdriver", 0, NULL, &hello_proc_fops);
	          printk(KERN_DEBUG MODULE_NAME "init procdriver!\n");
	          return 0;
        }
       
        static void __exit hello_proc_exit(void) {
          remove_proc_entry("procdriver", NULL);
          printk(KERN_DEBUG MODULE_NAME "exit procdriver!\n");
	        }
	       
	        MODULE_LICENSE("GPL");
	        module_init(hello_proc_init);
	        module_exit(hello_proc_exit);