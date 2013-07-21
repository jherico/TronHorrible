/*
        OpenLase - a realtime laser graphics toolkit

Copyright (C) 2009-2011 Hector Martin "marcan" <hector@marcansoft.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 or version 3.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

//#include "libol.h"


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <iostream>
using namespace std;

int portid;

struct blip {
    int active;
    float ts;
    float age;
    float x, y;
    float r;
    float phase;
    float bright;
    int note;
};

float notespeed = 1.0;
float avgnotes = 0.0;

#define NUM_BLIPS 32

int cur_blip = 0;
struct blip blips[NUM_BLIPS];

uint16_t channels = 0;


int midi_action(snd_seq_t *seq_handle) {

    snd_seq_event_t *ev;
    int cnt = 0;

    do {
        snd_seq_event_input(seq_handle, &ev);
        cout << (int)ev->type << " " << (int)ev->source.client << " " << (int)ev->source.port << endl;
        snd_seq_free_event(ev);
    } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
    return cnt;
}


int midi_listen(int argc, char *argv[])
{

    snd_seq_t *seq_handle;
    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
        fprintf(stderr, "Error opening ALSA sequencer.\n");
        exit(1);
    }
    snd_seq_set_client_name(seq_handle, "midiview");
    if ((portid = snd_seq_create_simple_port(seq_handle, "midiview",
        SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
        fprintf(stderr, "Error creating sequencer port.\n");
        exit(1);
    }

    int npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
    pollfd pfd;
    snd_seq_poll_descriptors(seq_handle, &pfd, npfd, POLLIN);

    while(1) {
        if (poll(&pfd, npfd, 0) > 0)
            midi_action(seq_handle);



//        char* device =  "/dev/midi" ;
//        unsigned char data[3] = {0x90, 60, 127};
//
//        // step 1: open the OSS device for writing
//        int fd = open(device, O_WRONLY, 0);
//        if (fd < 0) {
//           printf("Error: cannot open %s\n", device);
//           exit(1);
//        }
//
//        // step 2: write the MIDI information to the OSS device
//        write(fd, data, sizeof(data));
//
//        // step 3: (optional) close the OSS device
//        close(fd);

        usleep(1000);
    }

    exit (0);
}
