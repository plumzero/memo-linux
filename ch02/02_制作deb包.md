
在制作 deb 包时，除了可执行文件，还需要一些控制信息来描述这个安装包，如软件的版本、作者、安装包要安装的路径等，这些控制信息在一个叫作 control 的文件里。

```c
  #include <stdio.h>
  int main()
  {
    printf("hello world\n");
    return 0;
  }
```
编译上面的程序，为生成后的二进程文件 `helloworld` 制作一个软件安装包。

首先创建一个 helloworld 同名目录，然后进入该目录，分别创建 DEBIAN、usr/local/bin 目录，并在 DEBIAN 目录下创建 control 文件，将可执行文件 helloworld 复制到 usr/local/bin 目录下，最后的文件树结构如下:
```s
helloworld
├── DEBIAN
│   └── control
└── usr
    └── local
        └── bin
            └── helloworld
```
DEBIAN 目录下的 control 文件用来记录 helloworld 安装包的安装信息，可以通过编辑这个文件来配置相关安装信息。
```s
  package:helloworld
  version:1.0
  architecture:amd64
  maintainer:wit
  description: deb package demo
```

另外一个目录 usr/local/bin 表示 deb 默认安装路径。这两个文件完成后，就可以使用 dpkg 命令来制作安装包。
```s
  dpkg -b helloworld/ helloworld_1.0_amd64.deb
```
顺利的话，就会在 helloworld 的同级目录下，生成一个名为 helloworld_1.0_amd64.deb 的安装包。

接下来使用 dpkg 命令安装这个 deb 包，来验证是否正常:
```s
  sudo dpkg -i helloworld_1.0_amd64.deb
```
出现如下输出时，则说明安装成功:
```s
正在选中未选择的软件包 helloworld。
(正在读取数据库 ... 系统当前共安装有 326138 个文件和目录。)
正准备解包 helloworld_1.0_amd64.deb  ...
正在解包 helloworld (1.0) ...
正在设置 helloworld (1.0) ...
```

安装成功后，在 Shell 终端下直接输入 helloworld 命令可以直接运行。

也可以通过 dpkg 命令卸载 helloworld 程序:
```s
  sudo dpkg -P helloworld // 卸载 helloworld 程序及配置文件
  sudo dpkg -r helloworld // 卸载 helloworld 程序
```

具体的测试文件在[这里](t/helloworld)
