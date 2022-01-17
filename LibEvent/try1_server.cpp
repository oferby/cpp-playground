// for sockaddr_in
#include <netinet/in.h>
// for socket functions
#include <sys/socket.h>
// for fcntl
#include <fcntl.h>

#include <event2/event.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAX_LINE 16384
#define PORT 4040

void do_read(evutil_socket_t fd, short events, void* arg);
void do_write(evutil_socket_t fd, short events, void* arg);

char
rot13_char(char c) {
    /* We don't want to use isalpha here; setting the locale would change
     * which characters are considered alphabetical. */
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

struct fd_state {
    char buffer[MAX_LINE];
    size_t buffer_used;

    size_t n_written;
    size_t write_upto;

    struct event* read_event;
    struct event* write_event; 
};

struct fd_state* alloc_fd_state(struct event_base *base, evutil_socket_t fd) {

    fd_state* state = new fd_state;
    if (!state) {
        perror("could not create fd_state\n");
        exit(EXIT_FAILURE);
    }

    state->read_event = event_new(base, fd, EV_READ|EV_PERSIST, do_read, state);
    if (!state->read_event) {
        perror("could not create read event\n");
        exit(EXIT_FAILURE);
    }

    state->write_event = event_new(base, fd, EV_WRITE|EV_PERSIST, do_write, state);
    if(!state->write_event) {
        perror("could not create write event\n");
        exit(EXIT_FAILURE);
    }

    return state;

}

void free_fd_state(struct fd_state* state) {
    event_free(state->read_event);
    event_free(state->write_event);
    free(state);
}


void do_read(evutil_socket_t fd, short events, void* arg) {
    puts(">> do_read");
    struct fd_state* state = (fd_state*) arg;
    char buf[1024];
    ssize_t result;
    int i;

    while (1) {
        assert(state->write_event);
        result = recv(fd, buf, sizeof(buf), 0);
        if (result <= 0)
            break;

        for (i=0; i < result; ++i)  {
            if (state->buffer_used < sizeof(state->buffer))
                state->buffer[state->buffer_used++] = rot13_char(buf[i]);
            if (buf[i] == '\n') {
                assert(state->write_event);
                event_add(state->write_event, NULL);
                state->write_upto = state->buffer_used;
            }
        }
    }

    if (result == 0) {
        free_fd_state(state);
    } else if (result < 0) {
        if (errno == EAGAIN) {
            return;
        }
        perror("recv error\n");
        free_fd_state(state);
    }
     
}


void do_write(evutil_socket_t fd, short events, void* arg) {
    puts(">> do_write\n");
    struct fd_state* state = (fd_state*)arg;

    while (state->n_written < state->write_upto) {
        ssize_t result = send(fd, state->buffer + state->n_written,
            state->write_upto - state->n_written, 0);
        
        if (result < 0) {
            if (errno == EAGAIN) 
                return;
            free_fd_state(state);
            return;
        }
        assert (result != 0);

        state->n_written += result;
    }

    if (state->n_written == state->buffer_used)
        state->n_written = state->write_upto = state->buffer_used = 1;
    
    event_del(state->write_event);
}


void do_accept(evutil_socket_t listener, short event, void* arg) {
    puts(">> do_accept\n");
    struct event_base* base = (event_base*) arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0) {
        perror("could not accept connections\n");
        exit(EXIT_FAILURE);
    }

    if (fd > FD_SETSIZE) 
        close(fd); 
    else {
        struct fd_state* state;
        evutil_make_socket_nonblocking(fd);
        state = alloc_fd_state(base, fd);
        assert(state);
        event_add(state->read_event, NULL);
    }

}


void run(void) {
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base* base;
    struct event* listener_event;

    base = event_base_new();
    if(!base)
        return;
        
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(PORT);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(listener);

    if(bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    if(listen(listener, 16) < 0) {
        perror("error listening to socket\n");
        return;
    }

    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
    event_add(listener_event, NULL);

    event_base_dispatch(base);
}


int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);

    run();
    return 0;
}


