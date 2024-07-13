// SPDX Licence-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2024 Laurent Fazio <laurent.fazio@gmail.com>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#ifndef Cast
#define Cast(type, ptr) ((type)(ptr))
#endif

ssize_t cobs_send(int fd, const void *buf, size_t len, ssize_t (*wr)(int, const void *, size_t))
{
	const uint8_t *end = Cast(const uint8_t *, buf) + len;
    uint8_t *ptr = Cast(uint8_t *, buf);
    size_t total = 0;
    int err;
    uint8_t code = 0x01;

    if (!buf || !len)
		return -EINVAL;

	while (ptr <= end) {
		if (*ptr) {
			code++;
			ptr++;
		}

		if (!*ptr || ptr == end || code == 0xff) {
			err = wr(fd, &code, 1);
			if (err < 0)
				return err;


            if (code > 0x01) {
                err = wr(fd, ptr - code + 1, code - 1);
                if (err < 0)
                    return err;
            }

            total += code;
            code = 0x01;

            if (!*ptr)
				ptr++;
		}
	}

	code = 0x00;
	err = wr(fd, &code, 1);
	if (err < 0)
		return err;

    total += 1;

	return total;
}
