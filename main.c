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
#include <pthread.h>

#include "pxnv900_driver.h"
//#include "votol_driver.h"


//for socket CAN
int s;
struct sockaddr_can addr;
struct ifreq ifr;
struct can_frame frame_pxn;
struct can_frame frame_rcv;
struct can_frame frame_send;
int nbytes;

void init_CAN()
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

    //Prepare CAN frame send
    frame_send.can_id = 0x00;
    frame_send.can_dlc = 8; 
    frame_send.data[0] = 0x17; 
    frame_send.data[1] = 0x18;
    frame_send.data[2] = 0x19; 
    frame_send.data[3] = 0x20;
    frame_send.data[4] = 0x21; 
    frame_send.data[5] = 0x22;
    frame_send.data[6] = 0x23; 
    frame_send.data[7] = 0x24;
    
    //Prepare CAN receive
    frame_rcv.can_id = 0x00;
    frame_rcv.can_dlc = 8; 
    frame_rcv.data[0] = 0x00; 
    frame_rcv.data[1] = 0x00;
    frame_rcv.data[2] = 0x00; 
    frame_rcv.data[3] = 0x00;
    frame_rcv.data[4] = 0x00; 
    frame_rcv.data[5] = 0x00;
    frame_rcv.data[6] = 0x00; 
    frame_rcv.data[7] = 0x00;
    
    //Prepare CAN frame pxn
    frame_pxn.can_id = 0x00;
    frame_pxn.can_dlc = 8; 
    frame_pxn.data[0] = 0x01; 
    frame_pxn.data[1] = 0x02;
    frame_pxn.data[2] = 0x03; 
    frame_pxn.data[3] = 0x04;
    frame_pxn.data[4] = 0x05; 
    frame_pxn.data[5] = 0x06;
    frame_pxn.data[6] = 0x07; 
    frame_pxn.data[7] = 0x08;
}

void can_send(struct can_frame *frame)
{
    if (write(s, frame, sizeof(struct can_frame)) < 0) {
        perror("Write failed");
        close(s);
    }

    printf("CAN frame sent successfully\n");
}

int can_rcv(struct can_frame *frame)
{
    nbytes = read(s, frame, sizeof(frame));

 	if (nbytes < 0) {
		perror("Read");
		return 1;
	}

    printf("CAN frame receive successfully\n");
    printf("CAN ID: %d, Data: ", frame->can_id);
    
    printf("CAN Message Received: ID=%X, Data=%02X %02X %02X %02X %02X %02X \
           %02X %02X\n",
           frame->can_id, frame->data[0], frame->data[1], frame->data[2], 
           frame->data[3], frame->data[4], frame->data[5], frame->data[6], 
           frame->data[7]);
    
    return 0;
}

void *ThreadPXN(void *vargp)
{
	pxn_start(&frame_pxn);
	can_send(&frame_pxn);
	return NULL;
}

void *ThreadSend(void *vargp)
{
	
	return NULL;
}

void *ThreadRcv(void *vargp)
{
	int flag = 1;
	flag = can_rcv(&frame_rcv);
	if(flag == 0)
	{
		parse_rcvdata(&frame_rcv);
	}
	
	return NULL;
}

int main()
{
	init_CAN();
	
	pthread_t thread_pxn;
	pthread_t thread_send;
	pthread_t thread_rcv;
	
	//pthread_create(&thread_pxn, NULL, ThreadPXN,NULL);
	pthread_create(&thread_send, NULL, ThreadSend,NULL);
	pthread_create(&thread_rcv, NULL, ThreadRcv,NULL);
	
	while(1)
	{
	//pthread_join(thread_pxn, NULL);
	//pthread_join(thread_send, NULL);
	pthread_join(thread_rcv, NULL);
	}
	
}
