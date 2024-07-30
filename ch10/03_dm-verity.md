
参考:
- [下载源代码](https://source.android.google.cn/source/downloading?hl=zh-cn)
- [实现dm-verity](https://source.android.google.cn/docs/security/features/verifiedboot/dm-verity?hl=zh-cn)
- [Android启动时验证](https://source.android.com/docs/security/features/verifiedboot/avb?hl=zh-cn)

### 编译与加载

首先将 `/path/to/linux-4.4.49/drivers/md/` 目录下的 dm-verity.c 和 dm-bufio.c(前者依赖后者)文件拷出，放置在某个目录下，并为该目录编写 Makefile 文件。

执行 make 编译，顺利的话会产生 dm-bufio.ko 和 dm-verity.ko 两个文件。因为后者依赖前者，所以应该先加载 dm-bufio.ko 文件。

[dm-verity示例](dm-verity)

不过在执行 `insmod` 命令时出现如下错误:
```s
  insmod: ERROR: could not insert module dm-bufio.ko: Invalid module format
```
这时因为编译出来的驱动与运行系统内核不一致导致的。

不过好在系统中已经有编译好的驱动了，在 `/lib/modules/$(uname -r)/kernel/drivers/md` 目录下。进入该目录下，执行如下命令:
```s
  sudo insmod dm-bufio.ko
  sudo insmod dm-verity.ko
```

执行 `lsmod | grep dm_`，如果出现如下类似输出，说明加载成功了:
```s
  dm_verity              20480  0
  dm_bufio               28672  1 dm_verity
```

执行 `modinfo dm_verity` 可以查看更多信息:
```s
filename:       /lib/modules/4.15.0-45-generic/kernel/drivers/md/dm-verity.ko
license:        GPL
description:    device-mapper target for transparent disk integrity checking
author:         Will Drewry <wad@chromium.org>
author:         Mandeep Baines <msb@chromium.org>
author:         Mikulas Patocka <mpatocka@redhat.com>
srcversion:     2250409D29AE0BFB1AE111A
depends:        dm-bufio
retpoline:      Y
intree:         Y
name:           dm_verity
vermagic:       4.15.0-45-generic SMP mod_unload 
parm:           prefetch_cluster:uint
```

### 应用
