// SPDX Licence-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2024 Laurent Fazio <laurent.fazio@gmail.com>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#ifndef min
#define min(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif /* min */

#ifndef Cast
#define Cast(type, ptr) ((type)(ptr))
#endif

ssize_t cobs_recv(int fd, void *buf, size_t len, ssize_t (*rd)(int, void *, size_t))
{
	size_t offset = 0;
    uint8_t code = 0xff;
    uint8_t block = 0;
    int err;

    while (offset < len) {
        if (block) { // Decode block byte
            err = rd(fd,
                     Cast(uint8_t *, buf) + offset,
                     min(code - 1u, len - offset));
            if (err < 0)
                return err;

            offset += err;
            block -= err;
        } else {
            err = rd(fd, &block, 1);
            if (err < 0)
                return err;

            if (block && (code != 0xff)) { // Encoded zero, write it unless it's delimiter.
                Cast(uint8_t *, buf)[offset] = 0x00;
                offset += 1;
            }
            code = block;
            if (!code) // Delimiter code found
                break;

            block--;
        }
    }

    return offset;
}
