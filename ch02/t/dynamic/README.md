
执行:
```s
  gcc sub.c -shared -fPIC -o libtest.so
  gcc main.c -ldl
  ./a.out
```