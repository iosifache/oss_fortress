#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"
#include "generate_recovery_token.h"

#define HEXDUMP_COLS 16

char *bytes_to_hex_str(char *bytes, int length) {
    const unsigned char bytearr[] = { 0x12, 0x34, 0x56, 0x78 };
    const size_t hex_str_len = 2 * length;
    char *hex_str, *p;
    int i;

    hex_str = malloc(hex_str_len + 1);
    if (!hex_str) return NULL;

    p = hex_str;
    for (size_t i = 0; i < length; i++) {
        p += sprintf(p, "%.2x", bytes[i]);
    }

    return hex_str;
}

char *generate_recovery_token(BYTE *data, int length) {
  BYTE *buf, *hashed, *buf_hex;
  SHA256_CTX ctx;
  char *server_recovery_passphrase;
  int hashed_len, passphrase_len;

  server_recovery_passphrase = getenv("PORTRAIT_RECOVERY_PASSPHRASE");
  if (server_recovery_passphrase == NULL) return NULL;

  passphrase_len = strlen(server_recovery_passphrase) - 1;

  buf = (BYTE *)malloc(SHA256_BLOCK_SIZE * sizeof(BYTE));
  if (!buf)
    return NULL;

  hashed_len = length + passphrase_len;
  hashed = (BYTE *)malloc(hashed_len * sizeof(BYTE));
  if (!hashed)
    return NULL;

  memcpy(hashed, server_recovery_passphrase, passphrase_len);
  memcpy(hashed + passphrase_len, data, length);

  sha256_init(&ctx);
  sha256_update(&ctx, hashed, hashed_len);
  sha256_final(&ctx, buf);

  buf_hex = bytes_to_hex_str(buf, SHA256_BLOCK_SIZE);
  free(buf);

  return buf_hex;
}
