#!/bin/bash
make  
make mmu     

echo "---------------"
./mmu 256 BACKING_STORE.bin addresses.txt