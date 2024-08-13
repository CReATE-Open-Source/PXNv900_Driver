# PXNv900_Driver
This repository contains source code of PXNV900 Driver and main code for sending PXNV900 data with CAN using Raspberry Pi to Differential Controller Unit (DCU).

## How to use?
1. Check PXNV900 event using "sudo evtest"
2. Change PXNV900 event path on PXNV900_driver.h. (eg. "/dev/input/event6")
3. Compile main.c with gcc

## Schematic
![PXNtoDCu (1)](https://github.com/user-attachments/assets/f30f65cf-c85e-4e92-b5dd-b9cae7ca5498)
