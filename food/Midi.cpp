#include <alsa/asoundlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "common.hpp"

using namespace std;


static snd_seq_event_t ev;
int my_output_port;
int my_input_port;
int seq_client, seq_port;
int my_client = 0, my_port;
snd_seq_t * handle;

void midi_action2(snd_seq_t *seq_handle) {

    snd_seq_event_t *ev;

    do {
        snd_seq_event_input(seq_handle, &ev);
        snd_seq_ev_ctrl_t & control = ev->data.control;
        fprintf(stderr, "Event type %d, source %d, source port %d, dest %d, dest port %d      \r", ev->type, ev->source.client, ev->source.port, ev->dest.client, ev->dest.port);
        my_client = ev->dest.client;
        my_port = ev->dest.port;
        seq_client = ev->source.client;
        seq_port = ev->source.port;
        switch (ev->type) {
        case SND_SEQ_EVENT_CONTROLLER:
            fprintf(stderr, "Control event on Channel %2d: %5d %5d %5d \r", ev->data.control.channel,
                    ev->data.control.value, control.param, (int) control.channel);
            break;
        case SND_SEQ_EVENT_PITCHBEND:
            fprintf(stderr, "Pitchbender event on Channel %2d: %5d %5d %5d \r", ev->data.control.channel,
                    ev->data.control.value, control.param, (int) control.channel);
            break;
        case SND_SEQ_EVENT_NOTEON:
            fprintf(stderr, "Note On event on Channel %2d: %5d       \r", ev->data.control.channel, ev->data.note.note);
            break;
        case SND_SEQ_EVENT_NOTEOFF:
            fprintf(stderr, "Note Off event on Channel %2d: %5d      \r", ev->data.control.channel, ev->data.note.note);
            break;
        default:

            break;
        }
        snd_seq_free_event(ev);
    } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
}


static void
send_event(int do_flush)
{
    if (my_client == 0) {
        return;
    }
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_source(&ev, my_output_port);
    snd_seq_ev_set_dest(&ev, seq_client, seq_port);

    snd_seq_event_output(handle, &ev);
//    if (do_flush)
//        snd_seq_flush_output(handle);
}


/*----------------------------------------------------------------------------*/
int midi_main(int argc, char* argv[]) {
    int err = snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0);
    if (err < 0)
        exit(1);

    snd_seq_set_client_name(handle, "Rift");
    my_input_port = snd_seq_create_simple_port(handle, "port 0",
    SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
    SND_SEQ_PORT_TYPE_APPLICATION);
    if (my_input_port < 0) {
        exit(1);
    }

    my_output_port = snd_seq_create_simple_port(handle, "port 1",
    SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
    SND_SEQ_PORT_TYPE_APPLICATION);

    snd_seq_port_subscribe_t * subscription;
    snd_seq_addr_t source;  source.client = 129; source.port = 0;
    snd_seq_addr_t dest;  source.client = 130; source.port = 0;

    err = snd_seq_port_subscribe_malloc(&subscription);
    snd_seq_port_subscribe_set_dest(subscription, &dest);
    snd_seq_port_subscribe_set_sender(subscription, &source);
    err = snd_seq_subscribe_port(handle, subscription);

    int npfd = snd_seq_poll_descriptors_count(handle, POLLIN);
    pollfd* pfd = (struct pollfd *) alloca(npfd * sizeof(struct pollfd));
    snd_seq_poll_descriptors(handle, pfd, npfd, POLLIN);
    time_t lastSend = 0;
    while (1) {
        if (poll(pfd, npfd, 100) > 0) {
            midi_action2(handle);
        }
        if (time(NULL) - lastSend > 1) {
            snd_seq_ev_set_noteon(&ev, 90, 0, 0);
            send_event(1);
            lastSend  = time(NULL);
        }
        usleep(10);

//        (..in application 128:0..)
//        snd_seq_port_subscribe_t subs;
//        memset(&subs, 0, sizeof(subs));
//        subs.sender.client = 128;
//        subs.sender.port = 0;
//        subs.dest.client = 65;
//        subs.dest.port = 1;
//        snd_seq_subscribe_port(seq, &subs);

//      snd_seq_event_t ev;
//      snd_seq_ev_clear(&ev);
//      snd_seq_ev_set_source(&ev, my_output_port);
//      snd_seq_ev_set_subs(&ev);
//      snd_seq_ev_set_direct(&ev);
//      snd_seq_event_output(seq, &ev);
//      snd_seq_drain_output(seq);
    }
}
//    snd_seq_addr_t addr; memset(&addr, 0, sizeof(addr));
//    snd_seq_parse_address(handle, &addr, "FLUID");
//    snd_seq_parse_address(handle, &addr, "QmidiNet:0");
//    int result = snd_seq_connect_from(handle, my_port, 128, 0);

