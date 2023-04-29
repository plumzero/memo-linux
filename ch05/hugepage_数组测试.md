
这里对大页与 vector 如何结合使用，给出一个示例，以作参考。

初始时大页内存环境如下:
```sh
AnonHugePages:         0 kB
HugePages_Total:     512
HugePages_Free:      512
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
```
一共分配了 512 个大小为 2M 的大页。

必须要做的是为 vector 设计一个分配器，该分配器使用大页作为存储地址。这里编写了个名为 [hpallocator](t/hpallocator.h) 的分配器，该分配器在 `allocator` 函数中调用 mmap 函数使用大页内存，在 `deallocate` 中释放存储。

### 简单示例

编写测试代码:
```c++
  std::vector<int64_t, hpallocator<int64_t>> vec;
  vec.reserve(2);
```
上面的示例代码中，会调用分配器的 `allocate` 函数，分配一个大页。这个时候大页内存环境如下:
```sh
  AnonHugePages:         0 kB
  HugePages_Total:     512
  HugePages_Free:      511
  HugePages_Rsvd:        0
  HugePages_Surp:        0
  Hugepagesize:       2048 kB
```
可以看到使用了 1 个大页，还剩下 511 个大页。

继续执行如下代码:
```c++
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
```
在执行完上述代码的前两句之后，大页内存环境没有变化，在执行完第三句时，大页环境变化了，如下:
```sh
  AnonHugePages:         0 kB
  HugePages_Total:     512
  HugePages_Free:      510
  HugePages_Rsvd:        0
  HugePages_Surp:        0
  Hugepagesize:       2048 kB
```
可以看到，又使用了 1 个大页，还剩下 510 个大页。原因是 push_back 的调用超出了预先分配的容量，进行了内存的重新分配。而系统在分配内存时，总是会从一个新的页开始。

当然不能根据上面的现象认为只有 510 个大页可供使用了，我们相信 vector 在内存重分配的实现过程中会对原来的旧内存进行回收。况且上述大页环境数据是在系统层面观察到的，我们相信在进程层面，第一个分配的大页已经还给了进程(但没有还给系统)，所以进程的可用大页应该是 511 个。

上面的猜测是合理的，但要验证还需要一定的测试，这里就不测试了。不过从这里也可以有一些启示:
- 在对 vector 的使用过程中，应该避免对内存的重分配
- 大页使用方面应该要节约和慎重，否则从系统层面来看，可用大页资源会减少，而系统内存碎片化程度会大大增加

> 经过后面的测试发现，并不如我们预期的那样。也就是说 vector 好像并没有及时回收那些旧内存。

这是上面的[示例代码](t/05_02_simple.cpp)，在关键函数地方也添加了一些打印，这可以让使用者看到一些更细节的东西。

