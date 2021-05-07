#!/bin/bash


sed  '/	chkabort();/ c\\' bfi.c > bfi_patched.c

gcc bfi_patched.c -o bfi