#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <asm/siginfo.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");



#define PROCFS_MAX_SIZE         1024
#define PROCFS_TESTLEVEL        "battery_test_csh"
#define PROCFS_NOTIFYPID        "battery_notify_csh"
#define PROCFS_THRESHOLD        "battery_threshold_csh"
#define CHR_DEV_NAME "sample"  // device file name
#define CHR_DEV_MAJOR 574  //Device file's Major number
#define DELAY(msec) ((msec)* HZ /10)

/* Declaration of variables used in this module */


static int level = 99;
static int test_level = 0;                      //indicates level of battery remain.
static int notify_pid = -1;
static int threshold = -1;

/* End of declaration */



/* Declaration of ancillary variables */
static struct timer_list timer;
static char procfs_buffer[PROCFS_MAX_SIZE];     
static unsigned long procfs_buffer_size = 0;    //size of receive side buffer
static struct proc_dir_entry *proc_entry1,*proc_entry2,*proc_entry3;       //indicates procfs entry.

/* End of declaration */



/*
        Implementation of procfs write function
*/
static int test_level_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
	

        int status = 0;
        int requested;

        procfs_buffer_size = len;

	printk("my kernel test_value : %d\n",test_level);
        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }

		printk("my kernel test_value1 : %d\n",test_level);
        
	/* write data to the buffer */
        if ( copy_from_user(procfs_buffer, user_space_buffer, procfs_buffer_size))
	{
                return -EFAULT;
        }

	printk("my kernel test_value2 : %d\n",test_level);
        
	status  = kstrtoint(procfs_buffer, 10, &requested);
	printk("my status : %d\n",status);
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...)\n");
                return -ENOMEM;
        }
        // validate level value.
        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        }
        // accept value.
      test_level = requested;
	printk("my kernel test_value3 : %d\n",test_level);
        // *off += procfs_buffer_size; // not necessary here!

        return procfs_buffer_size;



}

/*
        Implementation of procfs read function
*/
static int test_level_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{

        int ret = 0;
        int flag = 0;

        if(*off < 0) *off = 0;

        snprintf(procfs_buffer, 16, "%d\n", test_level);
        procfs_buffer_size = strlen(procfs_buffer);

        if(*off > procfs_buffer_size){
                return -EFAULT;
        }else if(*off == procfs_buffer_size){
                return 0;
        }

        if(procfs_buffer_size - *off > count)
                ret = count;
        else
                ret = procfs_buffer_size - *off;




        flag = copy_to_user(user_space_buffer, procfs_buffer + (*off), ret);
	printk("my read flag : %d\n",flag);

        if(flag < 0)
                return -EFAULT;

        *off += ret;

       return ret;


}
//threshold 
static int threshold_write(struct file *filp,int* user_space_buffer,size_t count,loff_t *off)
{

	if(copy_from_user(&threshold,user_space_buffer,count)==0)
	{
		printk("complete compy_from_user_ threshold %d\n",threshold);
		return count;

	}
	else
	{
		printk("error _threshold_wirte \n");
		return -1;
	}

}
static int threshold_read(struct file * filp,int* user_space_buffer,size_t count,loff_t *off)
{

	if(copy_to_user(user_space_buffer,&threshold,count)==0)
	{
		printk("complete compy_to_user_ threshold %d\n",threshold);
		return count;

	}
	else
	{
		printk("error _threshold_wirte \n");
		return -1;
	}

}


//notify 
static int notify_write(struct file *filp, pid_t* user_space_buffer,size_t count,loff_t *off)
{

	if(copy_from_user(&notify_pid,user_space_buffer,count)==0)
	{
		printk("complete compy_from_user_ threshold %d\n",notify_pid);
		return count;

	}
	else
	{
		printk("error _threshold_wirte \n");
		return -1;
	}


}

static int notify_read(struct file *filp, pid_t* user_space_buffer,size_t count,loff_t *off)
{

	if(copy_to_user(user_space_buffer,&notify_pid,count)==0)
	{
		printk("complete compy_to_user_ threshold %d\n",notify_pid);
		return count;

	}
	else
	{
		printk("error _threshold_wirte \n");
		return -1;
	}

}
/*
        Configuration of file_operations

        This structure indicate functions when read or write operation occured.
*/
static const struct file_operations my_proc_fops = {
        .write = test_level_write,
        .read = test_level_read,
};
static const struct file_operations my_proc_fops_threshold ={
	.write = threshold_write,
	.read = threshold_read,
};

static const struct file_operations my_proc_fops_notify ={

	.write = notify_write,
	.read = notify_read,
};



