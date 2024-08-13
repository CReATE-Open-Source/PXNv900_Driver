# PXNv900_Driver
This repository contains source code of PXNV900 Driver and main code for sending PXNV900 data with CAN using Raspberry Pi to Differential Controller Unit (DCU).

## How to use?
1. Check PXNV900 event using "sudo evtest"
2. Change PXNV900 event path on PXNV900_driver.h. (eg. "/dev/input/event6")
3. Compile main.c with gcc

## Schematic
![PXNtoDCU](https://github.com/user-attachments/assets/292a96b0-4e5d-426c-942e-130539e0ef17)
