#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>


int main(int argc, char** argv) {

    struct event_base* base;
    enum event_method_feature f;

    base = event_base_new();
    if (!base) {
        perror("could not get event base.");
        exit(EXIT_FAILURE);
    }

    printf("using libevent with backend %s\n", event_base_get_method(base));

    f = static_cast<event_method_feature>(event_base_get_features(base));
    if (f & EV_FEATURE_ET)
        printf("  Edge-triggered event supported.\n");
    if (f & EV_FEATURE_O1)
        printf("  O(1) event notification supported.\n");
    if (f & EV_FEATURE_FDS)
        printf("  All FD types are supported.\n");

    return 0;


}