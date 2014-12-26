/*
 * balboa-aes.c - example code using Balboa FPGA acceleration
 * library to do useful computation on an FPGA.
 *
 * Copyright (c) 2014 Andrew Isaacson <adi@hexapodia.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * Provided AS IS with no warranty.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <unistd.h>

#include <balboa.h>

void die(char *fmt, ...) __attribute__((noreturn));

void die(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

void usage(const char *cmd)
{
    die("Usage: %s [-p port]\n",
            cmd);
}

struct aes_core {
    b_u8 key[16];
    b_u8 plaintext[16];
    b_u8 ciphertext[16];
};

int hexit(int c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    die("Invalid hex '%c'\n", c);
}

void unhex(unsigned char *b, char *hex)
{
    int i;

    for (i=0; hex[i] && hex[i+1]; i+=2) {
        b[i/2] = hexit(hex[i]) << 4 | hexit(hex[i+1]);
    }
}

int main(int argc, char **argv)
{
    balboa *b;
    balboa_core *aes;
    int c;
    struct aes_core *p;
    const char *o_port = "";
    unsigned char buf[16], result[16];
    int buflen = 16;
    int i;

    while ((c = getopt(argc, argv, "p:")) != EOF) {
        switch (c) {
        case 'p':
            o_port = optarg;
            break;
        default:
            usage(argv[0]);
        }
    }

    b = balboa_open(o_port);
    if (!b) {
        die("balboa_open failed: %s\n", balboa_last_error(b));
    }

    aes = balboa_get_core(b, "aes");
    if (!aes) {
        die("balboa_get_core(aes) failed: %s\n", balboa_last_error(b));
    }

    p = balboa_core_get_win(aes, 0);
    
    unhex(buf, "10a58869d74be5a374cf867cfb473859");
    b_memcpy(p->key, buf, buflen);
    unhex(buf, "00000000000000000000000000000000");
    b_memcpy(p->plaintext, buf, buflen);

    b_memcpy(result, p->ciphertext, buflen);
    unhex(buf, "6d251e6944b051e04eaa6fb4dbf78465");

    for (i=0; i<buflen; i++) {
        if (buf[i] != result[i]) {
            printf("[%d] %02x != %02x\n", i, buf[i], result[i]);
        }
    }

    return 0;
}
