#ifndef PXN_DRIVE_H
#define PXN_DRIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>
#include <net/if.h>

// DATA DESCRIPTION PXN V900
/*
    UP : 3  17 -1           DOWN : 3 17 1
    LEFT : 3 16 -1          RIGHT : 3 16 1

    Y : 1 308 1
    X : 1 307 1
    A : 1 304 1
    B : 1 305 1

    L1/LB : 1 310 1         Left_Hand : 1 310 1
    L2/LT : 3 2 255         Brake : 3 2 0-255
    L3/SL : 1 317 1

    R1/RB : 1 311 1         Right_Hand : 1 311 1
    R2/RT : 3 5 255         Acceleration : 3 5 0-255
    R3/SR : 1 318 1

    Options : 1 315 1
    Share   : 1 314 1
    Mode    : 1 316 1

    Steering Wheel :
    A) 270 Degree Mode
    turn left->  -135 degree : 3 0 -32768
    turn right-> +135 degree : 3 0 +32767

    B) 900 Degree Mode
    full left-> -450 degreeFF : 3 0 -32768
    full right-> +450 degree : 3 0 +32767
*/

/*
    CAN ID      DLC        Data
    0x100       8          FF FF FF FF FF FF FF FF

*/


void conversion(struct input_event *ev, struct can_frame *frame){
    /*unsigned int can_id;
    unsigned char can_data[8];*/

    /*for (int i=0; i<8; ++i) can_data[i] = 0x00;

    can_id = 0x100;

    // FF: EV_TYPE
    can_data[0] = ev->type;
    // FF FF: EV_CODE
    can_data[2] = ev->code & 0xFF;
    can_data[1] = (ev->code << 8) & 0xFF;
    // FF FF: EV_VALUE
    if (ev->type == 3 && ev->code == 0){ev->value += 32768;}
    if (ev->type == 3 && (ev->code == 16 || ev->code ==17)){ev->value +=2;}
    can_data[4] = ev->value & 0xFF;
    can_data[3] = (ev->value << 8) & 0xFF;

    printf("CAN ID: %X, Data: ", can_id);
    for (int i=0; i<8; ++i){
        printf("%02X ", can_data[i]);
    }
    printf("\n");*/

    // FF: EV_TYPE
    frame->data[0] = ev->type;
    // FF FF: EV_CODE
    frame->data[2] = ev->code & 0xFF;
    frame->data[1] = (ev->code << 8) & 0xFF;
    // FF FF: EV_VALUE
    if (ev->type == 3 && ev->code == 0){ev->value += 32768;}
    if (ev->type == 3 && (ev->code == 16 || ev->code == 17)){ev->value +=2;}
    frame->data[4] = ev->value & 0xFF;
    frame->data[3] = (ev->value << 8) & 0xFF;

    printf("CAN ID: %X, Data: ", frame->can_id);
    for (int i=0; i<8; ++i){
        printf("%02X ", frame->data[i]);
    }
    printf("\n");

}

/*void init_CAN()
{
    system("sudo ip link set can0 type can bitrate 125000");
    //system("sudo ifconfig can0 txquelen 1000");
    system("sudo ip link set up can0");


    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(s < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    //set up CAN interface
    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    //Bind socket
    if(bind(s,(struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Binding Failed!");
        close(s);
        return 1;
    }

    //Prepare CAN frame
    frame.can_id = 0x00;
    frame.can_dlc = 8; 
    frame.data[0] = 0x00; 
    frame.data[1] = 0x00;
    frame.data[2] = 0x00; 
    frame.data[3] = 0x00;
    frame.data[4] = 0x00; 
    frame.data[5] = 0x00;
    frame.data[6] = 0x00; 
    frame.data[7] = 0x00;
}

void can_send()
{
    if (write(s, &frame, sizeof(struct can_frame)) < 0) {
        perror("Write failed");
        close(s);
        return 1;
    }

    printf("CAN frame sent successfully\n");
}*/

void pxn_start(struct can_frame *frame){
    const char *device_path = "/dev/input/event6";

    // Open the joystick device
    int joystick_fd = open(device_path, O_RDONLY);

    if (joystick_fd == -1) {
        perror("Error opening joystick device");
        return -1;
    }

    printf("Joystick device opened successfully: %s\n", device_path);
    struct input_event ev;

    ssize_t bytes = read(joystick_fd, &ev, sizeof(ev));
    if (bytes == -1) {
        perror("Error reading joystick events");
    }

    if(ev.type == EV_ABS || ev.type == EV_KEY) {
    conversion(&ev, frame);  // Call the conversion function
    }
    
    // Close the joystick device
    close(joystick_fd);
    return 0;
}

#endif

// Just information
/*  # ev.type
    - EV_SYN (0x00) : Event sinkronisasi
    - EV_KEY (0x01) : Event tombol (keyboard atau joystick)
    - EV_REL (0x02) : Event perubahan relatif
    - EV_ABS (0x03) : Event nilai absolut
    - EV_MSC (0x04) : Event kontrol miscellaneous
    - EV_SW  (0x05) : Event switch
    - EV_LED (0x11) : Event Status LED
    - EV_FF  (0x15) : Event force feedback

*/


// void conversion(struct input_event *ev){
    
//     unsigned int can_id;
//     unsigned char can_data[8];

//     for (int i=0; i<8; ++i) can_data[i] = 0xFF;
    
//     switch (ev->type){
//         case EV_KEY:    // ev.type = 1 -> example: Map key press to CAN ID 0x100
//             can_id = 0x100;
//             switch (ev->code){
//                 case 304: can_data[0] = 1; break;   // Button A
//                 case 305: can_data[1] = 1; break;   // Button B
//                 case 307: can_data[2] = 1; break;   // Button X
//                 case 308: can_data[3] = 1; break;   // Button Y

//                 case 310: can_data[4] = 1; break;   // Button LB
//                 case 311: can_data[5] = 1; break;   // Button RB
//                 case 317: can_data[6] = 1; break;   // Button SL
//                 case 318: can_data[7] = 1; break;   // Button SR

//                 // case 314: can_data[7] = 1; break;   // Button Share
//                 // case 315: can_data[7] = 1; break;   // Button Option
//                 // case 316: can_data[7] = 1; break;   // Button Mode
//             }
//             break;
        
//         case EV_ABS:    // ev.type = 3 -> example : Map key press to CAN ID 0x101
//             can_id = 0x101;
//             switch (ev->code){
//                 case 0 : // Steering Wheel (Offset = +32768)
//                          can_data[0] = (ev->value + 32768) & 0xFF;         
//                          can_data[1] = ((ev->value + 32768) >> 8) & 0xFF;  
//                          break;
//                 case 2 : can_data[2] = ev->value; break;   // Brake Pedal
//                 case 5 : can_data[3] = ev->value; break;  // Acceleration Pedal
//                          // Button Arrow (Offset = +2)       
//                 case 16: can_data[4] = (ev->value + 2); break; // LEFT=1  RIGHT=3
//                 case 17: can_data[5] = (ev->value + 2); break; // UP=1  DOWN=3 
//             }
//             break;
        
//         default:
//             return;

//     }

//     printf("CAN ID: %X, Data: ", can_id);
//     for (int i=0; i<8; ++i){
//         printf("%02X ", can_data[i]);
//     }
//     printf("\n");
// }
