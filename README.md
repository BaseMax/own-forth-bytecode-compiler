# Own Forth Bytecode Compiler

Just a attempt to implement a own bytecode compiler.

It's a own forth **virtual machine** (VM).

**Date:** 2019-2020

## Input

Thee cmd.txt is:

```
003a 2074 7420 7322 2048 656c 6c6f 2c20
776f 726c 6421 0a22 203b 0a74 740a 7479
7065 0a62 7965 0a0a
```

$ cat cmd.txt 

```
: tt s" Hello, world!
" ;
tt
type
bye
```

## Benchmark

_test.c_:
```c
#include <stdio.h>
int main(int argc, char const *argv[]) {
  printf("Hello, world! *************!\n");
  return 0;
}
```

C program:

```
$ gcc test.c -o test
$ time ./test
real  0m0.001s
user  0m0.001s
sys  0m0.000s
```

Own VM:

```
$ time ./123
real  0m0.003s
user  0m0.003s
sys  0m0.000s
```
#### How to debug machine code?

```
readelf -sW ./123 > readelf.txt
objdump -d 123 > objdump.txt
```

#### So result?

![own forth virtual machine using pure c](image.jpg)

© Copyright 2019 Max Base, ValK
