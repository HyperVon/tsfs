// Fixedpoint multiply and divide functions
// Watcom C/C++ 10.0 callable
#ifndef _FIXED32_H_
#define _FIXED32_H_



Fixed32 fixedmul266(Fixed32 num1, Fixed32 num2);
#pragma aux fixedmul266 =   \
		"imul edx"              \
		"add eax, 0040h"        \
		"adc edx, 0"            \
		"shrd eax, edx, 6"     \
		parm caller [eax] [edx] \
		value [eax]             \
		modify [eax edx];


Fixed32 fixedmul(Fixed32 num1, Fixed32 num2);
#pragma aux fixedmul =   \
		"imul edx"              \
		"add eax, 8000h"        \
		"adc edx, 0"            \
		"shrd eax, edx, 16"     \
		parm caller [eax] [edx] \
		value [eax]             \
		modify [eax edx];

Fixed32 fixeddiv(Fixed32 numer, Fixed32 denom);  // No rounding!
#pragma aux fixeddiv =   \
                "cmp ebx,0 "\
                "je my_end "\
		"xor eax, eax"          \
		"shrd eax, edx, 16"     \
		"sar edx, 16"           \
                "idiv ebx"\
                "jmp  my_end2"\
                "my_end: mov eax,1"\
                "my_end2:"\
		parm caller [edx] [ebx] \
		value [eax]             \
		modify [eax ebx edx];

Fixed32 fixedsqrt(Fixed32 n);
#pragma aux fixedsqrt = \
                "               xor eax, eax"          \
                "               mov ebx, 40000000h"    \
                "sqrthp1:       mov edx, ecx"          \
                "               sub edx, ebx"          \
                "               jb  sqrthp2"           \
                "               sub edx, eax"          \
                "               jb  sqrthp2"           \
                "               mov ecx, edx"          \
                "               shr eax,1"             \
                "               or  eax,ebx"           \
                "               shr ebx,2"             \
                "               jnz sqrthp1"           \
                "               jz  sqrthp5"           \
                "sqrthp2:       shr eax,1"             \
                "               shr ebx, 2"            \
                "               jnz sqrthp1"           \
                "sqrthp5:       mov ebx, 00004000h"    \
                "               shl eax, 16"           \
                "               shl ecx, 16"           \
                "sqrthp3:       mov edx, ecx"          \
                "               sub edx, ebx"          \
                "               jb sqrthp4"            \
                "               sub edx, eax"          \
                "               jb sqrthp4"            \
                "               mov ecx, edx"          \
                "               shr eax,1"             \
                "               or eax, ebx"           \
                "               shr ebx,2"             \
                "               jnz sqrthp3"           \
                "               jmp sqrthp6"           \
                "sqrthp4:       shr eax,1"             \
                "               shr ebx,2"             \
                "               jnz sqrthp3"           \
                "sqrthp6:"                             \
                parm [ecx]                             \
                value [eax]                            \
                modify [eax ebx ecx edx];


Fixed32 fixedsquare(Fixed32 n);
#pragma aux fixedsquare = \
                "imul eax"        \
                "add eax, 8000h"  \
                "adc edx,0"       \
                "shrd eax,edx,16" \
                parm [eax]        \
                value [eax]       \
                modify [eax edx];


int shl10idiv(int x, int y);
#pragma aux shl10idiv = \
    " cmp   ebx,0    "\
    " je    my_end   "\
    " mov   edx, eax "\
    " shl   eax, 10  "\
    " sar   edx, 22  "\
    " idiv  ebx      "\
    " jmp   my_end2 "\
    " my_end: mov eax,1        "\
    " my_end2: "\
    parm [eax] [ebx] \
    modify exact [eax edx] \
   value [eax]


#endif
