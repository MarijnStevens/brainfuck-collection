#!/bin/bash

nasm -f bin -o bf bf.asm && chmod +x bf

./bf < ../../../bf/helloworld.b > helloworld && \
 	chmod +x helloworld && \
 	./helloworld