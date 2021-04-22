#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define _OP       (15 << 28)
#define _ARG      0x0fffffff
#define _JMP      (0 << 28)
#define _JZ       (1 << 28)
#define _CALL     (2 << 28)
#define _RES1     (3 << 28)
#define _E_CALL   (4 << 28)
#define _RES2     (5 << 28)
#define _RES3     (6 << 28)
#define _ALU      (7 << 28)
#define _LIT      (8 << 28)
#define _LIT_MASK 0x7fffffff

int32_t * cmdlist;
int cmdlistlength=0;

int32_t r[64];
int32_t s[128];
int32_t pc;
int32_t rp = -1;
int32_t sp = -1;
int32_t mem[256];

void vm_exec(int32_t op);
void ALU_exec(int32_t arg);

int main(int argc, char* argv[]){
if ( argc != 2 )
  {
    printf( "usage: %s filename.bin", argv[0] );
  }
  else
  {
    off_t sz;
    FILE *f = fopen(argv[1],"rb");
    if ( f == 0 )
      {
        printf( "Could not open file %s\n",argv[1] );
      }
      else
      {
        fseek(f, 0L, SEEK_END);
        sz = ftell(f);
        cmdlistlength= sz / sizeof(cmdlist[0]);
        cmdlist = (int32_t*)malloc(sz);
        rewind(f);
        fread(cmdlist, 1, sz, f);
        fclose(f);

        while( pc < cmdlistlength && pc >= 0 )
        {
          int32_t op = cmdlist[pc];
printf("cmd %08Xd\n",op);
          vm_exec(op);
        }
printf("stack pointer %d\n",sp);
printf("stack         %d\n",s[sp]);
        free(cmdlist);
      }
  }
  return 0;
}

void vm_exec(int32_t op)
{
  if ( (op & _LIT) != 0 )
  {
    s[++sp] = op & _LIT_MASK;
    pc++;
    return;
  }
  int32_t arg = op & _ARG;
  switch (op & _OP)
  {
    case _JMP:    {
                    pc = arg;
                    break;
                  }
    case _JZ:     {
                    if(s[sp] == 0)
                    {
                      pc = arg;
                    }
                    sp--;
                    break;
                  }
    case _CALL:   {
                    r[++rp] = pc + 1;
                    pc = arg;
                    break;
                  }
    case _E_CALL: {
                    //todo builtin func call
                    break;
                  }
    case _ALU:    {
                    ALU_exec(arg);
                    break;
                  }
    }
}

void ALU_exec(int32_t arg)
{
  int32_t res = 0, _t = 0, _n = 0, _r = 0;
  if (sp >= 0)
  {
    _t = s[sp];
  }

  if (sp > 0)
  {
    _n = s[sp-1];
  }

  if (rp > 0)
  {
    _r = r[rp-1];
  }

  pc++;

  int32_t code = (arg & (0x7ffff << 8)) >> 8;
  switch (code)
  {
    case 0:  {
               res = _t;
               break;
             }
    case 1:  {
               res = _n;
               break;
             }
    case 2:  {
               res = _t + _n;
               break;
             }
    case 3:  {
               res = _t & _n;
               break;
             }
    case 4:  {
               res = _t | _n;
               break;
             }
    case 5:  {
               res = _t ^ _n;
               break;
             }
    case 6:  {
               res = ~_t;
               break;
             }
    case 7:  {
               if (_n == _t)
               {
                 res = 1;
               }
               break;
             }
    case 8:  {
               if (_n < _t)
               {
                 res = 1;
               }
               break;
             }
    case 9:  {
               res = _n >> _t;
               break;
             }
    case 10: {
               res = _t - 1;
               break;
             }
    case 11: {
               res = r[_r];
               break;
             }
    case 12: {
               res = mem[_t];
               break;
             }
    case 13: {
               res = _n << _t;
               break;
             }
    case 14: {
               res = sp + 1;
               break;
             }
    case 15: {
               if (_n < _t)
               {
                 res = 1;
               }
               break;
             }
  }

  int32_t ds = arg & 3;
  int32_t rs = (arg & (3 << 2)) >> 2;

  if (ds == 1)
  {
    sp++;
  }
  if (ds == 2)
  {
    sp--;
  }
  if (rs == 1)
  {
    rp++;
  }
  if (rs == 2)
  {
    rp--;
  }

  if ((arg&(1<<5)) != 0) 
  {
    //printf("N->[T]\n");
    mem[_t] = _n;
  }
  if ((arg&(1<<6)) != 0)
  {
    //printf("T->R\n");
    if (rp < 0)
    {
      //printf("Call stack underrun\n")
      exit(1);
    }
    r[rp] = _t;
  }
  if ((arg&(1<<7)) != 0)
  {
    //printf("T->N\n");
    if (sp > 0)
    {
      s[sp-1] = _t;
    }
  }
  if ((arg&(1<<27)) != 0)
  {
    //printf("R->PC\n");
    pc = r[_r];
  }
  if (sp >= 0)
  {
    s[sp] = res;
  }
}
