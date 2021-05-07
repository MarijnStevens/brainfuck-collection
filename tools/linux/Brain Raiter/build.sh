#!/bin/bash

echo -e "Build bf"
nasm -f bin -o bf bf.asm && chmod +x bf

echo -e "Build helloworld with bf"
./bf < ../../../bf/helloworld.b > helloworld && chmod +x helloworld

echo -e "Testing: " && ./helloworld