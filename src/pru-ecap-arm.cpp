/*
 * pru-ecap-arm.cpp
 *
 *  Created on: 12 feb 2018
 *      Author: andrea
 */


#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE         512
char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES            10000
#define DEVICE_NAME             "/dev/rpmsg_pru30"


using namespace std;

int main() {
    cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
    struct pollfd pollfds[1];
    int i;
    int result = 0;

    /* Open the rpmsg_pru character device file */
    pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

    /*
     * If the RPMsg channel doesn't exist yet the character device
     * won't either.
     * Make sure the PRU firmware is loaded and that the rpmsg_pru
     * module is inserted.
     */
    if (pollfds[0].fd < 0) {
            printf("Failed to open %s\n", DEVICE_NAME);
            return -1;
    }

    /* The RPMsg channel exists and the character device is opened */
    printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);

    /* Send 'hello world!' to the PRU through the RPMsg channel */
    result = write(pollfds[0].fd, "START", 6);
    if (result > 0)
            printf("Message START sent to PRU\n");

    result = read(pollfds[0].fd, readBuf, 8);
    if (result > 0)
            printf("Message received from PRU:%s\n\n", readBuf);

    struct EcapData
    {
        char cmd[8];
        uint32_t cap1[8];
        uint32_t cap2[8];
        uint32_t cap3[8];
        uint32_t cap4[8];
    };
    printf("Sizeof struct: %d ", sizeof(struct EcapData));

    for (i = 0; i < NUM_MESSAGES; i++) {
        result = read(pollfds[0].fd, readBuf, sizeof(struct EcapData));
        struct EcapData *data = (struct EcapData *) readBuf;
        for(int j = 0; j < 8; j++) {
            uint32_t cap1 = data->cap1[j];
            uint32_t cap2 = data->cap2[j];
            uint32_t cap3 = data->cap3[j];
            uint32_t cap4 = data->cap4[j];
//            cap1 = ((cap1 & 0xFFFF) << 15) | (cap1 >> 15);
//            cap2 = ((cap2 & 0xFFFF) << 15) | (cap2 >> 15);
//            cap3 = ((cap3 & 0xFFFF) << 15) | (cap3 >> 15);
//            cap4 = ((cap4 & 0xFFFF) << 15) | (cap4 >> 15);
            if (result > 0)
                    printf("Message received from PRU:%s, %lu, %lu, %lu, %lu\n", data->cmd, (unsigned long)cap1/200, (unsigned long)cap2/200, (unsigned long)cap3/200, (unsigned long)cap4/200);
        }
    }

    /* Received all the messages the example is complete */
    printf("Received %d messages, closing %s\n", NUM_MESSAGES, DEVICE_NAME);

    /* Close the rpmsg_pru character device file */
    close(pollfds[0].fd);

    return 0;
}


