#include "carrot_module.h"
#include <memory>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/types.h>
#include <string>

struct OpenSSL_B_Gone {
  void operator()(EVP_PKEY *p) const { EVP_PKEY_free(p); }
  void operator()(EVP_PKEY_CTX *p) const { EVP_PKEY_CTX_free(p); }
  void operator()(BIO *p) const { BIO_free_all(p); }
};

bool isValidPublicKey(const std::string &pemPublicKey) {
  if (pemPublicKey.empty()) return false;

  if (pemPublicKey.find("-----BEGIN PUBLIC KEY-----") == std::string::npos ||
      pemPublicKey.find("-----END PUBLIC KEY-----") == std::string::npos) {
    return false;
  }

  std::unique_ptr<BIO, OpenSSL_B_Gone> bioKey(BIO_new_mem_buf(
      pemPublicKey.data(), static_cast<int>(pemPublicKey.size())));
  if (!bioKey) return false;

  std::unique_ptr<EVP_PKEY, OpenSSL_B_Gone> pkey(
      PEM_read_bio_PUBKEY(bioKey.get(), nullptr, nullptr, nullptr));

  return pkey != nullptr;
}

std::string b64enc(const std::vector<unsigned char> &data) {
  std::unique_ptr<BIO, OpenSSL_B_Gone> b64(BIO_new(BIO_f_base64()));
  std::unique_ptr<BIO, OpenSSL_B_Gone> bioMem(BIO_new(BIO_s_mem()));

  BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
  BIO_push(b64.get(), bioMem.get());

  BIO_write(b64.get(), data.data(), static_cast<int>(data.size()));
  BIO_flush(b64.get());

  char *outputBuffer;
  long outputLength = BIO_get_mem_data(bioMem.get(), &outputBuffer);

  std::string result(outputBuffer, outputLength);
  bioMem.release();
  return result;
}

std::string sslCrypt2b64(const std::string &input, const std::string &pubkey) {
  std::unique_ptr<BIO, OpenSSL_B_Gone> bio(
      BIO_new_mem_buf(pubkey.data(), static_cast<int>(pubkey.size())));
  if (!bio)
    return "";

  std::unique_ptr<EVP_PKEY, OpenSSL_B_Gone> pkey(
      PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));
  if (!pkey)
    return "";

  std::unique_ptr<EVP_PKEY_CTX, OpenSSL_B_Gone> ctx(
      EVP_PKEY_CTX_new(pkey.get(), nullptr));
  if (!ctx || EVP_PKEY_encrypt_init(ctx.get()) <= 0)
    return "";
  if (EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_PADDING) <= 0)
    return "";

  size_t ciphertextLen = 0;
  const unsigned char *inBuf =
      reinterpret_cast<const unsigned char *>(input.data());

  if (EVP_PKEY_encrypt(ctx.get(), nullptr, &ciphertextLen, inBuf,
                       input.size()) <= 0)
    return "";

  std::vector<unsigned char> ciphertext(ciphertextLen);
  if (EVP_PKEY_encrypt(ctx.get(), ciphertext.data(), &ciphertextLen, inBuf,
                       input.size()) <= 0)
    return "";
  ciphertext.resize(ciphertextLen);

  return b64enc(ciphertext);
}
