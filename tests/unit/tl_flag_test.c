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

static void test_tl_parse_line_quoted_positional(void) {
    TEST_ASSERT_TRUE(tl_parse_line("prog \"first pos\" --flag v -- \"after dd\" plain"));
    TEST_ASSERT_EQUAL_STRING("v", tl_get_flag("--flag"));
    TEST_ASSERT_EQUAL_UINT(3, tl_count_positional());
    TEST_ASSERT_EQUAL_STRING("first pos", tl_get_positional(0));
    TEST_ASSERT_EQUAL_STRING("after dd", tl_get_positional(1));
    TEST_ASSERT_EQUAL_STRING("plain", tl_get_positional(2));
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
    RUN_TEST(test_tl_parse_line_quoted_positional);

    return UNITY_END();
}
