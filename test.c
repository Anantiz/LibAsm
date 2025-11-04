#include "libasm.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/* ========================================================================== */
/*                              ANSI COLOR CODES                              */
/* ========================================================================== */

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"

/* ========================================================================== */
/*                            GLOBAL TEST COUNTERS                            */
/* ========================================================================== */

static int tests_passed = 0;
static int tests_failed = 0;

/* ========================================================================== */
/*                             LOGGING FUNCTIONS                              */
/* ========================================================================== */

static void log_header(const char *test_name) {
    fprintf(stderr, COLOR_CYAN);
    fprintf(stderr, " [%d] TEST: %s\n" COLOR_RESET, getpid(), test_name);
}

static void log_result(const char *name, int is_success, const char *details_fmt, ...) {
    va_list args;

    fprintf(stderr, "  %s: ", name);

    if (is_success) {
        fprintf(stderr, COLOR_GREEN "PASS" COLOR_RESET);
    } else {
        fprintf(stderr, COLOR_RED "FAIL" COLOR_RESET);
    }

    fprintf(stderr, "\n");

    if (details_fmt) {
        fprintf(stderr, "    ");
        va_start(args, details_fmt);
        vfprintf(stderr, details_fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}

static void record_test_result(int is_success) {
    if (is_success) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

/* ========================================================================== */
/*                          ASSERTION HELPER FUNCTIONS                        */
/* ========================================================================== */

static int expect_int_eq(const char *name, int got, int want) {
    int is_equal = (got == want);
    log_result(name, is_equal, "expected=%d got=%d", want, got);
    record_test_result(is_equal);
    return is_equal;
}

static int expect_sizet_eq(const char *name, size_t got, size_t want) {
    int is_equal = (got == want);
    log_result(name, is_equal, "expected=%zu got=%zu", want, got);
    record_test_result(is_equal);
    return is_equal;
}

static int expect_str_eq(const char *name, const char *got, const char *want) {
    const char *got_str = got ? got : "(null)";
    const char *want_str = want ? want : "(null)";
    int is_equal = (strcmp(got_str, want_str) == 0);

    log_result(name, is_equal, "expected=\"%s\" got=\"%s\"", want_str, got_str);
    record_test_result(is_equal);
    return is_equal;
}

static int expect_errno(const char *name, int got_errno, int want_errno) {
    int is_equal = (got_errno == want_errno);

    log_result(name, is_equal, "expected errno=%d (%s) got errno=%d (%s)",
               want_errno, strerror(want_errno),
               got_errno, strerror(got_errno));
    record_test_result(is_equal);
    return is_equal;
}

/* ========================================================================== */
/*                               TEST: ft_strlen                              */
/* ========================================================================== */

static int test_strlen_advanced(void) {
    const char *test_name = "ft_strlen advanced";
    log_header(test_name);

    int all_tests_passed = 1;

    /* Test: Empty string */
    all_tests_passed &= expect_sizet_eq("empty string", ft_strlen(""), 0);

    /* Test: Regular string */
    all_tests_passed &= expect_sizet_eq("\"Hello, world!\"", ft_strlen("Hello, world!"), 13);

    /* Test: Long string (100k characters) */
    size_t large_size = 100000;
    char *large_string = malloc(large_size + 1);

    if (!large_string) {
        log_result("large allocation", 0, "malloc failed");
        record_test_result(0);
        all_tests_passed = 0;
    } else {
        memset(large_string, 'a', large_size);
        large_string[large_size] = '\0';
        all_tests_passed &= expect_sizet_eq("long 100k 'a's", ft_strlen(large_string), large_size);
        free(large_string);
    }

    /* Test: String with embedded NUL (should count up to first NUL) */
    char embedded_nul[] = { 'a', 'b', 'c', '\0', 'd', 'e', '\0' };
    all_tests_passed &= expect_sizet_eq("embedded NUL", ft_strlen(embedded_nul), 3);

    return all_tests_passed;
}

/* ========================================================================== */
/*                               TEST: ft_strcpy                              */
/* ========================================================================== */

static int test_strcpy_advanced(void) {
    const char *test_name = "ft_strcpy advanced";
    log_header(test_name);

    int all_tests_passed = 1;
    char destination[256];

    /* Test: Normal copy */
    memset(destination, 0xAA, sizeof(destination));
    char source1[] = "Copy this!";
    char *return_value = ft_strcpy(destination, source1);

    all_tests_passed &= expect_str_eq("copy normal content", destination, source1);
    all_tests_passed &= expect_int_eq("return pointer == dst", return_value == destination, 1);

    /* Test: Empty source string */
    memset(destination, 'X', 10);
    char source2[] = "";
    return_value = ft_strcpy(destination, source2);

    all_tests_passed &= expect_str_eq("copy empty string", destination, source2);
    all_tests_passed &= expect_int_eq("return pointer empty == dst", return_value == destination, 1);

    return all_tests_passed;
}

/* ========================================================================== */
/*                               TEST: ft_strcmp                              */
/* ========================================================================== */

static int test_strcmp_advanced(void) {
    const char *test_name = "ft_strcmp advanced";
    log_header(test_name);

    int all_tests_passed = 1;

    /* Test: Equal strings */
    all_tests_passed &= expect_int_eq("\"Hello\" == \"Hello\"", ft_strcmp("Hello", "Hello"), 0);

    /* Test: First string less than second */
    int cmp_hello_world = ft_strcmp("Hello", "World");
    all_tests_passed &= expect_int_eq("\"Hello\" < \"World\"", cmp_hello_world < 0, 1);

    /* Test: First string greater than second */
    int cmp_world_hello = ft_strcmp("World", "Hello");
    all_tests_passed &= expect_int_eq("\"World\" > \"Hello\"", cmp_world_hello > 0, 1);

    /* Test: Length difference - shorter first */
    all_tests_passed &= expect_int_eq("\"abc\" < \"abcd\"", ft_strcmp("abc", "abcd") < 0, 1);

    /* Test: Length difference - longer first */
    all_tests_passed &= expect_int_eq("\"abcd\" > \"abc\"", ft_strcmp("abcd", "abc") > 0, 1);

    /* Test: High byte comparison (unsigned char values > 127) */
    unsigned char high_byte[] = { (unsigned char)200, 0 };
    unsigned char low_byte[] = { (unsigned char)100, 0 };
    int comparison_result = ft_strcmp((char*)high_byte, (char*)low_byte);

    all_tests_passed &= expect_int_eq("high byte comparison", comparison_result > 0, 1);

    return all_tests_passed;
}

/* ========================================================================== */
/*                               TEST: ft_write                               */
/* ========================================================================== */

static int test_write_advanced(void) {
    const char *test_name = "ft_write advanced";
    log_header(test_name);

    int all_tests_passed = 1;

    /* Test: Write to pipe and verify output */
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        log_result("pipe create", 0, "pipe failed: %s", strerror(errno));
        record_test_result(0);
        return 0;
    }

    const char *message = "Library write test\n";
    ssize_t bytes_written = ft_write(pipe_fds[1], message, strlen(message));

    if (bytes_written != (ssize_t)strlen(message)) {
        log_result("ft_write pipe bytes", 0, "expected=%zu got=%zd", strlen(message), bytes_written);
        record_test_result(0);
        all_tests_passed = 0;
    } else {
        char buffer[128] = {0};
        ssize_t bytes_read = read(pipe_fds[0], buffer, sizeof(buffer));

        if (bytes_read != bytes_written) {
            log_result("readback length", 0, "expected=%zd got=%zd", bytes_written, bytes_read);
            record_test_result(0);
            all_tests_passed = 0;
        } else {
            all_tests_passed &= expect_str_eq("readback content", buffer, message);
        }
    }

    close(pipe_fds[0]);
    close(pipe_fds[1]);

    /* Test: Invalid file descriptor should set errno and return -1 */
    errno = 0;
    ssize_t invalid_result = ft_write(-1, "x", 1);
    int saved_errno = errno;

    all_tests_passed &= expect_int_eq("write invalid fd returns -1", invalid_result, -1);
    all_tests_passed &= expect_errno("write invalid fd errno", saved_errno, EBADF);

    /* Test: Zero-byte write should return 0 */
    int fd_null = open("/dev/null", O_WRONLY);
    if (fd_null >= 0) {
        ssize_t zero_write = ft_write(fd_null, message, 0);
        all_tests_passed &= expect_int_eq("zero-byte write", zero_write, 0);
        close(fd_null);
    } else {
        log_result("open /dev/null", 0, "open failed: %s", strerror(errno));
        record_test_result(0);
        all_tests_passed = 0;
    }

    return all_tests_passed;
}

/* ========================================================================== */
/*                                TEST: ft_read                               */
/* ========================================================================== */

static int test_read_advanced(void) {
    const char *test_name = "ft_read advanced";
    log_header(test_name);

    int all_tests_passed = 1;

    /* Test: Read from pipe */
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        log_result("pipe create", 0, "pipe failed: %s", strerror(errno));
        record_test_result(0);
        return 0;
    }

    const char *payload = "Data for ft_read";
    ssize_t bytes_written = write(pipe_fds[1], payload, strlen(payload));

    if (bytes_written != (ssize_t)strlen(payload)) {
        log_result("write to pipe", 0, "partial write %zd", bytes_written);
        record_test_result(0);
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return 0;
    }

    /* Close write end so read will see EOF after data */
    close(pipe_fds[1]);

    char buffer[128] = {0};
    ssize_t bytes_read = ft_read(pipe_fds[0], buffer, sizeof(buffer));

    if (bytes_read < 0) {
        log_result("ft_read error", 0, "ft_read failed: %s", strerror(errno));
        record_test_result(0);
        all_tests_passed = 0;
    } else {
        all_tests_passed &= expect_int_eq("ft_read bytes", bytes_read, (ssize_t)strlen(payload));
        all_tests_passed &= expect_str_eq("ft_read content", buffer, payload);
    }

    close(pipe_fds[0]);

    /* Test: Invalid file descriptor should return -1 and set errno */
    errno = 0;
    char temp_buffer[10];
    ssize_t invalid_result = ft_read(-1, temp_buffer, sizeof(temp_buffer));
    int saved_errno = errno;

    all_tests_passed &= expect_int_eq("read invalid fd returns -1", invalid_result, -1);
    all_tests_passed &= expect_errno("read invalid fd errno", saved_errno, EBADF);

    /* Test: Zero-byte read */
    int fd_null = open("/dev/null", O_RDONLY);
    if (fd_null >= 0) {
        ssize_t zero_read = ft_read(fd_null, temp_buffer, 0);
        all_tests_passed &= expect_int_eq("zero-byte read", zero_read, 0);
        close(fd_null);
    } else {
        log_result("open /dev/null for read", 0, "open failed: %s", strerror(errno));
        record_test_result(0);
        all_tests_passed = 0;
    }

    return all_tests_passed;
}

/* ========================================================================== */
/*                               TEST: ft_strdup                              */
/* ========================================================================== */

static int test_strdup_advanced(void) {
    const char *test_name = "ft_strdup advanced";
    log_header(test_name);

    int all_tests_passed = 1;

    /* Test: Normal string duplication */
    const char *original = "Original string for strdup";
    char *duplicate = ft_strdup(original);

    if (!duplicate) {
        log_result("ft_strdup alloc", 0, "returned NULL");
        record_test_result(0);
        return 0;
    }

    all_tests_passed &= expect_str_eq("duplicate equals original", duplicate, original);
    all_tests_passed &= expect_int_eq("duplicate pointer different", duplicate != original, 1);
    free(duplicate);

    /* Test: Empty string duplication */
    duplicate = ft_strdup("");
    if (!duplicate) {
        log_result("ft_strdup empty alloc", 0, "returned NULL");
        record_test_result(0);
        all_tests_passed = 0;
    } else {
        all_tests_passed &= expect_str_eq("duplicate empty", duplicate, "");
        free(duplicate);
    }

    /* Test: Large string duplication (200k characters) */
    size_t large_size = 200000;
    char *large_string = malloc(large_size + 1);

    if (!large_string) {
        log_result("malloc large", 0, "malloc failed");
        record_test_result(0);
        all_tests_passed = 0;
    } else {
        memset(large_string, 'z', large_size);
        large_string[large_size] = '\0';

        duplicate = ft_strdup(large_string);
        if (!duplicate) {
            log_result("ft_strdup large", 0, "returned NULL");
            record_test_result(0);
            all_tests_passed = 0;
        } else {
            all_tests_passed &= expect_sizet_eq("large dup length", ft_strlen(duplicate), large_size);
            free(duplicate);
        }

        free(large_string);
    }

    return all_tests_passed;
}

/* ========================================================================== */
/*                                MAIN FUNCTION                               */
/* ========================================================================== */

int main(void) {
    fprintf(stderr, COLOR_YELLOW "=== libasm advanced test runner ===\n" COLOR_RESET);

    int all_tests_passed = 1;

    all_tests_passed &= test_strlen_advanced();
    all_tests_passed &= test_strcpy_advanced();
    all_tests_passed &= test_strcmp_advanced();
    all_tests_passed &= test_write_advanced();
    all_tests_passed &= test_read_advanced();
    all_tests_passed &= test_strdup_advanced();

    fprintf(stderr, COLOR_YELLOW "=== summary ===\n" COLOR_RESET);
    fprintf(stderr, "  tests passed: %d\n  tests failed: %d\n",
            tests_passed, tests_failed, all_tests_passed ? "ALL TESTS PASSED!" : "SOME TESTS FAILED!");

    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}