//device Driver's function


int chr_open(struct inode *inode, struct file *filp)
{
                int number=MINOR(inode->i_rdev); //부번호를 NUMBER에 저장
                printk("Virtual Character Device Open: Minor Number is %d\n",number);
                return 0;
}

ssize_t chr_write(struct file *flip, const int * buf, size_t count, loff_t *f_pos)
{
        copy_from_user(&test_level,buf,count); //user로 부터 커널영역에 값을 읽어 온다.
        printk("write _testlevel: %d\n",test_level);
}

ssize_t chr_read(struct file *filp, const int *buf, size_t count, loff_t *f_pos)
{
        //printk("read data: %s\n",buf);
        
                copy_to_user(buf , &test_level , count);
       
   
        return count;
}

int chr_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
        switch(cmd)
        {
                case 0: printk("cmd value is %d\n",cmd);break;
                case 1: printk("cmd value is %d\n",cmd);break;
        }
        return 0;
}

int chr_release(struct inode *inode, struct file *flip)
{
        printk("Virtual Character Device Release\n");
        return 0;
}

struct file_operations chr_fops =
{
        owner : THIS_MODULE,
        unlocked_ioctl :chr_ioctl,
        write : chr_write,
        read : chr_read,
        open : chr_open,
        release : chr_release
};
/*
int sample_init(void)
{
        int registration;
        printk("Registration Character Device to Kernel\n");
        registration = register_chrdev(CHR_DEV_MAJOR,CHR_DEV_NAME,&chr_fops);
        if(registration <0)
                return registration;
        printk("Major number: %d\n", registration);
        return 0;
}

void sample_cleanup(void)
{
        printk("Unregistration Character Device to Kernel\n");
        unregister_chrdev(CHR_DEV_MAJOR,CHR_DEV_NAME);
}
*/
static void mysignal_function(void)
{
	static int index=0;
	int val=0;
	struct task_struct *mytask=NULL;
	printk("test_level : %d, threshold : %d, pid : %d \n",test_level,threshold,notify_pid);
	if(notify_pid != -1)
	{
		mytask=pid_task(find_pid_ns(notify_pid, &init_pid_ns),PIDTYPE_PID);
		if( mytask!=NULL)
		{
			if(test_level <threshold)
			{
				val=send_sig_info(SIGUSR1,SEND_SIG_NOINFO,mytask);
			}
			else
			{
				val=send_sig_info(SIGUSR2,SEND_SIG_NOINFO,mytask);
				
			}
		}
		else
		{
			printk("struct build fail\n");
		}
	}
	else
	{
		printk("app process not launch!");
	}
	timer.expires = get_jiffies_64() + DELAY(10);
	timer.function = mysignal_function;
	add_timer(&timer);

}




/*
        This function will be called on initialization of  kernel module
*/
int init_module(void)
{

        int ret = 0;
        int registration;
        printk("Registration Character Device to Kernel\n");
        registration = register_chrdev(CHR_DEV_MAJOR,CHR_DEV_NAME,&chr_fops);
        if(registration <0)
                return registration;
        printk("Major number: %d\n", registration);
	
        proc_entry1 = proc_create(PROCFS_TESTLEVEL, 0666, NULL, &my_proc_fops);
	proc_entry2 = proc_create(PROCFS_THRESHOLD,0666,NULL, &my_proc_fops_threshold);
	proc_entry3 = proc_create(PROCFS_NOTIFYPID,0666,NULL, &my_proc_fops_notify);
        if(proc_entry1 == NULL )
        {	
		printk("PROC_ENTRY1 ERROR \n");
                return -ENOMEM;
        }
	if(proc_entry2 ==NULL)
	{
		printk("proc_entry2 error \n");
		return -ENOMEM;
	}
	if(proc_entry3 ==NULL)
	{
		printk("proc_entry3 error \n");
		return -ENOMEM;
	}
	init_timer(&timer);
	timer.expires =get_jiffies_64() + DELAY(10);
	timer.function = mysignal_function;
	add_timer(&timer);
        return ret;

}

/*
        This function will be called on cleaning up of kernel module
*/
void cleanup_module(void)
{

	del_timer(&timer);
        printk("Unregistration Character Device to Kernel\n");
        unregister_chrdev(CHR_DEV_MAJOR,CHR_DEV_NAME);

        remove_proc_entry(PROCFS_TESTLEVEL, proc_entry1);
	remove_proc_entry(PROCFS_THRESHOLD, proc_entry2);
	remove_proc_entry(PROCFS_NOTIFYPID, proc_entry3);
}
