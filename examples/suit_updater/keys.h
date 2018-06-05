
#include "cose.h"
#include "cose/crypto.h"

static unsigned char pubkey[COSE_CRYPTO_SIGN_ED25519_PUBLICKEYBYTES] = {
  0x54, 0x8d, 0x2e, 0xe7, 0x96, 0x82, 0x63, 0xd6, 0xfe, 0xbf, 0x73, 0x82,
  0xea, 0x3e, 0x9b, 0x8b, 0xf8, 0xcb, 0x60, 0xf3, 0xd0, 0x7f, 0x4a, 0x3e,
  0xc0, 0x02, 0x67, 0x5b, 0xf3, 0xc6, 0xa5, 0xa3
};