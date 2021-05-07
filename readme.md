
# Brainfuck

## Introduction

This repository is contains brainfuck program collected from the sources:

- http://brainfuck.org/brainfuck.html
- https://esolangs.org/wiki/Brainfuck
- https://en.wikipedia.org/wiki/Brainfuck
- https://www.iwriteiam.nl/Ha_BF.html

## Language instruction set.

```
> increment the data pointer
  to point to the next cell to the right

< decrement the data pointer
  to point to the next cell to the left

+ increment (increase by one) the
  byte at the data point

- decrement (decrease by one) the
  byte at the data point

. output the byte at the data pointer

, accept one byte of input, storing
  it's value in the byte at the data pointer

[ if the byte at the data pointer is
  zero, then instead of moving the
  instuction pointer forward to the
  next command, jump it forward to
  the command after the matching ]
  command

] if the byte at the data pointer is nonzero,
  then instead of moving the instruction pointer
  forward to the next command, jump it
  back to the command after the matching
  ] command.

  Any other character are ignored; and should be seen as documentation
  There are some commen brainfuck extensions with a couple of more
  instructions; and any brainfuck program should be backwards-compatible
```

Because brainfuck does not have a formal language specification;
see Portability there are multiple dialects, some interpeters have extentions

```
# Some implementations, use it to dumb the internal state of the programming,
  usefull for debugging purposes. This command originated as a feature of Urban
  Müller original interpreter in C; there it outputs the values of the first
  10 cells in decimals, and indicates the pointer's location

! Separate brainfuck programs from the input for them.
  This is done for brainfuck interpreters in brainfuck, because these
  interpreters must receive both programs and input via brainfuck's single
  input channel and must distingquish them somehow.

  Frans Faase's brainfuck interpreter in brainfuck,
  Daniel B Cristofani also uses it in his implementation,
```

## Machine requirements
 - 30000 bytes of memory
 - 2 streams. I/O


## Basic C equivalent code

```
char array[30000] = {0};
char *ptr = &array[0];

++ptr;              // >
--ptr;              // <
++*ptr;             // +
--*ptr;             // -
putchar(*ptr);      // .
scanf(" %c, ptr);   // ,
while (*ptr) {      // [
}                   // ]
```

## Code

A lot of code in brainfuck-programs are written by other people, and
some provide there own copyright statement, but some don't.

If the writter was known, but not in the file itself; I modified the files to
reflect that; in the format: `(C) Copyright <name>`
