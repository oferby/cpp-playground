#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>


int main(int argc, char** argv) {

    const char** methods = event_get_supported_methods();
    printf("Libevent version: %s.\nAvailable Methods:\n", event_get_version());
    for (int i=0; methods[i] != NULL; ++i) {
        printf("  %s\n",methods[i]);
    }

    return 0;
}