
- [大页](https://zhuanlan.zhihu.com/p/503738975)
- [Architecture Page Table Helpers](https://www.kernel.org/doc/html/latest/vm/arch_pgtable_helpers.html)
- [linux巨页内存编程](https://blog.csdn.net/github_38294679/article/details/122219094)


本示例测试是在 Intel 架构上进行的。

CPU 是否支持大页内存，可以通过 CPU flags 进行判断:
- 如果 `pse` 存在，则支持 2M 大页
- 如果 `pdpe1gb` 存在，则支持 1G 大页


### 设置大页数量

```sh
  echo 512 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
```

查看大页信息:
```sh
  grep -i huge /proc/meminfo
```
执行上面的命令，有如下输出:
```sh
  AnonHugePages:         0 kB
  HugePages_Total:     512
  HugePages_Free:      512
  HugePages_Rsvd:        0
  HugePages_Surp:        0
  Hugepagesize:       2048 kB
```
表示尺寸为 2M 的大页数量一共有 512 个，其中可用的有 512 个。

设置完大页后，系统会将大页对应的这块内存从闲余内存(MemFree)中去掉。这里分别在设置前后执行 `grep -i mem /proc/meminfo` 命令获取内存信息来对比感受:

设置前:
```sh
  MemTotal:        8157232 kB
  MemFree:         7450832 kB
  MemAvailable:    7582216 kB
  Shmem:              9088 kB
```

设置后:
```sh
  MemTotal:        8157232 kB
  MemFree:         6402404 kB
  MemAvailable:    6533840 kB
  Shmem:              9088 kB
```
对于 MemFree 项，计算 (6402404 - 7450832) / 1024 = -1023.85M，相比设置后，其闲余内存少了大约 1G，而这些内存显然被用做设置大页了(512 × 2 = 1024M)。

### 挂载大页

到此为止，其实就可以使用大页了。不过，如果想要以描述符的形式指定使用哪个大页，也可以将大页挂载到特定的文件系统上。

挂载大页:
```sh
  mkdir /mnt/huge
  mount -t hugetlbfs nodev /mnt/huge
```

如果想要永久挂载大页(即开机可用)，可以将如下语句添加到 `/etc/fstab` 文件中:
```sh
  nodev /mnt/huge hugetlbfs defaults 0 0
```
对于 1GB 大页，则是如下语句:
```sh
  nodev /mnt/huge_1GB hugetlbfs pagesize=1GB 0 0
```

### 使用示例

以下示例，可作参考:
- [大页+匿名映射](t/05_01_hp_anon.cpp)
- [大页+非匿名映射](t/05_01_hp_fd.cpp)
- [大页+共享内存+匿名映射](t/05_01_hp_shared_anon.cpp)
- [大页+共享内存+非匿名映射](t/05_01_hp_shared_fd.cpp)
- [参考代码段](t/05_01_hp_usage.cc)

`大页 + 共享内存 + /dev/zero` 的使用还有些奇怪，这是[示例代码](t/05_01_hp_zero.cpp)。
