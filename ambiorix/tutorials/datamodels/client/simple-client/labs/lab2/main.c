/****************************************************************************
**
** Copyright (c) 2020 SoftAtHome
**
** Redistribution and use in source and binary forms, with or
** without modification, are permitted provided that the following
** conditions are met:
**
** 1. Redistributions of source code must retain the above copyright
** notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above
** copyright notice, this list of conditions and the following
** disclaimer in the documentation and/or other materials provided
** with the distribution.
**
** Subject to the terms and conditions of this license, each
** copyright holder and contributor hereby grants to those receiving
** rights under this license a perpetual, worldwide, non-exclusive,
** no-charge, royalty-free, irrevocable (except for failure to
** satisfy the conditions of this license) patent license to make,
** have made, use, offer to sell, sell, import, and otherwise
** transfer this software, where such license applies only to those
** patent claims, already acquired or hereafter acquired, licensable
** by such copyright holder or contributor that are necessarily
** infringed by:
**
** (a) their Contribution(s) (the licensed copyrights of copyright
** holders and non-copyrightable additions of contributors, in
** source or binary form) alone; or
**
** (b) combination of their Contribution(s) with the work of
** authorship to which such Contribution(s) was added by such
** copyright holder or contributor, if, at the time the Contribution
** is added, such addition causes such combination to be necessarily
** infringed. The patent license shall not apply to any other
** combinations which include the Contribution.
**
** Except as expressly stated above, no rights or licenses from any
** copyright holder or contributor is granted under this license,
** whether expressly, by implication, estoppel or otherwise.
**
** DISCLAIMER
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
** CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
** USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
** AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
** ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <amxc/amxc.h>
#include <amxp/amxp.h>
#include <amxd/amxd_dm.h>
#include <amxb/amxb.h>

static int app_check_args(int argc, char* argv[]) {
    int retval = 0;

    if(argc < 1) {
        printf("\n\nInvalid number of arguments\n");
        printf("Usage: %s <PATH>\n", argv[0]);
        retval = 1;
    }

    return retval;
}

static int app_initialize(amxb_bus_ctx_t** bus_ctx) {
    int retval = 0;
    const char* ba_backend = getenv("AMXB_BACKEND");
    const char* uri = getenv("AMXB_URI");

    if(ba_backend == NULL) {
        printf("No backend defined - set environment variable 'AMXB_BACKEND'\n");
        retval = 2;
        goto leave;
    }

    if(uri== NULL) {
        printf("No URI defined - set environment variable 'AMXB_URI'\n");
        retval = 2;
        goto leave;
    }

    // Load back-end
    retval = amxb_be_load(ba_backend);
    if(retval != 0) {
        printf("Failed to load back-end [%s]\n", ba_backend);
        printf("Return code = %d\n", retval);
        goto leave;
    }
    // Connect to bus system
    retval = amxb_connect(bus_ctx, uri);
    if(retval != 0) {
        printf("Failed to connect to [%s]\n", uri);
        printf("Return code = %d\n", retval);
    }

leave:
    return retval;
}

static int app_get(amxb_bus_ctx_t* bus_ctx,
                   const char* path) {
    int rv = 0;
    amxc_var_t ret;
    amxc_var_t* temp;

    amxc_var_init(&ret);

    // TODO: fetch the object(s)
    rv = amxb_get(bus_ctx, path, 1, &ret, 1);
    
    /** Just test get parameter inside instance DM */
    // First solution: Directly accesss
    printf("**** First solution query -- Access directly at get_path\r\n");
    temp = amxc_var_get_path(&ret, "0.\"Phonebook.Contact.1.\".LastName", AMXC_VAR_FLAG_DEFAULT);
    amxc_var_dump(temp, STDOUT_FILENO);
    char* str_name = amxc_var_dyncast(cstring_t, temp);
    printf("[INFO][%s_line: %d] Compare the return with ret: %p - %p, LastName :%s\r\n", __FILE__, __LINE__, temp, &ret, str_name);

    // Second solution
    printf("\n\n**** Second solution query -- get & take key function\r\n");
    temp = amxc_var_get_path(&ret, "0.\"Phonebook.Contact.1.\"", AMXC_VAR_FLAG_DEFAULT);
    amxc_var_dump(temp, STDOUT_FILENO);

    // temp = amxc_var_take_key(temp, "LastName"); // @NOTE it will delete the key-value out of temp
    temp = amxc_var_get_key(temp, "LastName", AMXC_VAR_FLAG_DEFAULT); // @NOTE Just query

    str_name = amxc_var_dyncast(cstring_t, temp);
    printf("[INFO][%s_line: %d] Compare the return with ret: %p - %p, LastName :%s\r\n", __FILE__, __LINE__, temp, &ret, str_name);

    printf("\n");

    amxc_var_dump(&ret, STDOUT_FILENO);

leave:
    amxc_var_clean(&ret);
    return rv;
}


static int app_get_greeter(amxb_bus_ctx_t* bus_ctx,
                   const char* path) {
    int rv = 0;
    amxc_var_t ret;
    amxc_var_t* temp;

    bool is = false;
    int status = (int)is;

    amxc_var_init(&ret);

    // TODO: fetch the object(s)
    rv = amxb_get(bus_ctx, path, 0, &ret, 1);
    temp = amxc_var_get_first(&ret);
    temp = amxc_var_get_path(&ret, "\"Phonebook.Contact.1.\"", AMXC_VAR_FLAG_DEFAULT);

    printf(">>>>> GET Greeter test, status: %d\n", status);
    amxc_var_dump(temp, STDOUT_FILENO);
    // amxc_var_dump(&ret, STDOUT_FILENO);

    printf("\n\n");
leave:
    amxc_var_clean(&ret);
    return rv;
}

int main(int argc, char* argv[]) {
    int retval = 0;
    amxb_bus_ctx_t* bus_ctx = NULL;

    retval = app_check_args(argc, argv);
    if (retval != 0) {
        goto leave;
    }

    retval = app_initialize(&bus_ctx);
    if (retval != 0) {
        goto leave;
    }

    retval = app_get_greeter(bus_ctx, "Greeter.Statistics.AddHistoryCount");
    retval = app_get(bus_ctx, argv[1]);

leave:
    amxb_free(&bus_ctx);
    amxb_be_remove_all();
    return retval;
}
