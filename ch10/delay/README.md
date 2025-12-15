
测试内容。

执行下面的命令，完成驱动的编译、加载等:
```s
# 编译模块
make

# 加载模块
sudo insmod my_delay_module.ko

# 查看内核消息（查看 delay 输出）
dmesg | tail -20
```

执行下面的命令，完成驱动的卸载:
```s
# 卸载模块
sudo rmmod my_delay_module

# 再次查看消息
dmesg | tail -20
```