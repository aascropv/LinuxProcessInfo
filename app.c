#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int proc_file_open;
    char info_symbol;
    char buffer[3] = "";
    char read_buf[1024];
    int write_ret __attribute__((unused));
    int read_ret;

    proc_file_open = open("/proc/my_info", O_RDWR);
    if (proc_file_open == -1)
    {
        printf("Can't open /proc/my_info\n");
        return -1;
    }

    while(1)
    {
        printf("Which information do you want?\n");
        printf("Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)?\n");
        if (scanf("%s", buffer) == 1)
        {
            info_symbol = buffer[0];
            if (info_symbol == 'v')
            {
                write_ret = write(proc_file_open, "1", 2);
                printf("\nVersion: ");
            }
            else if (info_symbol == 'c')
            {
                write_ret = write(proc_file_open, "2", 2);
                printf("\nCPU information:\n");
            }
            else if (info_symbol == 'm')
            {
                write_ret = write(proc_file_open, "3", 2);
                printf("\nMemory information:\n");
            }
            else if (info_symbol == 't')
            {
                write_ret = write(proc_file_open, "4", 2);
                printf("\nTime information:\n");
            }
            else if (info_symbol == 'a')
            {
                write_ret = write(proc_file_open, "5", 2);
            }
            else if (info_symbol == 'e')
            {
                break;
            }
        }
        else
        {
            printf("Failed to read integer.\n");
        }
        lseek(proc_file_open, 0, SEEK_SET);
        while((read_ret = read(proc_file_open, read_buf, 1024)))
        {
            read_buf[read_ret] = '\0';
            printf("%s", read_buf);
        }
        printf("\n");
        printf("--------------------------------------------");
        printf("\n\n");
        buffer[0] = '\0';
    }

    close(proc_file_open);

    return 0;
}
