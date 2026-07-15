// This file was taken from the ZNU Kernel Source
// https://github.com/turbomaster95/znu

/*	       The Nicense Software License
 *                     Version 1.1
 * 
 * Copyright (c) 2026 Deva Midhun. All rights reserved.
 * 
 * NOTICE: This license must be included, intact and unmodified, in all copies
 * or substantial portions of the Software, in any form — including but not
 * limited to source code, object code, compiled binaries, bytecode, minified
 * or obfuscated code, assembled firmware, documentation, and any derivative
 * or combined works. Redistribution in any form without this notice is a
 * violation of this License.
 * 
 * --------------------------------------------------------------------------------
 * PREAMBLE
 * --------------------------------------------------------------------------------
 * 
 * This License governs the use, reproduction, modification, and distribution of
 * the Software and any Derivative Works thereof. By obtaining, using, copying,
 * modifying, or distributing the Software, You accept and agree to be bound by
 * the terms of this License. If You do not agree to these terms, You must not
 * use or distribute the Software.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 0 — DEFINITIONS
 * --------------------------------------------------------------------------------
 * 
 * "Author"            means Deva Midhun, the original copyright holder of
 *                     the Software.
 * 
 * "Contributor"       means any individual or legal entity that submits a
 *                     Contribution to the Work with the intent that it be
 *                     incorporated into the Work.
 * 
 * "Contribution"      means any work of authorship, including modifications or
 *                     additions to the Work, submitted by a Contributor to the
 *                     Author or to the Work's repository.
 * 
 * "Derivative Work"   means any work that is based on or derived from the
 *                     Software, including translations, adaptations, abridgements,
 *                     condensations, modifications, or any other form in which
 *                     the Work may be recast, transformed, or adapted.
 * 
 * "Distribution"      means making the Software or Derivative Works available to
 *                     any third party by any means, including but not limited to
 *                     electronic transmission, physical media, or network access.
 * 
 * "License"           means this Nicense Software License, Version 1.1.
 * 
 * "NOTICE file"       means a human-readable file named "NOTICE" distributed with
 *                     the Work that contains attribution information required by
 *                     the Author or Contributors.
 * 
 * "Object Form"       means any form resulting from mechanical transformation or
 *                     translation of a Source Form, including but not limited to
 *                     compiled object code, bytecode, generated documentation,
 *                     assembled firmware, or minified or obfuscated code.
 * 
 * "Source Form"       means the preferred form for making modifications, including
 *                     but not limited to software source code, documentation
 *                     source, and configuration files.
 * 
 * "Software" / "Work" means the copyrightable work made available under this
 *                     License, in either Source Form or Object Form.
 * 
 * "You" / "Your"      means an individual or legal entity exercising rights
 *                     granted under this License.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 1 — GRANT OF COPYRIGHT LICENSE
 * --------------------------------------------------------------------------------
 * 
 * Subject to the terms and conditions of this License, the Author hereby grants
 * to You a perpetual, worldwide, non-exclusive, no-charge, royalty-free,
 * irrevocable copyright license to reproduce, prepare Derivative Works of,
 * publicly display, publicly perform, sublicense, and distribute the Software
 * and such Derivative Works in Source Form or Object Form.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 2 — GRANT OF PATENT LICENSE
 * --------------------------------------------------------------------------------
 * 
 * Subject to the terms and conditions of this License, each Contributor hereby
 * grants to You a perpetual, worldwide, non-exclusive, no-charge, royalty-free,
 * irrevocable patent license to make, have made, use, offer to sell, sell,
 * import, and otherwise transfer the Work. This grant applies only to those
 * patent claims licensable by such Contributor that are necessarily infringed
 * by their Contribution(s) alone or by combination of their Contribution(s)
 * with the Work to which such Contribution(s) was submitted.
 * 
 * If You institute patent litigation against any entity (including a cross-claim
 * or counterclaim) alleging that the Work or a Contribution incorporated within
 * it constitutes direct or contributory patent infringement, then any patent
 * licenses granted to You under this License shall terminate as of the date
 * such litigation is filed.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 3 — REDISTRIBUTION AND INCLUSION REQUIREMENTS
 * --------------------------------------------------------------------------------
 * 
 * You may reproduce and distribute copies of the Work or Derivative Works thereof
 * in any medium — including Source Form, Object Form, compiled binaries,
 * bytecode, minified code, obfuscated code, firmware, or any other form —
 * with or without modifications, provided that You meet ALL of the following
 * conditions:
 * 
 *   (a) NOTICE INCLUSION. This License, including the copyright notice in its
 *       header, must be included verbatim and unmodified in all copies or
 *       substantial portions of the Software, regardless of form. You must not
 *       remove, alter, or obscure this notice.
 * 
 *   (b) LICENSE DELIVERY. You must provide any recipient of the Work or
 *       Derivative Works with a copy of this License. This applies whether the
 *       Software is distributed as source code, compiled binary, embedded
 *       firmware, or through a network service.
 * 
 *   (c) MODIFICATION DISCLOSURE. If You modify the Work, You must include a
 *       prominent notice in the modified files stating that You changed the
 *       files, the nature of the changes, and the date of such changes.
 * 
 *   (d) ATTRIBUTION RETENTION. You must retain, in the Source Form of any
 *       Derivative Work that You distribute, all copyright, patent, trademark,
 *       and attribution notices from the Source Form of the Work, excluding
 *       notices that do not pertain to any part of the Derivative Work.
 * 
 *   (e) NOTICE FILE. If the Work includes a NOTICE file, You must include a
 *       readable copy of the attribution notices contained within such NOTICE
 *       file in any Derivative Works that You distribute.
 * 
 *   (f) OBJECT FORM NOTICE. When distributing the Work or Derivative Works in
 *       Object Form (e.g., compiled binaries, minified files, firmware), You
 *       must ensure that the copyright notice and a reference to this License
 *       are either embedded in the Object Form itself, displayed at runtime
 *       where technically feasible, or provided in accompanying documentation.
 * 
 *   (g) SAAS AND NETWORK USE. If You operate the Software or a Derivative Work
 *       as a service made available to third parties over a network, You must
 *       make this License and the applicable copyright notice accessible to
 *       users of that service (e.g., in a legal notices or attribution page).
 * 
 *   (h) NO MISREPRESENTATION. You must not represent any Derivative Work as the
 *       original Work, nor claim authorship over unmodified portions of the
 *       Software. You must not use the Author's name to endorse or promote
 *       products derived from this Software without prior written permission.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 4 — CONTRIBUTIONS AND SUBMISSIONS
 * --------------------------------------------------------------------------------
 * 
 * Unless explicitly stated otherwise, any Contribution intentionally submitted
 * for inclusion in the Work by You to the Author shall be under the terms and
 * conditions of this License, without any additional terms or conditions.
 * 
 * Nothing in this section shall supersede or modify the terms of any separate
 * written license agreement You may have executed with the Author regarding
 * such Contributions.
 * 
 * The Author reserves the right to accept, reject, or modify any Contribution
 * at their sole discretion.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 5 — TRADEMARKS
 * --------------------------------------------------------------------------------
 * 
 * This License does not grant permission to use the trade names, trademarks,
 * service marks, or product names of the Author, except as reasonably required
 * for describing the origin of the Work and reproducing the content of any
 * required NOTICE file.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 6 — PRIVACY AND DATA
 * --------------------------------------------------------------------------------
 * 
 * If the Software collects, processes, or transmits personal data, You are
 * solely responsible for ensuring that any such use complies with all applicable
 * privacy and data protection laws in the jurisdictions in which You operate
 * or in which Your users are located. The Author makes no representation that
 * the Software is compliant with any particular privacy law or regulation.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 7 — EXPORT COMPLIANCE
 * --------------------------------------------------------------------------------
 * 
 * The Software may be subject to export control laws and regulations. You are
 * solely responsible for compliance with all applicable export and import laws
 * and regulations. You must not export or re-export the Software in violation
 * of any applicable laws.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 8 — DISCLAIMER OF WARRANTY
 * --------------------------------------------------------------------------------
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND NON-INFRINGEMENT. THE AUTHOR
 * AND CONTRIBUTORS MAKE NO WARRANTY THAT:
 * 
 *   (a) THE SOFTWARE WILL MEET YOUR REQUIREMENTS;
 *   (b) THE SOFTWARE WILL BE UNINTERRUPTED, TIMELY, SECURE, OR ERROR-FREE;
 *   (c) THE RESULTS OBTAINED FROM USE OF THE SOFTWARE WILL BE ACCURATE OR
 *       RELIABLE; OR
 *   (d) ANY ERRORS IN THE SOFTWARE WILL BE CORRECTED.
 * 
 * YOU ASSUME ALL RISKS ASSOCIATED WITH THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE. SHOULD THE SOFTWARE PROVE DEFECTIVE, YOU ASSUME THE ENTIRE COST
 * OF ALL NECESSARY SERVICING, REPAIR, OR CORRECTION. SOME JURISDICTIONS DO NOT
 * ALLOW THE EXCLUSION OF IMPLIED WARRANTIES — IN SUCH CASES, THIS EXCLUSION
 * APPLIES TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 9 — LIMITATION OF LIABILITY
 * --------------------------------------------------------------------------------
 * 
 * IN NO EVENT AND UNDER NO LEGAL THEORY — WHETHER IN TORT (INCLUDING NEGLIGENCE),
 * CONTRACT, STRICT LIABILITY, OR OTHERWISE — SHALL THE AUTHOR OR ANY CONTRIBUTOR
 * BE LIABLE TO YOU FOR ANY DAMAGES, INCLUDING ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, PUNITIVE, OR CONSEQUENTIAL DAMAGES OF ANY CHARACTER
 * (INCLUDING BUT NOT LIMITED TO: DAMAGES FOR LOSS OF GOODWILL, BUSINESS
 * INTERRUPTION, DATA LOSS, LOSS OF PROFITS, OR PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES), EVEN IF THE AUTHOR OR CONTRIBUTOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 * 
 * THIS LIMITATION APPLIES:
 * 
 *   (a) TO THE FULLEST EXTENT PERMITTED BY APPLICABLE LAW; AND
 *   (b) REGARDLESS OF WHETHER SUCH DAMAGES ARISE FROM USE OR INABILITY TO USE
 *       THE SOFTWARE, FROM UNAUTHORIZED ACCESS TO OR ALTERATION OF YOUR DATA,
 *       OR FROM ANY OTHER CAUSE RELATED TO THIS LICENSE.
 * 
 * SOME JURISDICTIONS DO NOT ALLOW THE LIMITATION OR EXCLUSION OF INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES — IN SUCH CASES, THIS LIMITATION APPLIES TO THE MAXIMUM
 * EXTENT PERMITTED BY APPLICABLE LAW.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 10 — INDEMNIFICATION
 * --------------------------------------------------------------------------------
 * 
 * You agree to indemnify, defend, and hold harmless the Author and all
 * Contributors from and against any and all claims, damages, losses, liabilities,
 * costs, and expenses (including reasonable attorneys' fees) arising out of or
 * relating to:
 * 
 *   (a) Your use, modification, or distribution of the Software;
 *   (b) Your violation of any term of this License;
 *   (c) Any product, service, or Derivative Work You create using the Software;
 *   (d) Your violation of any applicable law or third-party right in connection
 *       with the use or distribution of the Software.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 11 — GOVERNING LAW AND DISPUTE RESOLUTION
 * --------------------------------------------------------------------------------
 * 
 * This License shall be governed by and construed in accordance with applicable
 * law, without regard to conflict-of-law principles. Any dispute arising out of
 * or in connection with this License shall first be subject to good-faith
 * negotiation between the parties. If unresolved within thirty (30) days, either
 * party may pursue any remedy available under applicable law.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 12 — TERMINATION
 * --------------------------------------------------------------------------------
 * 
 * (a) AUTOMATIC TERMINATION. Your rights under this License shall terminate
 *     automatically and without notice if You fail to comply with any term or
 *     condition of this License.
 * 
 * (b) TERMINATION UPON PATENT LITIGATION. As described in Section 2, Your
 *     patent license shall terminate if You initiate patent litigation against
 *     any entity alleging that the Work constitutes patent infringement.
 * 
 * (c) REINSTATEMENT. If You cure the violation within thirty (30) days of
 *     becoming aware of it, Your rights may be reinstated at the Author's
 *     sole discretion.
 * 
 * (d) SURVIVAL. Sections 0, 8, 9, 10, and 11 shall survive any termination
 *     of this License.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 13 — SEVERABILITY AND WAIVER
 * --------------------------------------------------------------------------------
 * 
 * If any provision of this License is held to be unenforceable or invalid under
 * applicable law, such provision shall be modified to the minimum extent
 * necessary to make it enforceable, and the remaining provisions shall continue
 * in full force and effect.
 * 
 * The Author's failure to enforce any provision of this License shall not
 * constitute a waiver of that provision or any future right to enforce it.
 * 
 * --------------------------------------------------------------------------------
 * SECTION 14 — ENTIRE AGREEMENT
 * --------------------------------------------------------------------------------
 * 
 * This License constitutes the entire agreement between You and the Author
 * with respect to the Software and supersedes all prior or contemporaneous
 * understandings, representations, or agreements regarding the Software, unless
 * You have a separate written agreement executed with the Author.
 * 
 */

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
