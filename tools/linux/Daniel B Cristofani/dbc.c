/*the dbc brainfuck compiler*/
/*version 1.1*/
/*Produces machine-language executables for Sun machines. Quite
straightforward; does optimization of sequences of identical instructions, but
does not optimize away any loops. Uses the operating system's memory
protection, i.e. if you try to access anything outside the array you will get a
segmentation fault immediately; this for efficiency.

usage: dbc takes three arguments: the size of the array (in bytes), the name of
the source file, and the name of the output file. Exactly those three, in that
order. This could be simplified by layering a shell script on top of it, e.g.:

#!/bin/sh
rm -f out
realdbc 0x00010000 $1 out && chmod 500 out && exec out

Notice that for the memory protection to work right, the array size had better
be a multiple of the page size, i.e. 65536 bytes.

Some limitations besides the primitive user interface:
-a sequence of more than 4095 +s, -s, <s, or >s in a row will not be handled
correctly.
-similar, but even less likely: in a program more than two million machine
instructions long, the jumps will not work properly.
-the resulting programs accept exactly as much input as is specified by the ,
command; any input remaining after the programs terminate will be treated as a
command by the shell. In short the programs do not eat all the leftover input
when terminating. I may fix this.
-i've used sparc v8 instructions which are "deprecated" in v9, notably the non-
predicting conditional branches. This for portability.
-the traps to the system (for read, write, and exit calls) are here done with
trap number 8. Old systems may perhaps need trap 0 instead; which would involve
replacing the final '8' with a '0' in the number on each line marked "ta 8".
-the final fwrite call assumes compilation on a bigendian machine.

If you find any bugs, or have any questions about the compiler, or any feature
requests, or anything, talk to me. Likewise if you derive benefit from my work
I'd be glad to hear it.

Daniel B Cristofani (cristofdathevanetdotcom)
http://www.hevanet.com/cristofd/brainfuck/
*/

/*Make any use you like of this software. I can't stop you anyway. :)*/

#include <stdio.h>
#include <stdlib.h>

long cnums[256]; /*how many of each character?*/

