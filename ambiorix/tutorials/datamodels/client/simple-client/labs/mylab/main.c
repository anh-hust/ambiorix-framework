#include <stdlib.h>
#include <stdio.h>

#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxb/amxb.h>

#define UBUS_URI "ubus:/var/run/ubus/ubus.sock"
#define PCB_URI "pcb:/var/run/pcb_sys"

int main(int argc, char* argv[]) {
    const char* ba_backend = getenv("AMXB_BACKEND");
    int retval = amxb_be_load(ba_backend);
    if (retval != 0) {
        printf("Failed to load back-end [%s]\n", ba_backend);
        printf("Return code = %d\n", retval);
        exit(2);
    }
    printf("Success load back-end [%s]\n", ba_backend);


    amxb_bus_ctx_t* bus_ctx = NULL;
    retval = amxb_connect(&bus_ctx, UBUS_URI);
    if (retval != 0) {
        printf("Failed to connect to [%s]\n", UBUS_URI);
        printf("Return code = %d\n", retval);
        exit(3);
    }
    printf("Success connect [%s]\n", ba_backend);


    return 0;
}