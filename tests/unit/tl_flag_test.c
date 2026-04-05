#include "tl_flag.h"
#include "unity.h"

void setUp(void) {
    // Setup code if needed
}

void tearDown(void) {
    tl_free_args();
}

static void test_tl_parse_args(void) {
    char *argv[] = {"program", "--test-flag"};
    tl_parse_args(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--test-flag"));
}

static void test_tl_lookup_flag(void) {
    char *argv[] = {"program", "--test-flag"};
    tl_parse_args(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--test-flag"));
    TEST_ASSERT_FALSE(tl_lookup_flag("--nonexistent-flag"));
}

static void test_tl_get_flag(void) {
    char *argv[] = {"program", "--key=value"};
    tl_parse_args(2, argv);
    TEST_ASSERT_EQUAL_STRING("value", tl_get_flag("--key"));
    TEST_ASSERT_NULL(tl_get_flag("--nonexistent-key"));
}

static void test_tl_get_flag_space(void) {
    char *argv[] = {"program", "--key", "value"};
    tl_parse_args(3, argv);
    TEST_ASSERT_EQUAL_STRING("value", tl_get_flag("--key"));
}

static void test_tl_flag_exact_match(void) {
    char *argv[] = {"program", "--foobar=1"};
    tl_parse_args(2, argv);
    TEST_ASSERT_FALSE(tl_lookup_flag("--foo"));
    TEST_ASSERT_NULL(tl_get_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("1", tl_get_flag("--foobar"));
}

static void test_tl_repeated_flag_equals(void) {
    char *argv[] = {"program", "--foo=x", "--foo=y", "--foo=z"};
    tl_parse_args(4, argv);
    TEST_ASSERT_EQUAL_UINT(3, tl_count_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("x", tl_get_flag_at("--foo", 0));
    TEST_ASSERT_EQUAL_STRING("y", tl_get_flag_at("--foo", 1));
    TEST_ASSERT_EQUAL_STRING("z", tl_get_flag_at("--foo", 2));
    TEST_ASSERT_NULL(tl_get_flag_at("--foo", 3));
    TEST_ASSERT_EQUAL_STRING("x", tl_get_flag("--foo"));
}

static void test_tl_repeated_flag_mixed(void) {
    char *argv[] = {"program", "--foo=x", "--foo", "y", "--foo=z"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_UINT(3, tl_count_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("x", tl_get_flag_at("--foo", 0));
    TEST_ASSERT_EQUAL_STRING("y", tl_get_flag_at("--foo", 1));
    TEST_ASSERT_EQUAL_STRING("z", tl_get_flag_at("--foo", 2));
}

static void test_tl_repeated_boolean_flag(void) {
    char *argv[] = {"program", "--verbose", "--verbose", "--verbose"};
    tl_parse_args(4, argv);
    TEST_ASSERT_EQUAL_UINT(3, tl_count_flag("--verbose"));
    TEST_ASSERT_NULL(tl_get_flag_at("--verbose", 0));
    TEST_ASSERT_NULL(tl_get_flag_at("--verbose", 1));
    TEST_ASSERT_NULL(tl_get_flag_at("--verbose", 2));
}

static void test_tl_adjacent_space_flags(void) {
    char *argv[] = {"program", "--foo", "x", "--bar", "y"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_STRING("x", tl_get_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("y", tl_get_flag("--bar"));
}

static void test_tl_space_flag_trailing_boolean(void) {
    char *argv[] = {"program", "--foo", "x", "--bar"};
    tl_parse_args(4, argv);
    TEST_ASSERT_EQUAL_STRING("x", tl_get_flag("--foo"));
    TEST_ASSERT_TRUE(tl_lookup_flag("--bar"));
    TEST_ASSERT_NULL(tl_get_flag("--bar"));
}

static void test_tl_positional_terminator(void) {
    char *argv[] = {"program", "command", "--foo", "--", "--baz", "--qux"};
    tl_parse_args(6, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--foo"));
    TEST_ASSERT_NULL(tl_get_flag("--foo"));
    TEST_ASSERT_FALSE(tl_lookup_flag("--baz"));
    TEST_ASSERT_FALSE(tl_lookup_flag("--qux"));
    TEST_ASSERT_EQUAL_UINT(3, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("command", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("--baz", tl_get_positional(1));
    TEST_ASSERT_EQUAL_STRING("--qux", tl_get_positional(2));
    TEST_ASSERT_NULL(tl_get_positional(3));
}

static void test_tl_positional_interleaved(void) {
    char *argv[] = {"program", "foo", "--flag", "val", "bar"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_STRING("val", tl_get_flag("--flag"));
    TEST_ASSERT_EQUAL_UINT(2, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("foo", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("bar", tl_get_positional(1));
}

static void test_tl_quoted_value_from_argv(void) {
    char *argv[] = {"program", "--foo", "bar baz qux"};
    tl_parse_args(3, argv);
    TEST_ASSERT_EQUAL_STRING("bar baz qux", tl_get_flag("--foo"));
}

static void test_tl_parse_line_quoted(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program --foo \"bar baz qux\""));
    TEST_ASSERT_EQUAL_STRING("bar baz qux", tl_get_flag("--foo"));
}

static void test_tl_parse_line_escape(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program --foo \"a\\\"b\""));
    TEST_ASSERT_EQUAL_STRING("a\"b", tl_get_flag("--foo"));
}

static void test_tl_parse_line_full(void) {
    TEST_ASSERT_TRUE(tl_parse_line("prog cmd --foo=1 --foo 2 --bar \"x y\" -- --baz"));
    TEST_ASSERT_EQUAL_UINT(2, tl_count_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("1", tl_get_flag_at("--foo", 0));
    TEST_ASSERT_EQUAL_STRING("2", tl_get_flag_at("--foo", 1));
    TEST_ASSERT_EQUAL_STRING("x y", tl_get_flag("--bar"));
    TEST_ASSERT_EQUAL_UINT(2, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("cmd", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("--baz", tl_get_positional(1));
}

static void test_tl_parse_line_unterminated(void) {
    TEST_ASSERT_FALSE(tl_parse_line("program --foo \"unterminated"));
}

static void test_tl_short_flag(void) {
    char *argv[] = {"program", "-h"};
    tl_parse_args(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-h"));
    TEST_ASSERT_NULL(tl_get_flag("-h"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_short_flag_value(void) {
    char *argv[] = {"program", "-o", "out.txt", "-v"};
    tl_parse_args(4, argv);
    TEST_ASSERT_EQUAL_STRING("out.txt", tl_get_flag("-o"));
    TEST_ASSERT_TRUE(tl_lookup_flag("-v"));
    TEST_ASSERT_NULL(tl_get_flag("-v"));
}

static void test_tl_short_flag_equals(void) {
    char *argv[] = {"program", "-o=out.txt"};
    tl_parse_args(2, argv);
    TEST_ASSERT_EQUAL_STRING("out.txt", tl_get_flag("-o"));
}

static void test_tl_bare_dash_is_positional(void) {
    char *argv[] = {"program", "-"};
    tl_parse_args(2, argv);
    TEST_ASSERT_FALSE(tl_lookup_flag("-"));
    TEST_ASSERT_EQUAL_UINT(1, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("-", tl_get_positional(0));
}

static void test_tl_short_flag_exact_match(void) {
    char *argv[] = {"program", "-help"};
    tl_parse_args(2, argv);
    TEST_ASSERT_FALSE(tl_lookup_flag("-h"));
    TEST_ASSERT_NULL(tl_get_flag("-h"));
    TEST_ASSERT_TRUE(tl_lookup_flag("-help"));
}

static void test_tl_short_repeated_boolean(void) {
    char *argv[] = {"program", "-v", "-v", "-v"};
    tl_parse_args(4, argv);
    TEST_ASSERT_EQUAL_UINT(3, tl_count_flag("-v"));
    TEST_ASSERT_NULL(tl_get_flag_at("-v", 0));
    TEST_ASSERT_NULL(tl_get_flag_at("-v", 1));
    TEST_ASSERT_NULL(tl_get_flag_at("-v", 2));
}

static void test_tl_short_repeated_mixed(void) {
    char *argv[] = {"program", "-o=a", "-o", "b", "-o=c"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_UINT(3, tl_count_flag("-o"));
    TEST_ASSERT_EQUAL_STRING("a", tl_get_flag_at("-o", 0));
    TEST_ASSERT_EQUAL_STRING("b", tl_get_flag_at("-o", 1));
    TEST_ASSERT_EQUAL_STRING("c", tl_get_flag_at("-o", 2));
}

static void test_tl_short_adjacent(void) {
    char *argv[] = {"program", "-a", "x", "-b", "y"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_STRING("x", tl_get_flag("-a"));
    TEST_ASSERT_EQUAL_STRING("y", tl_get_flag("-b"));
}

static void test_tl_short_trailing_boolean(void) {
    char *argv[] = {"program", "-o", "-v"};
    tl_parse_args(3, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-o"));
    TEST_ASSERT_NULL(tl_get_flag("-o"));
    TEST_ASSERT_TRUE(tl_lookup_flag("-v"));
    TEST_ASSERT_NULL(tl_get_flag("-v"));
}

static void test_tl_short_value_is_bare_dash(void) {
    char *argv[] = {"program", "-o", "-"};
    tl_parse_args(3, argv);
    TEST_ASSERT_EQUAL_STRING("-", tl_get_flag("-o"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_short_long_mixed(void) {
    char *argv[] = {"program", "-v", "--name=foo", "-o", "out", "--flag"};
    tl_parse_args(6, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-v"));
    TEST_ASSERT_EQUAL_STRING("foo", tl_get_flag("--name"));
    TEST_ASSERT_EQUAL_STRING("out", tl_get_flag("-o"));
    TEST_ASSERT_TRUE(tl_lookup_flag("--flag"));
    TEST_ASSERT_NULL(tl_get_flag("--flag"));
}

static void test_tl_short_followed_by_long(void) {
    char *argv[] = {"program", "-o", "--other"};
    tl_parse_args(3, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-o"));
    TEST_ASSERT_NULL(tl_get_flag("-o"));
    TEST_ASSERT_TRUE(tl_lookup_flag("--other"));
}

static void test_tl_positional_dashdash_trailing_empty(void) {
    char *argv[] = {"program", "--foo", "--"};
    tl_parse_args(3, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--foo"));
    TEST_ASSERT_NULL(tl_get_flag("--foo"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_positional_second_dashdash_is_positional(void) {
    char *argv[] = {"program", "--", "a", "--", "b"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_UINT(3, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("a", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("--", tl_get_positional(1));
    TEST_ASSERT_EQUAL_STRING("b", tl_get_positional(2));
}

static void test_tl_positional_mixed_short_long(void) {
    char *argv[] = {"program", "pos1", "-v", "--name", "foo", "pos2", "--", "-x", "--y"};
    tl_parse_args(9, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-v"));
    TEST_ASSERT_EQUAL_STRING("foo", tl_get_flag("--name"));
    TEST_ASSERT_FALSE(tl_lookup_flag("-x"));
    TEST_ASSERT_FALSE(tl_lookup_flag("--y"));
    TEST_ASSERT_EQUAL_UINT(4, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("pos1", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("pos2", tl_get_positional(1));
    TEST_ASSERT_EQUAL_STRING("-x", tl_get_positional(2));
    TEST_ASSERT_EQUAL_STRING("--y", tl_get_positional(3));
}

static void test_tl_positional_only(void) {
    char *argv[] = {"program", "a", "b", "c"};
    tl_parse_args(4, argv);
    TEST_ASSERT_EQUAL_UINT(0, tl_count_flag("--any"));
    TEST_ASSERT_EQUAL_UINT(3, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("a", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("b", tl_get_positional(1));
    TEST_ASSERT_EQUAL_STRING("c", tl_get_positional(2));
}

static void test_tl_positional_dashdash_only(void) {
    char *argv[] = {"program", "--"};
    tl_parse_args(2, argv);
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_parse_line_quoted_positional(void) {
    TEST_ASSERT_TRUE(tl_parse_line("prog \"first pos\" --flag v -- \"after dd\" plain"));
    TEST_ASSERT_EQUAL_STRING("v", tl_get_flag("--flag"));
    TEST_ASSERT_EQUAL_UINT(3, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("first pos", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("after dd", tl_get_positional(1));
    TEST_ASSERT_EQUAL_STRING("plain", tl_get_positional(2));
}

static void test_tl_long_empty_value(void) {
    char *argv[] = {"program", "--foo="};
    tl_parse_args(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("", tl_get_flag("--foo"));
}

static void test_tl_long_value_contains_equals(void) {
    char *argv[] = {"program", "--foo=a=b=c"};
    tl_parse_args(2, argv);
    TEST_ASSERT_EQUAL_STRING("a=b=c", tl_get_flag("--foo"));
}

static void test_tl_long_space_value_before_terminator(void) {
    char *argv[] = {"program", "--foo", "val", "--", "pos"};
    tl_parse_args(5, argv);
    TEST_ASSERT_EQUAL_STRING("val", tl_get_flag("--foo"));
    TEST_ASSERT_EQUAL_UINT(1, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("pos", tl_get_positional(0));
}

static void test_tl_short_empty_value(void) {
    char *argv[] = {"program", "-o="};
    tl_parse_args(2, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-o"));
    TEST_ASSERT_EQUAL_STRING("", tl_get_flag("-o"));
}

static void test_tl_short_multichar_name(void) {
    char *argv[] = {"program", "-xvf", "archive.tar"};
    tl_parse_args(3, argv);
    TEST_ASSERT_TRUE(tl_lookup_flag("-xvf"));
    TEST_ASSERT_EQUAL_STRING("archive.tar", tl_get_flag("-xvf"));
    TEST_ASSERT_FALSE(tl_lookup_flag("-x"));
    TEST_ASSERT_FALSE(tl_lookup_flag("-v"));
    TEST_ASSERT_FALSE(tl_lookup_flag("-f"));
}

static void test_tl_terminator_at_start(void) {
    char *argv[] = {"program", "--", "--foo", "bar"};
    tl_parse_args(4, argv);
    TEST_ASSERT_FALSE(tl_lookup_flag("--foo"));
    TEST_ASSERT_EQUAL_UINT(2, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("--foo", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("bar", tl_get_positional(1));
}

static void test_tl_null_flag_argument(void) {
    char *argv[] = {"program", "--foo=bar"};
    tl_parse_args(2, argv);
    TEST_ASSERT_FALSE(tl_lookup_flag(NULL));
    TEST_ASSERT_NULL(tl_get_flag(NULL));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_flag(NULL));
    TEST_ASSERT_NULL(tl_get_flag_at(NULL, 0));
}

static void test_tl_empty_argv(void) {
    char *argv[] = {"program"};
    tl_parse_args(1, argv);
    TEST_ASSERT_FALSE(tl_lookup_flag("--anything"));
    TEST_ASSERT_NULL(tl_get_flag("--anything"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_flag("--anything"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
    TEST_ASSERT_NULL(tl_get_positional(0));
}

static void test_tl_reparse_clears_previous(void) {
    char *argv1[] = {"program", "--old=1", "oldpos"};
    tl_parse_args(3, argv1);
    TEST_ASSERT_EQUAL_STRING("1", tl_get_flag("--old"));
    TEST_ASSERT_EQUAL_UINT(1, tl_count_positional());

    char *argv2[] = {"program", "--new=2"};
    tl_parse_args(2, argv2);
    TEST_ASSERT_FALSE(tl_lookup_flag("--old"));
    TEST_ASSERT_EQUAL_STRING("2", tl_get_flag("--new"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_reparse_line_after_args(void) {
    char *argv[] = {"program", "--first=1"};
    tl_parse_args(2, argv);
    TEST_ASSERT_EQUAL_STRING("1", tl_get_flag("--first"));

    TEST_ASSERT_TRUE(tl_parse_line("program --second=2"));
    TEST_ASSERT_FALSE(tl_lookup_flag("--first"));
    TEST_ASSERT_EQUAL_STRING("2", tl_get_flag("--second"));
}

static void test_tl_free_args_idempotent(void) {
    tl_free_args();
    tl_free_args();
    char *argv[] = {"program", "--foo"};
    tl_parse_args(2, argv);
    tl_free_args();
    tl_free_args();
    TEST_ASSERT_FALSE(tl_lookup_flag("--foo"));
}

static void test_tl_count_flag_absent(void) {
    char *argv[] = {"program", "--foo"};
    tl_parse_args(2, argv);
    TEST_ASSERT_EQUAL_UINT(0, tl_count_flag("--missing"));
    TEST_ASSERT_NULL(tl_get_flag_at("--missing", 0));
    TEST_ASSERT_NULL(tl_get_flag_at("--foo", 5));
}

static void test_tl_parse_line_empty(void) {
    TEST_ASSERT_TRUE(tl_parse_line(""));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_parse_line_whitespace_only(void) {
    TEST_ASSERT_TRUE(tl_parse_line("   \t  "));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
}

static void test_tl_parse_line_program_only(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program"));
    TEST_ASSERT_EQUAL_UINT(0, tl_count_positional());
    TEST_ASSERT_FALSE(tl_lookup_flag("--anything"));
}

static void test_tl_parse_line_multiple_spaces(void) {
    TEST_ASSERT_TRUE(tl_parse_line("prog   --foo=1\t\t--bar   val"));
    TEST_ASSERT_EQUAL_STRING("1", tl_get_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("val", tl_get_flag("--bar"));
}

static void test_tl_parse_line_empty_quoted_value(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program --foo \"\""));
    TEST_ASSERT_TRUE(tl_lookup_flag("--foo"));
    TEST_ASSERT_EQUAL_STRING("", tl_get_flag("--foo"));
}

static void test_tl_parse_line_escaped_space(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program --foo bar\\ baz"));
    TEST_ASSERT_EQUAL_STRING("bar baz", tl_get_flag("--foo"));
}

static void test_tl_parse_line_trailing_backslash(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program --foo bar\\"));
    TEST_ASSERT_EQUAL_STRING("bar\\", tl_get_flag("--foo"));
}

static void test_tl_parse_line_double_backslash(void) {
    TEST_ASSERT_TRUE(tl_parse_line("program --foo \"a\\\\b\""));
    TEST_ASSERT_EQUAL_STRING("a\\b", tl_get_flag("--foo"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tl_parse_args);
    RUN_TEST(test_tl_lookup_flag);
    RUN_TEST(test_tl_get_flag);
    RUN_TEST(test_tl_get_flag_space);
    RUN_TEST(test_tl_flag_exact_match);
    RUN_TEST(test_tl_repeated_flag_equals);
    RUN_TEST(test_tl_repeated_flag_mixed);
    RUN_TEST(test_tl_repeated_boolean_flag);
    RUN_TEST(test_tl_adjacent_space_flags);
    RUN_TEST(test_tl_space_flag_trailing_boolean);
    RUN_TEST(test_tl_positional_terminator);
    RUN_TEST(test_tl_positional_interleaved);
    RUN_TEST(test_tl_quoted_value_from_argv);
    RUN_TEST(test_tl_parse_line_quoted);
    RUN_TEST(test_tl_parse_line_escape);
    RUN_TEST(test_tl_parse_line_full);
    RUN_TEST(test_tl_parse_line_unterminated);
    RUN_TEST(test_tl_short_flag);
    RUN_TEST(test_tl_short_flag_value);
    RUN_TEST(test_tl_short_flag_equals);
    RUN_TEST(test_tl_bare_dash_is_positional);
    RUN_TEST(test_tl_short_flag_exact_match);
    RUN_TEST(test_tl_short_repeated_boolean);
    RUN_TEST(test_tl_short_repeated_mixed);
    RUN_TEST(test_tl_short_adjacent);
    RUN_TEST(test_tl_short_trailing_boolean);
    RUN_TEST(test_tl_short_value_is_bare_dash);
    RUN_TEST(test_tl_short_long_mixed);
    RUN_TEST(test_tl_short_followed_by_long);
    RUN_TEST(test_tl_positional_dashdash_trailing_empty);
    RUN_TEST(test_tl_positional_second_dashdash_is_positional);
    RUN_TEST(test_tl_positional_mixed_short_long);
    RUN_TEST(test_tl_positional_only);
    RUN_TEST(test_tl_positional_dashdash_only);
    RUN_TEST(test_tl_parse_line_quoted_positional);
    RUN_TEST(test_tl_long_empty_value);
    RUN_TEST(test_tl_long_value_contains_equals);
    RUN_TEST(test_tl_long_space_value_before_terminator);
    RUN_TEST(test_tl_short_empty_value);
    RUN_TEST(test_tl_short_multichar_name);
    RUN_TEST(test_tl_terminator_at_start);
    RUN_TEST(test_tl_null_flag_argument);
    RUN_TEST(test_tl_empty_argv);
    RUN_TEST(test_tl_reparse_clears_previous);
    RUN_TEST(test_tl_reparse_line_after_args);
    RUN_TEST(test_tl_free_args_idempotent);
    RUN_TEST(test_tl_count_flag_absent);
    RUN_TEST(test_tl_parse_line_empty);
    RUN_TEST(test_tl_parse_line_whitespace_only);
    RUN_TEST(test_tl_parse_line_program_only);
    RUN_TEST(test_tl_parse_line_multiple_spaces);
    RUN_TEST(test_tl_parse_line_empty_quoted_value);
    RUN_TEST(test_tl_parse_line_escaped_space);
    RUN_TEST(test_tl_parse_line_trailing_backslash);
    RUN_TEST(test_tl_parse_line_double_backslash);

    return UNITY_END();
}
