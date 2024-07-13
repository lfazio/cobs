// SPDX Licence-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2024 Laurent Fazio <laurent.fazio@gmail.com>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cobs/cobs.h>

#ifndef Cast
#define Cast(type, ptr) ((type)(ptr))
#endif

#ifndef PANIC_ON
#define PANIC_ON(cond, fmt, ...)                    \
	do {                                            \
        if (cond) {                                 \
            printf("<P> " fmt "\n", ##__VA_ARGS__); \
            exit(-1);                               \
        }                                           \
    } while (0)
#endif /* panic_on */

#ifndef ASSERT
#define ASSERT(cond, fmt, ...) PANIC_ON(!(cond), "%s+%d: ASSERT(%s): " fmt, __FILE__, __LINE__, #cond, ##__VA_ARGS__)
#endif

struct test_state {
    size_t offset;
    size_t iter;
};

static struct test_state _state;

static uint8_t test1[] = { 0x00 };
static uint8_t test1_expected[] = { 0x01, 0x01, 0x00 };

static uint8_t test2[] = { 0x00, 0x00 };
static uint8_t test2_expected[] = { 0x01, 0x01, 0x01, 0x00 };

static uint8_t test3[] = { 0x11, 0x22, 0x33 };
static uint8_t test3_expected[] = { 0x04, 0x11, 0x22, 0x33, 0x00 };

static uint8_t test4[254];
static uint8_t test4_expected[256];

static uint8_t test5[255];
static uint8_t test5_expected[257];

static uint8_t test6[255];
static uint8_t test6_expected[258];

static ssize_t _write_mem(int fd, const void *buf, size_t count)
{
	if (!count)
		return 0;

	switch (fd) {
	case 1:
		_state.iter++;
		ASSERT(memcmp(buf, test1_expected + _state.offset, count) ==
			       0,
		       "- test1: encoding: %02x != %02x [%lu], iter=%lu, offset=%lu",
		       *Cast(uint8_t *, buf), test1_expected[_state.offset], count,
		       _state.iter, _state.offset);

		_state.offset += count;
		break;

	case 2:
		_state.iter++;
		ASSERT(memcmp(buf, test2_expected + _state.offset, count) ==
			       0,
		       "- test2: encoding: %02x != %02x [%lu], iter=%lu, offset=%lu",
		       *Cast(uint8_t *, buf), test2_expected[_state.offset], count,
		       _state.iter, _state.offset);

		_state.offset += count;
		break;

	case 3:
		_state.iter++;
		ASSERT(memcmp(buf, test3_expected + _state.offset, count) ==
               0,
		       "- test%d: encoding: %02x != %02x [%lu], iter=%lu, offset=%lu",
		       fd, *Cast(uint8_t *, buf), test3_expected[_state.offset],
		       count, _state.iter, _state.offset);
		_state.offset += count;
		break;

	case 4:
		_state.iter++;
		ASSERT(memcmp(buf, test4_expected + _state.offset, count) ==
               0,
		       "- test%d: encoding: %02x != %02x [%lu], iter=%lu, offset=%lu",
		       fd, *Cast(uint8_t *, buf), test4_expected[_state.offset],
		       count, _state.iter, _state.offset);
		_state.offset += count;
		break;

	case 5:
		_state.iter++;
		ASSERT(memcmp(buf, test5_expected + _state.offset, count) ==
               0,
		       "- test%d: encoding: %02x != %02x [%lu], iter=%lu, offset=%lu",
		       fd, *Cast(uint8_t *, buf), test5_expected[_state.offset],
		       count, _state.iter, _state.offset);
		_state.offset += count;
		break;

	case 6:
		_state.iter++;
		ASSERT(memcmp(buf, test6_expected + _state.offset, count) ==
               0,
		       "- test%d: encoding: %02x != %02x [%lu], iter=%lu, offset=%lu",
		       fd, *Cast(uint8_t *, buf), test6_expected[_state.offset],
		       count, _state.iter, _state.offset);
		_state.offset += count;
		break;

	default:
        printf("- testcase %d: send: not found\n", fd);
		return -1;
    }

	return _state.offset;
}

static ssize_t _read_mem(int fd, void *buf, size_t count)
{
	if (!buf || !count)
		return 0;

	switch (fd) {
	case 1:
		_state.iter++;

		memcpy(buf, test1_expected + _state.offset, count);
		_state.offset += count;
		break;

	case 2:
		_state.iter++;

		memcpy(buf, test2_expected + _state.offset, count);
		_state.offset += count;
		break;

	case 3:
		_state.iter++;

		memcpy(buf, test3_expected + _state.offset, count);
		_state.offset += count;
		break;

	case 4:
		_state.iter++;

		memcpy(buf, test4_expected + _state.offset, count);
		_state.offset += count;
		break;

	case 5:
		_state.iter++;

		memcpy(buf, test5_expected + _state.offset, count);
		_state.offset += count;
		break;

	case 6:
		_state.iter++;

		memcpy(buf, test6_expected + _state.offset, count);
		_state.offset += count;
		break;

	default:
        printf("- testcase %d: recv: not found\n", fd);
		return -1;
	}

	return count;
}

static void _test_reset()
{
	memset(&_state, 0, sizeof(struct test_state));
}

int main(void)
{
	ssize_t res;
    uint8_t test[259];

    printf("COBS unit tests\n");
    printf("===============\n");

    printf("# test1: 0x00 -> 0x01 0x01 0x00\n");
    _test_reset();
    res = cobs_send(1, test1, sizeof(test1), _write_mem);
    ASSERT(res >= 0
           && res == sizeof(test1_expected),
           "- test1: %lu != %lu", res, sizeof(test1_expected));
    printf("- test1: send: OK\n");
    _test_reset();
    res = cobs_recv(1, test, sizeof(test), _read_mem);
    ASSERT(res >= 0
           && res == sizeof(test1)
           && memcmp(test, test1, sizeof(test1)) == 0,
           "- test1: %lu != %lu", res, sizeof(test1));
    printf("- test1: recv: OK\n");

    printf("# test2: 0x00 0x00 -> 0x01 0x01 0x01 0x00\n");
    _test_reset();
    res = cobs_send(2, test2, sizeof(test2), _write_mem);
    ASSERT(res >= 0
           && res == sizeof(test2_expected),
           "- test2: %lu != %lu", res, sizeof(test2_expected));
    printf("- test2: send: OK\n");
    _test_reset();
    res = cobs_recv(2, test, sizeof(test), _read_mem);
    ASSERT(res >= 0
           && res == sizeof(test2)
           && memcmp(test, test2, sizeof(test2)) == 0,
           "- test2: %lu != %lu", res, sizeof(test2));
    printf("- test2: recv: OK\n");

    printf("# test3: 0x11 0x22 0x33 -> 0x04 0x11 0x22 0x33 0x00\n");
    _test_reset();
    res = cobs_send(3, test3, sizeof(test3), _write_mem);
    ASSERT(res >= 0
           && res == sizeof(test3_expected),
           "- test3: %lu != %lu", res, sizeof(test3_expected));
    printf("- test3: send: OK\n");
    _test_reset();
    res = cobs_recv(3, test, sizeof(test), _read_mem);
    ASSERT(res >= 0
           && res == sizeof(test3)
           && memcmp(test, test3, sizeof(test3)) == 0,
           "- test3: %lu != %lu", res, sizeof(test3));
    printf("- test3: recv: OK\n");

    printf("# test4: 0x01 0x02 .. 0xfe -> 0xff 0x01 0x02 .. 0xfe 0x00\n");
    _test_reset();

    for (int i = 0; i < 0xfe; i++)
		test4[i] = i + 1;

	test4_expected[0] = 0xff;
	for (int i = 1; i <= 0xfe; i++)
		test4_expected[i] = i;
	test4_expected[0xff] = 0x00;

	res = cobs_send(4, test4, sizeof(test4), _write_mem);
	ASSERT(res >= 0
	       && res == sizeof(test4_expected),
	       "- test4: %lu != %lu", res, sizeof(test4_expected));
	printf("- test4: send: OK\n");
	_test_reset();
	res = cobs_recv(4, test, sizeof(test), _read_mem);
	ASSERT(res >= 0
	       && res == sizeof(test4)
	       && memcmp(test, test4, sizeof(test4)) == 0,
	       "- test4: %lu != %lu", res, sizeof(test4));
	printf("- test4: recv: OK\n");

	printf("# test5: 0x00 0x01 0x02 .. 0xfe -> 0x01 0xff 0x01 0x02 .. 0xfe 0x00\n");
	_test_reset();

	for (int i = 0; i <= 0xfe; i++)
		test5[i] = i;

	test5_expected[0] = 0x01;
	test5_expected[1] = 0xff;
	for (int i = 1; i <= 0xfe; i++)
		test5_expected[1 + i] = i;
	test5_expected[256] = 0x00;

	res = cobs_send(5, test5, sizeof(test5), _write_mem);
	ASSERT(res >= 0
	       && res == sizeof(test5_expected),
	       "- test5: %lu != %lu", res, sizeof(test5_expected));
	printf("- test5: send: OK\n");
	_test_reset();
	res = cobs_recv(5, test, sizeof(test), _read_mem);
	ASSERT(res >= 0
	       && res == sizeof(test5)
	       && memcmp(test, test5, sizeof(test5)) == 0,
	       "- test5: %lu != %lu", res, sizeof(test5));
	printf("- test5: recv: OK\n");

	printf("# test6: 0x01 0x02 .. 0xfe 0xff -> 0xff 0x01 0x02 .. 0xfe 0x02 0xff 0x00\n");
	_test_reset();

	for (int i = 0; i <= 0xfe; i++)
		test6[i] = i + 1;

	test6_expected[0] = 0xff;
	for (int i = 1; i <= 0xfe; i++)
		test6_expected[i] = i;
	test6_expected[255] = 0x02;
	test6_expected[256] = 0xff;
	test6_expected[257] = 0x00;

	res = cobs_send(6, test6, sizeof(test6), _write_mem);
	ASSERT(res >= 0
	       && res == sizeof(test6_expected),
	       "- test6: %lu != %lu", res, sizeof(test6_expected));
	printf("- test6: send: OK\n");
	_test_reset();
	res = cobs_recv(6, test, sizeof(test), _read_mem);
	ASSERT(res >= 0
	       && res == sizeof(test6)
	       && memcmp(test, test6, sizeof(test6)) == 0,
	       "- test6: %lu != %lu", res, sizeof(test6));
	printf("- test6: recv: OK\n");

	return 0;
}
