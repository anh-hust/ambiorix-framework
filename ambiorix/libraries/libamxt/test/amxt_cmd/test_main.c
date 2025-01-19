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

#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

#include "test_cmd.h"

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_can_parse_cmd_line),
        cmocka_unit_test(test_can_parse_cmd_line_embedded_strings),
        cmocka_unit_test(test_can_get_words),
        cmocka_unit_test(test_can_get_last_word),
        cmocka_unit_test(test_can_get_words_until_separator),
        cmocka_unit_test(test_can_strip_leading_and_trailing_spaces),
        cmocka_unit_test(test_count_separators),
        cmocka_unit_test(test_remove_until),
        cmocka_unit_test(test_remove_last_part),
        cmocka_unit_test(test_can_complete_path),
        cmocka_unit_test(test_can_add_parts),
        cmocka_unit_test(test_can_parse_short_options),
        cmocka_unit_test(test_short_options_with_value_fails_when_combined),
        cmocka_unit_test(test_can_parse_long_options),
        cmocka_unit_test(test_can_pass_valid_options),
        cmocka_unit_test(test_last_options_is_taken),
        cmocka_unit_test(test_get_option_fails_with_invalid_input),
        cmocka_unit_test(test_parse_option_print_errors),
        cmocka_unit_test(test_print_help),
        cmocka_unit_test(test_get_index),
        cmocka_unit_test(test_can_complete_help),
        cmocka_unit_test(test_can_complete_options),
        cmocka_unit_test(test_can_complete_option_value),
        cmocka_unit_test(test_complete_invalid_option),
        cmocka_unit_test(test_print_excess_args),
        cmocka_unit_test(test_can_parse_simple_table),
        cmocka_unit_test(test_can_parse_simple_array),
        cmocka_unit_test(test_can_parse_values_with_embedded_string),
        cmocka_unit_test(test_can_parse_composite_table),
        cmocka_unit_test(test_can_parse_composite_array),
        cmocka_unit_test(test_parsing_fails_when_array_not_allowed),
        cmocka_unit_test(test_parsing_fails_when_table_not_allowed),
        cmocka_unit_test(test_parsing_fails_when_missing_key),
        cmocka_unit_test(test_parsing_fails_when_no_equal),
        cmocka_unit_test(test_parsing_when_missing_value),
        cmocka_unit_test(test_parsing_composite_fails_when_not_allowed),
        cmocka_unit_test(test_parsing_fails_when_key_in_array),
        cmocka_unit_test(test_parsing_fails_when_double_key),
        cmocka_unit_test(test_parsing_fails_when_not_array_or_table),
        cmocka_unit_test(test_can_parse_primitive),
        cmocka_unit_test(test_can_get_values),
        cmocka_unit_test(test_quoted_string_is_a_string),
    };
    return cmocka_run_group_tests_name("amxt-cmd", tests, NULL, NULL);
}
