/*
 * This file is part of the Carrot/Ninjin project.
 *
 * This module is dual-licensed under the Nicense Software License v1.1
 * AND the Apache License, Version 2.0. You may choose either license.
 *
 * For the full terms of the Nicense, see the LICENSE.nicense file in this directory.
 */

// This file was taken from the ZNU Kernel Source
// https://github.com/turbomaster95/znu

#include <entropy.h>

static inline size_t local_strlen(const char *s) {
     size_t len = 0;
     while (s[len]) len++;
     return len;
}

static inline void local_memcpy(void *dst, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) *d++ = *s++;
}

static inline void local_memset(void *dst, int c, size_t n) {
    unsigned char *d = (unsigned char *)dst;
    while (n--) *d++ = (unsigned char)c;
}

static inline int local_memcmp(const void *a, const void *b, size_t n) {
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;
    int diff = 0;
    while (n--) diff |= (*pa++ ^ *pb++);
    return diff;
}

#define ROTRIGHT(word, bits) (((word) >> (bits)) | ((word) << (32 - (bits))))
#define CH(x, y, z)          (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z)         (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)               (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x)               (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x)              (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x)              (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

static const uint32_t k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

void sha256_transform(sha256_ctx_t *ctx, const uint8_t data[64]) {
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = ((uint32_t)data[j] << 24) | ((uint32_t)data[j + 1] << 16) |
               ((uint32_t)data[j + 2] << 8) | ((uint32_t)data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2]; d = ctx->state[3];
    e = ctx->state[4]; f = ctx->state[5]; g = ctx->state[6]; h = ctx->state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g; g = f; f = e;
        e = d + t1;
        d = c; c = b; b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c; ctx->state[3] += d;
    ctx->state[4] += e; ctx->state[5] += f; ctx->state[6] += g; ctx->state[7] += h;
}

void sha256_init(sha256_ctx_t *ctx) {
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667; ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372; ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f; ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab; ctx->state[7] = 0x5be0cd19;
}

void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

void sha256_final(sha256_ctx_t *ctx, uint8_t hash[32]) {
    uint32_t i = ctx->datalen;

    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56) ctx->data[i++] = 0x00;
    } else {
        ctx->data[i++] = 0x80;
        while (i < 64) ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        for (i = 0; i < 56; i++) ctx->data[i] = 0x00;
    }

    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = (uint8_t)(ctx->bitlen);
    ctx->data[62] = (uint8_t)(ctx->bitlen >> 8);
    ctx->data[61] = (uint8_t)(ctx->bitlen >> 16);
    ctx->data[60] = (uint8_t)(ctx->bitlen >> 24);
    ctx->data[59] = (uint8_t)(ctx->bitlen >> 32);
    ctx->data[58] = (uint8_t)(ctx->bitlen >> 40);
    ctx->data[57] = (uint8_t)(ctx->bitlen >> 48);
    ctx->data[56] = (uint8_t)(ctx->bitlen >> 56);
    sha256_transform(ctx, ctx->data);

    for (i = 0; i < 4; ++i) {
        hash[i]      = (uint8_t)((ctx->state[0] >> (24 - i * 8)) & 0xff);
        hash[i + 4]  = (uint8_t)((ctx->state[1] >> (24 - i * 8)) & 0xff);
        hash[i + 8]  = (uint8_t)((ctx->state[2] >> (24 - i * 8)) & 0xff);
        hash[i + 12] = (uint8_t)((ctx->state[3] >> (24 - i * 8)) & 0xff);
        hash[i + 16] = (uint8_t)((ctx->state[4] >> (24 - i * 8)) & 0xff);
        hash[i + 20] = (uint8_t)((ctx->state[5] >> (24 - i * 8)) & 0xff);
        hash[i + 24] = (uint8_t)((ctx->state[6] >> (24 - i * 8)) & 0xff);
        hash[i + 28] = (uint8_t)((ctx->state[7] >> (24 - i * 8)) & 0xff);
    }
}

/* One-shot SHA-256 */
static void sha256(const uint8_t *data, size_t len, uint8_t out[32]) {
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, out);
    local_memset(&ctx, 0, sizeof(ctx));
}

/*
 * Hash_df: derivation function using SHA-256
 * Generates requested_no_of_bits from input_string
 */
