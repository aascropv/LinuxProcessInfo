# OS2021_HW1

## proc/my_info : 

* kernel module

* 產生一個能抓取Linux version、CPU information、Memory information、  Time information的檔案

* 下圖為proc/my_info裡所呈現的樣子
![](https://i.imgur.com/o2rRZgU.png)

## app.c

接著我們需要利用 app.c 產生的app執行檔從User space控制my_info所顯示的資料。

* 利用write()、read()讀寫my_info
* 在 my_info.c (kernel module)中，有一個flag控制所要顯示的項目:
    '1'為Linux version
    '2'為CPU information
    '3'為Memory information
    '4'為Time information
    '5'為全部顯示，並且為預設值
* app.c 中的write()和read()

> write()

```cpp=
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
```

> read()

```cpp=
lseek(proc_file_open, 0, SEEK_SET);
while((read_ret = read(proc_file_open, read_buf, 1024)))
{
    read_buf[read_ret] = '\0';
    printf("%s", read_buf);
}
```
* app
![](https://i.imgur.com/NFultXD.png)
