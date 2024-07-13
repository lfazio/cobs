// SPDX Licence-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2024 Laurent Fazio <laurent.fazio@gmail.com>

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

/**
 * @brief Send a buffer @a buf of @a len size encoded with COBS
 *
 * This function does not allocate any buffer, not copy any data.
 *
 * @param[in] fd A descriptor given as first argument of @a wr
 * @param[in] buf The buffer of data to send encoded
 * @param[in] len The size of the @a buf buffer
 * @param[in,nullable] wr The write function used to transfer the
 *                        encoded buffer. If null, @a wr is
 *                        assignated the standard write() function.
 *
 * @return the number of byte sent, a negative value otherwise
 */
ssize_t cobs_send(int fd, const void *buf, size_t len,
                  ssize_t (*wr)(int fd, const void *buf, size_t count));

/**
 * @brief Read a COBS encoded data into a buffer @a buf of @a len size
 *
 * This function does not allocate any buffer, not copy any data.
 *
 * @param[in] fd A descriptor given as first argument of @a rd
 * @param[in] buf The buffer of decode data
 * @param[in] len The size of the @a buf buffer
 * @param[in,nullable] rd The read function used to recv the
 *                        encoded buffer. If null, @a wr is
 *                        assignated the standard read() function.
 *
 * @return the number of byte received, a negative value otherwise
 */
ssize_t cobs_recv(int fd, void *buf, size_t len,
                  ssize_t (*rd)(int fd, void *buf, size_t count));