static void hash_df(const uint8_t *input, size_t input_len,
                    uint8_t *out, size_t out_len) {
    sha256_ctx_t ctx;
    uint8_t counter = 1;
    size_t offset = 0;
    uint8_t len_bytes[4];

    len_bytes[0] = (uint8_t)((out_len * 8) >> 24);
    len_bytes[1] = (uint8_t)((out_len * 8) >> 16);
    len_bytes[2] = (uint8_t)((out_len * 8) >> 8);
    len_bytes[3] = (uint8_t)((out_len * 8));

    while (offset < out_len) {
        sha256_init(&ctx);
        sha256_update(&ctx, &counter, 1);
        sha256_update(&ctx, len_bytes, 4);
        sha256_update(&ctx, input, input_len);

        uint8_t hash[32];
        sha256_final(&ctx, hash);

        size_t copy = (out_len - offset < 32) ? (out_len - offset) : 32;
        local_memcpy(out + offset, hash, copy);
        offset += copy;
        counter++;
    }
    local_memset(&ctx, 0, sizeof(ctx));
}

/* Hashgen: generate pseudorandom bits (Section 10.1.1.4) */
static void hashgen(const uint8_t *V, uint8_t *out, size_t out_len) {
    sha256_ctx_t ctx;
    size_t offset = 0;
    uint8_t data[DRBG_V_LEN];
    local_memcpy(data, V, DRBG_V_LEN);

    while (offset < out_len) {
        sha256_init(&ctx);
        sha256_update(&ctx, data, DRBG_V_LEN);

        uint8_t hash[32];
        sha256_final(&ctx, hash);

        size_t copy = (out_len - offset < 32) ? (out_len - offset) : 32;
        local_memcpy(out + offset, hash, copy);
        offset += copy;

        /* V = (V + 1) mod 2^seedlen */
        int carry = 1;
        for (int i = DRBG_V_LEN - 1; i >= 0 && carry; i--) {
            int sum = data[i] + carry;
            data[i] = (uint8_t)sum;
            carry = sum >> 8;
        }
    }
    local_memset(data, 0, sizeof(data));
    local_memset(&ctx, 0, sizeof(ctx));
}

void drbg_instantiate(drbg_ctx_t *ctx, const uint8_t *entropy, size_t ent_len,
                      const uint8_t *nonce, size_t nonce_len,
                      const uint8_t *pers, size_t pers_len) {
    uint8_t seed_material[512];
    size_t seed_len = 0;

    /* seed_material = entropy || nonce || personalization_string */
    if (ent_len > 0 && ent_len <= 128) {
        local_memcpy(seed_material + seed_len, entropy, ent_len);
        seed_len += ent_len;
    }
    if (nonce_len > 0 && nonce_len <= 64) {
        local_memcpy(seed_material + seed_len, nonce, nonce_len);
        seed_len += nonce_len;
    }
    if (pers_len > 0 && pers_len <= 256) {
        local_memcpy(seed_material + seed_len, pers, pers_len);
        seed_len += pers_len;
    }

    hash_df(seed_material, seed_len, ctx->V, DRBG_V_LEN);

    uint8_t C_input[DRBG_V_LEN + 1];
    local_memcpy(C_input, ctx->V, DRBG_V_LEN);
    C_input[DRBG_V_LEN] = 0x00;
    hash_df(C_input, DRBG_V_LEN + 1, ctx->C, DRBG_C_LEN);

    ctx->reseed_counter = 1;
    ctx->seeded = 1;

    local_memset(seed_material, 0, sizeof(seed_material));
    local_memset(C_input, 0, sizeof(C_input));
}

void drbg_reseed(drbg_ctx_t *ctx, const uint8_t *entropy, size_t ent_len,
                 const uint8_t *addin, size_t addin_len) {
    if (!ctx->seeded) return;

    uint8_t seed_material[512];
    size_t seed_len = 0;

    /* seed_material = 0x01 || V || entropy || additional_input */
    seed_material[seed_len++] = 0x01;
    local_memcpy(seed_material + seed_len, ctx->V, DRBG_V_LEN);
    seed_len += DRBG_V_LEN;

    if (ent_len > 0 && ent_len <= 128) {
        local_memcpy(seed_material + seed_len, entropy, ent_len);
        seed_len += ent_len;
    }
    if (addin_len > 0 && addin_len <= 256) {
        local_memcpy(seed_material + seed_len, addin, addin_len);
        seed_len += addin_len;
    }

    hash_df(seed_material, seed_len, ctx->V, DRBG_V_LEN);

    uint8_t C_input[DRBG_V_LEN + 1];
    local_memcpy(C_input, ctx->V, DRBG_V_LEN);
    C_input[DRBG_V_LEN] = 0x00;
    hash_df(C_input, DRBG_V_LEN + 1, ctx->C, DRBG_C_LEN);

    ctx->reseed_counter = 1;

    local_memset(seed_material, 0, sizeof(seed_material));
    local_memset(C_input, 0, sizeof(C_input));
}

