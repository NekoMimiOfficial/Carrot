/*
 * This file is part of the Carrot/Ninjin project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Deva Midhun
 */

// This file was taken from the ZNU Kernel Source
// https://github.com/turbomaster95/znu

#ifndef ENTROPY_H
#define ENTROPY_H

#include <stdint.h>
#include <stddef.h>

#define DRBG_SEED_LEN       55   /* 440 bits for 256-bit security */
#define DRBG_V_LEN          55
#define DRBG_C_LEN          55
#define DRBG_RESEED_INTERVAL ((uint64_t)1 << 48)
#define DRBG_MAX_BYTES_PER_REQ 65536

typedef struct {
    uint8_t V[DRBG_V_LEN];       /* internal state V */
    uint8_t C[DRBG_C_LEN];       /* internal state C */
    uint64_t reseed_counter;     /* generation count since last reseed */
    int seeded;                  /* 0 = not seeded, 1 = seeded */
} drbg_ctx_t;

typedef struct {
    uint8_t accumulator[64];     /* SHA-256 hash of all entropy added */
    uint64_t entropy_estimate;   /* rough estimate of bits collected */
    int initialized;
} entropy_pool_t;

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} sha256_ctx_t;

void drbg_instantiate(drbg_ctx_t *ctx, const uint8_t *entropy, size_t ent_len,
                      const uint8_t *nonce, size_t nonce_len,
                      const uint8_t *pers, size_t pers_len);
void drbg_reseed(drbg_ctx_t *ctx, const uint8_t *entropy, size_t ent_len,
                 const uint8_t *addin, size_t addin_len);
int drbg_generate(drbg_ctx_t *ctx, uint8_t *out, size_t out_len,
                  const uint8_t *addin, size_t addin_len);
void drbg_uninstantiate(drbg_ctx_t *ctx);

void entropy_pool_init(entropy_pool_t *pool);
void entropy_pool_add(entropy_pool_t *pool, const uint8_t *data, size_t len);
void entropy_pool_extract(entropy_pool_t *pool, uint8_t *out, size_t out_len);

void secure_random_bytes(drbg_ctx_t *ctx, uint8_t *out, size_t len);
uint32_t secure_random_u32(drbg_ctx_t *ctx);
uint64_t secure_random_u64(drbg_ctx_t *ctx);
void secure_random_string(drbg_ctx_t *ctx, char *dest, size_t len);

int cpu_has_rdrand(void);
int cpu_has_rdseed(void);
int rdrand64(uint64_t *val);
int rdseed64(uint64_t *val);
void gather_hardware_entropy(uint8_t *out, size_t len);

void sha256_transform(sha256_ctx_t *ctx, const uint8_t data[64]);
void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len);
void sha256_final(sha256_ctx_t *ctx, uint8_t hash[32]);

#endif /* ENTROPY_H */
