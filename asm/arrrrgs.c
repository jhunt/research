/*
   arrrrgs.c - Seeing what functions with long arg lists look like

   This is not meant to be compiled into an executable, just object
   code.  Try `make arrrrgs.o`.

   To see the assembler code, run `objdump -M intel -d arrrrgs.o`

     0000000000000000 <fnargs>:
        0:  55                     push   rbp
        1:  48 89 e5               mov    rbp,rsp
        4:  89 7d fc               mov    DWORD PTR [rbp-0x4],edi
        7:  89 75 f8               mov    DWORD PTR [rbp-0x8],esi
        a:  89 55 f4               mov    DWORD PTR [rbp-0xc],edx
        d:  89 4d f0               mov    DWORD PTR [rbp-0x10],ecx
       10:  44 89 45 ec            mov    DWORD PTR [rbp-0x14],r8d
       14:  44 89 4d e8            mov    DWORD PTR [rbp-0x18],r9d
       18:  83 45 fc 01            add    DWORD PTR [rbp-0x4],0x1
       1c:  83 45 f8 01            add    DWORD PTR [rbp-0x8],0x1
       20:  83 45 f4 01            add    DWORD PTR [rbp-0xc],0x1
       24:  83 45 f0 01            add    DWORD PTR [rbp-0x10],0x1
       28:  83 45 ec 01            add    DWORD PTR [rbp-0x14],0x1
       2c:  83 45 e8 01            add    DWORD PTR [rbp-0x18],0x1
       30:  83 45 10 01            add    DWORD PTR [rbp+0x10],0x1
       34:  83 45 18 01            add    DWORD PTR [rbp+0x18],0x1
       38:  83 45 20 01            add    DWORD PTR [rbp+0x20],0x1
       3c:  83 45 28 01            add    DWORD PTR [rbp+0x28],0x1
       40:  83 45 30 01            add    DWORD PTR [rbp+0x30],0x1
       44:  83 45 38 01            add    DWORD PTR [rbp+0x38],0x1
       48:  b8 00 00 00 00         mov    eax,0x0
       4d:  5d                     pop    rbp
       4e:  c3                     ret

 */

int fnargs(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l)
{
	a++; b++; c++; d++; e++; f++; g++; h++; i++; j++; k++; l++;
	return 0;
}