int drbg_generate(drbg_ctx_t *ctx, uint8_t *out, size_t out_len,
                  const uint8_t *addin, size_t addin_len) {
    if (!ctx->seeded) return -1;
    if (out_len > DRBG_MAX_BYTES_PER_REQ) return -1;
    if (ctx->reseed_counter > DRBG_RESEED_INTERVAL) return -1;

    /* If additional input provided, update state first */
    if (addin_len > 0) {
        uint8_t w[32];
        sha256_ctx_t hctx;
        sha256_init(&hctx);
        sha256_update(&hctx, ctx->V, DRBG_V_LEN);
        sha256_update(&hctx, addin, addin_len);
        sha256_final(&hctx, w);

        /* V = (V + w) mod 2^seedlen */
        int carry = 0;
        for (int i = DRBG_V_LEN - 1; i >= 0; i--) {
            int sum = ctx->V[i] + w[i % 32] + carry;
            ctx->V[i] = (uint8_t)sum;
            carry = sum >> 8;
        }
        local_memset(w, 0, sizeof(w));
        local_memset(&hctx, 0, sizeof(hctx));
    }

    /* Generate output */
    hashgen(ctx->V, out, out_len);

    /* Update state: H = Hash(0x03 || V) */
    uint8_t H_input[DRBG_V_LEN + 1];
    H_input[0] = 0x03;
    local_memcpy(H_input + 1, ctx->V, DRBG_V_LEN);

    uint8_t H[32];
    sha256(H_input, DRBG_V_LEN + 1, H);

    /* V = (V + H + C + reseed_counter) mod 2^seedlen */
    int carry = 0;
    for (int i = DRBG_V_LEN - 1; i >= 0; i--) {
        int sum = ctx->V[i] + H[i % 32] + carry;
        ctx->V[i] = (uint8_t)sum;
        carry = sum >> 8;
    }

    carry = 0;
    for (int i = DRBG_V_LEN - 1; i >= 0; i--) {
        int sum = ctx->V[i] + ctx->C[i] + carry;
        ctx->V[i] = (uint8_t)sum;
        carry = sum >> 8;
    }

    uint8_t rc_bytes[8];
    rc_bytes[0] = (uint8_t)(ctx->reseed_counter >> 56);
    rc_bytes[1] = (uint8_t)(ctx->reseed_counter >> 48);
    rc_bytes[2] = (uint8_t)(ctx->reseed_counter >> 40);
    rc_bytes[3] = (uint8_t)(ctx->reseed_counter >> 32);
    rc_bytes[4] = (uint8_t)(ctx->reseed_counter >> 24);
    rc_bytes[5] = (uint8_t)(ctx->reseed_counter >> 16);
    rc_bytes[6] = (uint8_t)(ctx->reseed_counter >> 8);
    rc_bytes[7] = (uint8_t)(ctx->reseed_counter);

    carry = 0;
    for (int i = DRBG_V_LEN - 1; i >= DRBG_V_LEN - 8; i--) {
        int sum = ctx->V[i] + rc_bytes[DRBG_V_LEN - 1 - i] + carry;
        ctx->V[i] = (uint8_t)sum;
        carry = sum >> 8;
    }
    if (carry && DRBG_V_LEN > 8) {
        for (int i = DRBG_V_LEN - 9; i >= 0 && carry; i--) {
            int sum = ctx->V[i] + carry;
            ctx->V[i] = (uint8_t)sum;
            carry = sum >> 8;
        }
    }

    ctx->reseed_counter++;

    local_memset(H_input, 0, sizeof(H_input));
    local_memset(H, 0, sizeof(H));
    local_memset(rc_bytes, 0, sizeof(rc_bytes));

    return 0;
}

void drbg_uninstantiate(drbg_ctx_t *ctx) {
    local_memset(ctx, 0, sizeof(drbg_ctx_t));
}

void entropy_pool_init(entropy_pool_t *pool) {
    local_memset(pool, 0, sizeof(entropy_pool_t));
    pool->initialized = 1;
}

void entropy_pool_add(entropy_pool_t *pool, const uint8_t *data, size_t len) {
    if (!pool->initialized) entropy_pool_init(pool);

    /* Mix new data into accumulator using SHA-256 */
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, pool->accumulator, 64);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, pool->accumulator);

    /* Rough entropy estimate: assume 4 bits per byte of input */
    pool->entropy_estimate += len * 4;

    local_memset(&ctx, 0, sizeof(ctx));
}

