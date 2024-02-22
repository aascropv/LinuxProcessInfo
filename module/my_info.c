#include "my_info.h"

#define PROCFS_ENTRY_FILENAME "my_info"

static struct proc_dir_entry *my_proc_file;

static int proc_show_flag = 5;

static void version_show(struct seq_file *m)
{
    seq_printf(m, "Linux Version %s\n", utsname()->release);
}

static void cpuinfo_show(struct seq_file *m)
{
    u32 cpu = 0;
    struct cpuinfo_x86 *c;

    for_each_online_cpu(cpu)
    {
        c = &cpu_data(cpu);

        seq_printf(m, "processor\t: %d\n", c->logical_proc_id);
        seq_printf(m, "model name\t: %s\n", c->x86_model_id);
        seq_printf(m, "physical id\t: %d\n", c->phys_proc_id);
        seq_printf(m, "core id\t\t: %d\n", c->cpu_core_id);
        seq_printf(m, "cpu cores\t: %d\n", c->x86_max_cores);
        seq_printf(m, "cache size\t: %d KB\n", c->x86_cache_size);
        seq_printf(m, "clflush size\t: %d\n", c->x86_clflush_size);
        seq_printf(m, "cache_alignment\t: %d\n", c->x86_cache_alignment);
        seq_printf(m, "address sizes\t: %d bits physical, %d bits virtual\n", c->x86_phys_bits, c->x86_virt_bits);
        seq_puts(m, "\n");
    }
}

static void meminfo_show(struct seq_file *m)
{
    struct sysinfo i;
    unsigned long pages[NR_LRU_LISTS];
    int lru;

    si_meminfo(&i);
    // si_swapinfo(&i);

    for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
        pages[lru] = global_node_page_state(NR_LRU_BASE + lru);

    seq_printf(m, "MemTotal\t: %ld kB\n", i.totalram);
    seq_printf(m, "MemFree\t\t: %ld kB\n", i.freeram);
    seq_printf(m, "Buffers\t\t: %ld kB\n", i.bufferram);
    seq_printf(m, "Active\t\t: %ld kB\n", pages[LRU_ACTIVE_ANON] + pages[LRU_ACTIVE_FILE]);
    seq_printf(m, "Inactive\t: %ld kB\n", pages[LRU_INACTIVE_ANON] + pages[LRU_INACTIVE_FILE]);
    seq_printf(m, "Shmem\t\t: %ld kB\n", i.sharedram);
    seq_printf(m, "Dirty\t\t: %ld kB\n", global_node_page_state(NR_FILE_DIRTY));
    seq_printf(m, "Writeback\t: %ld kB\n", global_node_page_state(NR_WRITEBACK));
    seq_printf(m, "KernelStack\t: %ld kB\n", global_zone_page_state(NR_KERNEL_STACK_KB));
    seq_printf(m, "PageTables\t: %ld kB\n", global_zone_page_state(NR_PAGETABLE));
    seq_puts(m, "\n");
}

static void time_show(struct seq_file *m)
{
    struct timespec64 uptime;
    struct timespec64 idle;
    u64 nsec = 0;
    u32 rem = 0;
    int j;

    for_each_possible_cpu(j)
    nsec += (__force u64) kcpustat_cpu(j).cpustat[CPUTIME_IDLE];

    get_monotonic_boottime(&uptime);

    idle.tv_sec = div_u64_rem(nsec, NSEC_PER_SEC, &rem);
    idle.tv_nsec = rem;
    seq_printf(m, "Uptime\t\t: %lu.%02lu (s)\n", (unsigned long) uptime.tv_sec, (uptime.tv_nsec / (NSEC_PER_SEC / 100)));
    seq_printf(m, "Idletime\t: %lu.%02lu (s)\n", (unsigned long) idle.tv_sec, (idle.tv_nsec / (NSEC_PER_SEC / 100)));
    seq_puts(m, "\n");
}

static int proc_show(struct seq_file *m, void *v)
{
    if (proc_show_flag == 1)
    {
        version_show(m);
    }
    else if (proc_show_flag == 2)
    {
        cpuinfo_show(m);
    }
    else if (proc_show_flag == 3)
    {
        meminfo_show(m);
    }
    else if (proc_show_flag == 4)
    {
        time_show(m);
    }
    else if (proc_show_flag == 5)
    {
        seq_puts(m, "\n=============Version=============\n");
        version_show(m);
        seq_puts(m, "\n=============CPU================\n");
        cpuinfo_show(m);
        seq_puts(m, "\n============Memory==============\n");
        meminfo_show(m);
        seq_puts(m, "\n=============Time==============\n");
        time_show(m);
    }

    return 0;
}

static int procfile_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}


static ssize_t procfile_write(struct file *file, const char __user *buffer, size_t len, loff_t *off)
{
    /*if (len > MAX_PROC_SIZE)
    	buf_size = MAX_PROC_SIZE;
    else
    	buf_size = len;*/
    char buf[16];

    if (copy_from_user(buf, buffer, 16))
        return -EFAULT;
    if (strlen(buf) < 1)
        return -ENOEXEC;
    if (buf[0] >= '1' && buf[0] <= '5')
    {
        proc_show_flag = buf[0] - '0';

        printk(KERN_INFO "myinfo: proc file flag: %d\n", proc_show_flag);
    }

    printk(KERN_DEBUG "procfile_write: write 16 bytes\n");
    return strlen(buf);
}


static struct file_operations proc_file_fops =
{
    .owner = THIS_MODULE,
    .open = procfile_open,
    .read = seq_read,
    .write = procfile_write,
    .llseek = seq_lseek,
    .release = single_release,

};

static int __init my_info_init(void)
{
    my_proc_file = proc_create(PROCFS_ENTRY_FILENAME, 0644, NULL, &proc_file_fops);

    if (my_proc_file == NULL)
    {
        proc_remove(my_proc_file);
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", PROCFS_ENTRY_FILENAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "/proc/%s created\n", PROCFS_ENTRY_FILENAME);
    return 0;

}

static void __exit my_info_exit(void)
{
    proc_remove(my_proc_file);
    printk(KERN_INFO "/proc/%s removed\n", PROCFS_ENTRY_FILENAME);
}


module_init(my_info_init);
module_exit(my_info_exit);

MODULE_LICENSE("GPL");