int main(int argc, char **argv){
    FILE *input, *output;
    char nonbf[256] =
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    long *scode; /*sparc code to output*/
    long **stack; /*outstanding [s to patch.*/
    long **sptr; 
    long **plist; /*just-matched [s to patch the delay slot for soon*/
    long **plptr;
    long *psource; /*patch delay slot from here after here is filled*/
    long *a; /*current spot in machine code*/
    long b, c, frog, disp; /*input; # of same; second input; jump distance.*/
    long psize, msize, sdepth = 0; /*program size; array size; stack depth.*/

    if (argc < 4){
        fprintf(stderr, "too few arguments.\n");
        exit(1);
    }
    msize = strtol(argv[1], 0, 0);
    if (!(input = fopen(argv[2], "r"))){
        fprintf(stderr, "can\'t open %s for input.\n",argv[2]);
        exit(1);
    }
    if (!(output = fopen(argv[3], "w"))){
        fprintf(stderr, "can\'t open %s for output.\n",argv[3]);
        exit(1);
    }
    while ((b = getc(input)) != EOF){
        ++cnums[b];
        if (cnums[91] - cnums[93] > sdepth){
            sdepth = cnums[91] - cnums[93];
        } else if (cnums[91] < cnums[93]) {
            fprintf(stderr, "unmatched ].\n");
            exit(1);
        }
    }
    if (cnums[91] > cnums[93]) {
        fprintf(stderr, "unmatched [.\n");
        exit(1);
    }
    rewind(input);
    psize = 3*cnums[43] + 3*cnums[45] + 6*cnums[44] + 3*cnums[46];
    psize += cnums[60] + cnums[62] + 4*cnums[91] + 4*cnums[93];
    psize += 35; /*These are conservative estimates.*/
    if (!(scode = (long*)malloc((psize+32) * sizeof(long)))){
        fprintf(stderr, "not enough memory to store program.\n",argv[3]);
        exit(1);
    }
    if (!(stack=sptr=(long**)malloc((sdepth+32)*sizeof(long*)))){
        fprintf(stderr, "not enough memory for backpatch stack.\n",argv[3]);
        exit(1);
    }
    if (!(plist = plptr=(long**)malloc((sdepth+32)*sizeof(long*)))){
        fprintf(stderr, "not enough memory for patch list.\n",argv[3]);
        exit(1);
    }

    a = scode + 29;
    *a++ = 0x92100000; /*or %g0, &g0, %o1 (i.e. %o1 (pointer) = 0)*/
	*a++ = 0x94102001; /*or %g0, 1, %o2*/
    b = getc(input);
    while (b != EOF){
        c = 1; /*c counts how many identical instructions in a row there were*/
        while ((frog=getc(input))==b && ++c || frog!=EOF && nonbf[frog])
            ;

        switch (b){
        
        case '[':
            *a++ = 0xd60a4000; /*ld [%o1], %o3*/
            *a++ = 0x80a2e000; /*subcc %o3, 0, %g0*/
            while (c--) *sptr++ = a; 
            a += 2; /*backpatch later.*/
            break;
            
        case ']':
            *a++ = 0xd60a4000; /*ld [%o1], %o3*/
            *a++ = 0x80a2e000; /*subcc %o3, 0, %g0*/
            disp = (a - *--sptr);
            **sptr  = 0x22800003 + disp; /*be,a disp+3*/
            *plptr++ = *sptr+1;
            *a = 0x32c00003 - disp; /*bne,a -disp+3*/
            a[1] = (*sptr)[2];
            while (--c){
                disp = (a - *--sptr);
                **sptr = 0x22800003 + disp; /*be,a disp+3*/
                *plptr++ = *sptr+1;
            }
            a += 2;
            psource = a;
            break;
            
        case '<':
            *a++ = 0x92a26000 + c; /*subcc %o1, c, %o1*/
            break;
            
        case '>':
            *a++ = 0x92826000 + c; /*addcc %o1, c, %o1*/
            break;
            
        case ',':
            *a++ = 0x82102003; /*or %g0, 3, %g1*/
            while (c--){
            	*a++ = 0x90102000; /*or %g0, 0, %o0*/
            	*a++ = 0x91d02008; /*ta 8*/
            }
            break;
            
        case '.':
            *a++ = 0x82102004; /*or %g0, 4, %g1*/
            while (c--){
            	*a++ = 0x90102001; /*or %g0, 1, %o0*/
            	*a++ = 0x91d02008; /*ta 8*/
            }
            break;
            
        case '+':
            *a++ = 0xd60a4000; /*ld [%o1], %o3*/
            *a++ = 0x9682e000 + c; /*addcc %o3, c, %o3*/
            *a++ = 0xd62a4000; /*st %o3, [%o1]*/
            break;
            
        case '-':
            *a++ = 0xd60a4000; /*ld [%o1], %o3*/
            *a++ = 0x96a2e000 + c; /*subcc %o3, c, %o3*/
            *a++ = 0xd62a4000; /*st %o3, [%o1]*/
            break;
            
        default:
            break;
            
        }
        b=frog;
        while (plptr > plist && a > psource)
            **--plptr = *psource;
    }
    fclose(input);
    
    *a++ = 0x90102000; /*or %g0, 0, %o0*/
    *a++ = 0x82102001; /*or %g0, 1, %g1*/
    *a++ = 0x91d02008; /*ta 8*/
    while (plptr > plist && a > psource){
        **--plptr = *psource;
    }
    psize = (a - scode)*4;
    
    /*Now, set up headers for a simple ELF file.*/
    /*ELF header*/
    scode[0] = 0x7f454c46; /*ELF magic number*/
    scode[1] = 0x01020120; /*32-bit, big-endian, ELF v.1, reserved*/
    scode[2] = 0x64626320; /*reserved*/
    scode[3] = 0x312e3120; /*reserved*/
    scode[4] = 0x00020002; /*executable, SPARC*/
    scode[5] = 0x00000001; /*ELF v.1*/
    scode[6] = msize+0x0074; /*entry point in memory*/
    scode[7] = 0x00000034; /*offset to program header table*/
    scode[8] = 0x00000000; /*offset to nonexistent section header table*/
    scode[9] = 0x00000000; /*nonexistent flags*/
    scode[10] = 0x00340020; /*ELF header size, p header table entry size*/
    scode[11] = 0x00020000; /*2 p headers in p h table, null s h t entry size*/
    scode[12] = 0x00000000; /*no section headers, no section name stringtable*/
    
    /*program header for (zeroed) data segment*/
    scode[13] = 0x00000001; /*loadable segment*/
    scode[14] = 0x00000000; /*"offset" in file/
    scode[15] = 0x00000000; /*offset in memory*/
    scode[16] = 0x00000000; /*physical address not applicable*/
    scode[17] = 0x00000000; /*Takes no file space*/
    scode[18] = msize;      /*takes msize bytes in memory*/
    scode[19] = 0x00000006; /*flags: read and write*/
    scode[20] = 0x00010000; /*align on 64K boundary*/
    
    /*program header for code segment*/
    scode[21] = 0x00000001; /*loadable segment*/
    scode[22] = 0x00000000; /*offset in file*/
    scode[23] = msize;      /*offset in memory: after the data*/
    scode[24] = 0x00000000; /*physical address not applicable*/
    scode[25] = psize; /*size of header+code in file*/
    scode[26] = psize; /*size of header+code in memory*/
    scode[27] = 0x00000001; /*flags: execute only*/
    scode[28] = 0x00010000; /*align on 64K boundary*/

    fwrite(scode, 1, psize, output);
    fclose(output);
    exit(0);
}