void entropy_pool_extract(entropy_pool_t *pool, uint8_t *out, size_t out_len) {
    if (!pool->initialized) entropy_pool_init(pool);

    /* Use hash_df to stretch accumulator to requested length */
    hash_df(pool->accumulator, 64, out, out_len);

    /* Re-mix accumulator to prevent backtracking */
    sha256(pool->accumulator, 64, pool->accumulator);

    pool->entropy_estimate = 0;
}

int cpu_has_rdrand(void) {
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
        : "memory"
    );
    return (ecx >> 30) & 1;
}

int cpu_has_rdseed(void) {
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(7), "c"(0)
        : "memory"
    );
    return (ebx >> 18) & 1;
}

int rdrand64(uint64_t *val) {
    unsigned char ok;
    __asm__ volatile (
        "rdrand %0\n\t"
        "setc %1"
        : "=r" (*val), "=qm" (ok)
        :
        : "cc"
    );
    return (int)ok;
}

int rdseed64(uint64_t *val) {
    unsigned char ok;
    __asm__ volatile (
        "rdseed %0\n\t"
        "setc %1"
        : "=r" (*val), "=qm" (ok)
        :
        : "cc"
    );
    return (int)ok;
}

void gather_hardware_entropy(uint8_t *out, size_t len) {
    size_t offset = 0;
    int has_rdseed = cpu_has_rdseed();
    int has_rdrand = cpu_has_rdrand();

    while (offset < len) {
        uint64_t val = 0;
        int ok = 0;

        /* Prefer RDSEED (true entropy) over RDRAND (AES-CTR based) */
        if (has_rdseed) {
            for (int retry = 0; retry < 10 && !ok; retry++) {
                ok = rdseed64(&val);
            }
        }
        if (!ok && has_rdrand) {
            for (int retry = 0; retry < 10 && !ok; retry++) {
                ok = rdrand64(&val);
            }
        }

        if (ok) {
            size_t copy = (len - offset < 8) ? (len - offset) : 8;
            local_memcpy(out + offset, &val, copy);
            offset += copy;
        } else {
            /* Fallback: RDTSC + jitter */
            uint32_t lo, hi;
            __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
            val = ((uint64_t)hi << 32) | lo;
            /* Add some jitter */
            for (volatile int i = 0; i < 100; i++);

            size_t copy = (len - offset < 8) ? (len - offset) : 8;
            local_memcpy(out + offset, &val, copy);
            offset += copy;
        }
    }
}

void secure_random_bytes(drbg_ctx_t *ctx, uint8_t *out, size_t len) {
    size_t offset = 0;
    while (offset < len) {
        size_t chunk = (len - offset < DRBG_MAX_BYTES_PER_REQ) ? 
                       (len - offset) : DRBG_MAX_BYTES_PER_REQ;
        if (drbg_generate(ctx, out + offset, chunk, NULL, 0) != 0) {
            /* Reseed required */
            uint8_t entropy[64];
            gather_hardware_entropy(entropy, sizeof(entropy));
            drbg_reseed(ctx, entropy, sizeof(entropy), NULL, 0);
            drbg_generate(ctx, out + offset, chunk, NULL, 0);
            local_memset(entropy, 0, sizeof(entropy));
        }
        offset += chunk;
    }
}

uint32_t secure_random_u32(drbg_ctx_t *ctx) {
    uint32_t val;
    secure_random_bytes(ctx, (uint8_t *)&val, sizeof(val));
    return val;
}

uint64_t secure_random_u64(drbg_ctx_t *ctx) {
    uint64_t val;
    secure_random_bytes(ctx, (uint8_t *)&val, sizeof(val));
    return val;
}

void secure_random_string(drbg_ctx_t *ctx, char *dest, size_t len) {
    static const char charset[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const size_t charset_size = sizeof(charset) - 1;

    /* For rejection sampling: find largest multiple of charset_size 
       that fits in a byte (255 = 3*85, 256 = 3*85 + 1, so limit = 3*85 = 255) */
    const uint8_t limit = (uint8_t)(256 - (256 % charset_size));

    size_t i = 0;
    while (i < len) {
        uint8_t byte;
        secure_random_bytes(ctx, &byte, 1);
        if (byte < limit) {
            dest[i++] = charset[byte % charset_size];
        }
        /* Reject values >= limit to eliminate modulo bias */
    }
    dest[len] = '\0';
}
