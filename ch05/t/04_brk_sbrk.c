
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define _Assert(exp) { if (!(exp)) { exit(EXIT_FAILURE); } }

// 通过命令 getconf PAGE_SIZE 可以查看当前系统的内存页大小

int main()
{
  // 靠近 bss 段
  {
    // 获取当前 program break
    intptr_t cur_program_break = (intptr_t)sbrk(0);
    printf("initial program break: 0x%x\n", (unsigned int)cur_program_break);

    // program break 向上抬升 10 个
    intptr_t new_program_break = cur_program_break + 10;
    int ret = brk((void *)new_program_break);
    _Assert(ret == 0);
    cur_program_break = (intptr_t)sbrk(0);
    printf("increased program break: 0x%x\n", (unsigned int)cur_program_break);

    // 复原
    new_program_break = cur_program_break - 10;
    ret = brk((void *)new_program_break);
    _Assert(ret == 0);
    printf("recover program break: %p\n", sbrk(0));
    printf("\n");
  }

  // 远离 bss 段，近下一个内存页
  {
    intptr_t cur_program_break = (intptr_t)sbrk(0);
    printf("initial program break: 0x%x\n", (unsigned int)cur_program_break);

    intptr_t new_program_break = cur_program_break + 4096 - 1;
    int ret = brk((void *)new_program_break);
    _Assert(ret == 0);
    cur_program_break = (intptr_t)sbrk(0);
    printf("increased program break: 0x%x\n", (unsigned int)cur_program_break);
  }

  return 0;
}