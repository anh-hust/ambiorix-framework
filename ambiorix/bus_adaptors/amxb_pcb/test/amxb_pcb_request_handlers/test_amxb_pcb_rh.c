/****************************************************************************
**
** SPDX-License-Identifier: BSD-2-Clause-Patent
**
** SPDX-FileCopyrightText: Copyright (c) 2023 SoftAtHome
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
** this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
** this list of conditions and the following disclaimer in the documentation
** and/or other materials provided with the distribution.
**
** Subject to the terms and conditions of this license, each copyright holder
** and contributor hereby grants to those receiving rights under this license
** a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable
** (except for failure to satisfy the conditions of this license) patent license
** to make, have made, use, offer to sell, sell, import, and otherwise transfer
** this software, where such license applies only to those patent claims, already
** acquired or hereafter acquired, licensable by such copyright holder or contributor
** that are necessarily infringed by:
**
** (a) their Contribution(s) (the licensed copyrights of copyright holders and
** non-copyrightable additions of contributors, in source or binary form) alone;
** or
**
** (b) combination of their Contribution(s) with the work of authorship to which
** such Contribution(s) was added by such copyright holder or contributor, if,
** at the time the Contribution is added, such addition causes such combination
** to be necessarily infringed. The patent license shall not apply to any other
** combinations which include the Contribution.
**
** Except as expressly stated above, no rights or licenses from any copyright
** holder or contributor is granted under this license, whether expressly, by
** implication, estoppel or otherwise.
**
** DISCLAIMER
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
** USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <signal.h>

#include <amxc/amxc.h>
#include <amxp/amxp.h>

#include <amxd/amxd_dm.h>

#include <amxb/amxb.h>

#include "test_amxb_pcb_rh.h"

static amxb_bus_ctx_t* bus_ctx = NULL;

static void test_generic_setup() {
    amxc_string_t txt;
    amxc_string_init(&txt, 0);

    amxc_string_setf(&txt, "pcb_sysbus -n test_bus -I /tmp/test.sock");
    system(amxc_string_get(&txt, 0));
    sleep(1);
    amxc_string_clean(&txt);
}

static void test_generic_teardown() {
    amxc_string_t txt;
    amxc_string_init(&txt, 0);

    amxc_string_setf(&txt, "killall test_bus");
    system(amxc_string_get(&txt, 0));

    amxc_string_clean(&txt);
}

int test_amxb_pcb_setup(UNUSED void** state) {
    const char* cfg =
        "%config {\n"
        "   %global backends = ['../mod-amxb-test-pcb.so'];\n"
        "   %global uris = [ 'pcb:/tmp/test.sock' ];\n"
        "   %global listen = [ 'pcb:/tmp/test2.sock' ];\n"
        "   %global pcb = { register-name = '' };\n"
        "}\n";
    amxc_string_t txt;

    test_generic_setup();

    amxc_string_init(&txt, 0);

    unlink("test_config.odl");
    int fd = open("test_config.odl", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IROTH);
    assert_int_not_equal(fd, -1);
    amxc_string_setf(&txt, "%s", cfg);
    write(fd, amxc_string_get(&txt, 0), amxc_string_text_length(&txt));
    close(fd);

    amxc_string_reset(&txt);
    amxc_string_setf(&txt, "amxrt -A -D test_config.odl ../test_data//test_full_types.odl");
    system(amxc_string_get(&txt, 0));

    amxc_string_clean(&txt);

    assert_int_equal(amxb_be_load("../mod-amxb-test-pcb.so"), 0);
    assert_int_equal(amxb_connect(&bus_ctx, "pcb:/tmp/test.sock"), 0);

    sleep(1);

    return 0;
}

int test_amxb_pcb_teardown(UNUSED void** state) {
    amxb_disconnect(bus_ctx);
    amxb_free(&bus_ctx);

    system("killall amxrt");

    amxb_be_remove_all();
    unlink("test_config.odl");
    test_generic_teardown();

    return 0;
}

void test_pcb_cli_get(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject?");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] ?");
    printf("DONE\n");
    fflush(stdout);
    system("pcb_cli -T 2 pcb://ipc:[/tmp/test2.sock] ?");
    printf("DONE\n");
    system("pcb_cli -T 2 pcb://ipc:[/tmp/test2.sock] ?0");
    printf("DONE\n");
    fflush(stdout);
}

void test_pcb_cli_set(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.param1=hallo");
}

void test_pcb_cli_add(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject?0");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject?1");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject?2");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject?3");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.MultiInst+{number1:100,number2:100}'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.MultiInst+{key:\"HALLO\"}'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst.HALLO?");
    system("pcb_cli -i pcb://ipc:[/tmp/test.sock] TestObject.MultiInst.1?");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] persistent=yes TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] non_persistent=yes TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] -d TestObject.MultiInst?");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.PMultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] persistent=yes TestObject.PMultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] non_persistent=yes TestObject.PMultiInst+");
}

void test_pcb_cli_get_template_info(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] template_info=yes TestObject.MultiInst?");
}

void test_pcb_cli_del(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst.1-");
}

void test_pcb_cli_exec(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.get()'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.get(rel_path:\"MultiInst\")'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.get(\"MultiInst\")'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.get(\"MultiInst\")'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.function1(;y10, ;q10, ;u10, ;t10)'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.function1()'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.function2(;y10, ;n10, ;i10, ;x10)'");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject.function2()'");
}

void test_pcb_cli_subscribe(UNUSED void** state) {
    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject?&' &");

    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.param2=10");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst.2-");
    system("killall pcb_cli");

    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject?0&' &");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.param2=11");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst.3-");
    system("killall pcb_cli");

    system("pcb_cli pcb://ipc:[/tmp/test.sock] 'TestObject?2&' &");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.param2=12");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst+");
    system("pcb_cli pcb://ipc:[/tmp/test.sock] TestObject.MultiInst.4-");
    system("killall pcb_cli");
}