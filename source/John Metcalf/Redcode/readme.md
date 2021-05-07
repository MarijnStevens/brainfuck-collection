Brainf*** is an esoteric programming language developed by Urban Müller. There are 8 instructions in brainf***:

Ins	Macro	Description

```Brainfuck
<	lt	move pointer left
>	gt	move pointer right
+	inc	increment memory cell at pointer
-	dec	decrement memory cell at pointer
.	out	output a character from the cell at pointer
,	in	input a character and store in cell at pointer
[	op	jump past matching ] if cell at pointer is 0
]	cl	jump to matching [ if cell at pointer not 0
```

Brainf*** operates on an array of memory cells and has a pointer into the array.
Each instruction translates directly into Redcode. Before executing the
brainf*** program, the interpreter calculates and stores the offset for [ and ]
to avoid repeated searching.

Here's the code:

```Redcode
        lt equ sub #1, ptr
        gt equ add #1, ptr
        inc equ add #1, @ptr
        dec equ sub #1, @ptr
        out equ sts @ptr, 0
        in equ lds @ptr, 0
        op equ jmz 0, @ptr
        cl equ jmn 2, @ptr

        org    seek

ptr     mov.ba #0,        #prog

find    sne.a  #2,        *ptr
        sub    #1,        count
        sne.a  #0,        }ptr
        jmp    find,      >count
count   jmn    find,      #0

        mov.a  ptr,       @ptr
        sub.ba ptr,       >ptr
        mov.ba ptr,       {ptr
        sub.a  ptr,       *ptr

seek    jmn.a  seek,      >ptr
        jmn    ptr,       <ptr

prog
```