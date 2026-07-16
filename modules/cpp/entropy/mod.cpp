#include "carrot_module.h"

extern "C" {
#include <entropy.h>
}

#include <cstring>
#include <stdexcept>
#include <vector>

static std::string bytesToHex(const uint8_t *data, size_t len) {
    static const char hex[] = "0123456789abcdef";
    std::string result;
    result.reserve(len * 2);
    for (size_t i = 0; i < len; i++) {
        result += hex[data[i] >> 4];
        result += hex[data[i] & 0xf];
    }
    return result;
}

static std::shared_ptr<NinArray> bytesToArray(const uint8_t *data, size_t len) {
    std::vector<Value> elems;
    elems.reserve(len);
    for (size_t i = 0; i < len; i++)
        elems.push_back(static_cast<double>(data[i]));
    return std::make_shared<NinArray>(std::move(elems));
}

static drbg_ctx_t *getDefaultDrbg() {
    static drbg_ctx_t ctx = {};
    static bool initialized = false;

    if (!initialized) {
        uint8_t entropy[64];
        uint8_t nonce[32];
        gather_hardware_entropy(entropy, sizeof(entropy));
        gather_hardware_entropy(nonce, sizeof(nonce));
        drbg_instantiate(&ctx, entropy, sizeof(entropy),
                         nonce, sizeof(nonce),
                         (const uint8_t *)"carrot_entropy_mod", 19);
        memset(entropy, 0, sizeof(entropy));
        memset(nonce, 0, sizeof(nonce));
        initialized = true;
    }
    return &ctx;
}

struct Sha256Fn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "sha256"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<std::string>(args[0]))
            throw std::runtime_error("sha256(): argument must be a string");
        const auto &s = std::get<std::string>(args[0]);
        uint8_t hash[32];
        sha256_ctx_t ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const uint8_t *)s.data(), s.size());
        sha256_final(&ctx, hash);
        memset(&ctx, 0, sizeof(ctx));
        return bytesToHex(hash, 32);
    }
};

struct Sha256BytesFn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "sha256Bytes"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<std::string>(args[0]))
            throw std::runtime_error("sha256Bytes(): argument must be a string");
        const auto &s = std::get<std::string>(args[0]);
        uint8_t hash[32];
        sha256_ctx_t ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const uint8_t *)s.data(), s.size());
        sha256_final(&ctx, hash);
        memset(&ctx, 0, sizeof(ctx));
        return bytesToArray(hash, 32);
    }
};

struct Sha256State {
    sha256_ctx_t ctx;
    Sha256State() { sha256_init(&ctx); }
    ~Sha256State() { memset(&ctx, 0, sizeof(ctx)); }
};

struct Sha256UpdateFn : NinCallable {
    std::shared_ptr<Sha256State> st;
    Sha256UpdateFn(std::shared_ptr<Sha256State> s) : st(s) {}
    int arity() override { return 1; }
    std::string name() override { return "update"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<std::string>(args[0]))
            throw std::runtime_error("sha256.update(): argument must be a string");
        const auto &s = std::get<std::string>(args[0]);
        sha256_update(&st->ctx, (const uint8_t *)s.data(), s.size());
        return std::monostate{};
    }
};

struct Sha256FinalFn : NinCallable {
    std::shared_ptr<Sha256State> st;
    Sha256FinalFn(std::shared_ptr<Sha256State> s) : st(s) {}
    int arity() override { return 0; }
    std::string name() override { return "final"; }
    Value call(std::vector<Value>) override {
        uint8_t hash[32];
        sha256_final(&st->ctx, hash);
        sha256_init(&st->ctx);
        return bytesToHex(hash, 32);
    }
};

struct Sha256FinalBytesFn : NinCallable {
    std::shared_ptr<Sha256State> st;
    Sha256FinalBytesFn(std::shared_ptr<Sha256State> s) : st(s) {}
    int arity() override { return 0; }
    std::string name() override { return "finalBytes"; }
    Value call(std::vector<Value>) override {
        uint8_t hash[32];
        sha256_final(&st->ctx, hash);
        sha256_init(&st->ctx);
        return bytesToArray(hash, 32);
    }
};

struct Sha256ResetFn : NinCallable {
    std::shared_ptr<Sha256State> st;
    Sha256ResetFn(std::shared_ptr<Sha256State> s) : st(s) {}
    int arity() override { return 0; }
    std::string name() override { return "reset"; }
    Value call(std::vector<Value>) override {
        sha256_init(&st->ctx);
        return std::monostate{};
    }
};

struct CreateSha256Fn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "createSha256"; }
    Value call(std::vector<Value>) override {
        auto st = std::make_shared<Sha256State>();
        auto native = std::make_shared<NinNative>();
        native->typeName = "Sha256Context";
        native->data = st;
        native->getField = [st](const std::string &f) -> Value {
            if (f == "update")      return std::make_shared<Sha256UpdateFn>(st);
            if (f == "final")       return std::make_shared<Sha256FinalFn>(st);
            if (f == "finalBytes")  return std::make_shared<Sha256FinalBytesFn>(st);
            if (f == "reset")       return std::make_shared<Sha256ResetFn>(st);
            return std::monostate{};
        };
        return native;
    }
};

struct RandomBytesFn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "randomBytes"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<double>(args[0]))
            throw std::runtime_error("randomBytes(): argument must be a number");
        size_t len = (size_t)std::get<double>(args[0]);
        if (len == 0 || len > 65536)
            throw std::runtime_error("randomBytes(): length must be 1..65536");
        std::vector<uint8_t> buf(len);
        secure_random_bytes(getDefaultDrbg(), buf.data(), len);
        return bytesToArray(buf.data(), len);
    }
};

struct RandomU32Fn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "randomU32"; }
    Value call(std::vector<Value>) override {
        return (double)secure_random_u32(getDefaultDrbg());
    }
};

struct RandomU64Fn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "randomU64"; }
    Value call(std::vector<Value>) override {
        return (double)secure_random_u64(getDefaultDrbg());
    }
};

struct RandomStringFn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "randomString"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<double>(args[0]))
            throw std::runtime_error("randomString(): argument must be a number");
        size_t len = (size_t)std::get<double>(args[0]);
        std::string out(len, '\0');
        secure_random_string(getDefaultDrbg(), out.data(), len);
        return out;
    }
};

struct RandomFn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "random"; }
    Value call(std::vector<Value>) override {
        uint64_t v = secure_random_u64(getDefaultDrbg());
        return (double)(v >> 11) / 9007199254740992.0;
    }
};

struct RandRangeFn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "randrange"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<double>(args[0]))
            throw std::runtime_error("randrange(): argument must be a number");
        double margin = std::get<double>(args[0]);
        uint64_t v = secure_random_u64(getDefaultDrbg());
        return (double)(v >> 11) / 9007199254740992.0 * margin;
    }
};

struct GatherHwEntropyFn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "gatherHardwareEntropy"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<double>(args[0]))
            throw std::runtime_error("gatherHardwareEntropy(): argument must be a number");
        size_t len = (size_t)std::get<double>(args[0]);
        std::vector<uint8_t> buf(len);
        gather_hardware_entropy(buf.data(), len);
        return bytesToArray(buf.data(), len);
    }
};

struct HasRdrandFn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "hasRdrand"; }
    Value call(std::vector<Value>) override { return cpu_has_rdrand() != 0; }
};

struct HasRdseedFn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "hasRdseed"; }
    Value call(std::vector<Value>) override { return cpu_has_rdseed() != 0; }
};

struct DrbgState {
    drbg_ctx_t ctx;
    bool valid = false;
    DrbgState() { memset(&ctx, 0, sizeof(ctx)); }
    ~DrbgState() { if (valid) drbg_uninstantiate(&ctx); }
};

struct DrbgGenerateFn : NinCallable {
    std::shared_ptr<DrbgState> st;
    DrbgGenerateFn(std::shared_ptr<DrbgState> s) : st(s) {}
    int arity() override { return 1; }
    std::string name() override { return "generate"; }
    Value call(std::vector<Value> args) override {
        if (!st->valid)
            throw std::runtime_error("drbg.generate(): not instantiated");
        if (!std::holds_alternative<double>(args[0]))
            throw std::runtime_error("drbg.generate(): argument must be a number");
        size_t len = (size_t)std::get<double>(args[0]);
        std::vector<uint8_t> buf(len);
        if (drbg_generate(&st->ctx, buf.data(), len, nullptr, 0) != 0)
            throw std::runtime_error("drbg.generate(): failed (needs reseed?)");
        return bytesToArray(buf.data(), len);
    }
};

struct DrbgReseedFn : NinCallable {
    std::shared_ptr<DrbgState> st;
    DrbgReseedFn(std::shared_ptr<DrbgState> s) : st(s) {}
    int arity() override { return 0; }
    std::string name() override { return "reseed"; }
    Value call(std::vector<Value>) override {
        if (!st->valid)
            throw std::runtime_error("drbg.reseed(): not instantiated");
        uint8_t entropy[64];
        gather_hardware_entropy(entropy, sizeof(entropy));
        drbg_reseed(&st->ctx, entropy, sizeof(entropy), nullptr, 0);
        memset(entropy, 0, sizeof(entropy));
        return std::monostate{};
    }
};

struct DrbgUninstantiateFn : NinCallable {
    std::shared_ptr<DrbgState> st;
    DrbgUninstantiateFn(std::shared_ptr<DrbgState> s) : st(s) {}
    int arity() override { return 0; }
    std::string name() override { return "uninstantiate"; }
    Value call(std::vector<Value>) override {
        if (st->valid) { drbg_uninstantiate(&st->ctx); st->valid = false; }
        return std::monostate{};
    }
};

struct CreateDrbgFn : NinCallable {
    int arity() override { return 1; }
    std::string name() override { return "createDrbg"; }
    Value call(std::vector<Value> args) override {
        auto st = std::make_shared<DrbgState>();
        uint8_t ent[64], nonce[32];
        gather_hardware_entropy(ent, sizeof(ent));
        gather_hardware_entropy(nonce, sizeof(nonce));

        const uint8_t *pers = nullptr;
        size_t pers_len = 0;
        std::string pers_buf;
        
        if (std::holds_alternative<std::string>(args[0])) {
            pers_buf = std::get<std::string>(args[0]);
            if (!pers_buf.empty()) {
                pers = (const uint8_t *)pers_buf.data();
                pers_len = pers_buf.size();
            }
        }

        drbg_instantiate(&st->ctx, ent, sizeof(ent),
                         nonce, sizeof(nonce), pers, pers_len);
        st->valid = true;
        memset(ent, 0, sizeof(ent));
        memset(nonce, 0, sizeof(nonce));

        auto native = std::make_shared<NinNative>();
        native->typeName = "DrbgContext";
        native->data = st;
        native->getField = [st](const std::string &f) -> Value {
            if (f == "generate")      return std::make_shared<DrbgGenerateFn>(st);
            if (f == "reseed")        return std::make_shared<DrbgReseedFn>(st);
            if (f == "uninstantiate") return std::make_shared<DrbgUninstantiateFn>(st);
            return std::monostate{};
        };
        return native;
    }
};

struct PoolState {
    entropy_pool_t pool;
    PoolState() { entropy_pool_init(&pool); }
    ~PoolState() { memset(&pool, 0, sizeof(pool)); }
};

struct PoolAddFn : NinCallable {
    std::shared_ptr<PoolState> st;
    PoolAddFn(std::shared_ptr<PoolState> s) : st(s) {}
    int arity() override { return 1; }
    std::string name() override { return "add"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<std::string>(args[0]))
            throw std::runtime_error("pool.add(): argument must be a string");
        const auto &s = std::get<std::string>(args[0]);
        entropy_pool_add(&st->pool, (const uint8_t *)s.data(), s.size());
        return std::monostate{};
    }
};

struct PoolExtractFn : NinCallable {
    std::shared_ptr<PoolState> st;
    PoolExtractFn(std::shared_ptr<PoolState> s) : st(s) {}
    int arity() override { return 1; }
    std::string name() override { return "extract"; }
    Value call(std::vector<Value> args) override {
        if (!std::holds_alternative<double>(args[0]))
            throw std::runtime_error("pool.extract(): argument must be a number");
        size_t len = (size_t)std::get<double>(args[0]);
        std::vector<uint8_t> buf(len);
        entropy_pool_extract(&st->pool, buf.data(), len);
        return bytesToArray(buf.data(), len);
    }
};

struct CreatePoolFn : NinCallable {
    int arity() override { return 0; }
    std::string name() override { return "createEntropyPool"; }
    Value call(std::vector<Value>) override {
        auto st = std::make_shared<PoolState>();
        auto native = std::make_shared<NinNative>();
        native->typeName = "EntropyPool";
        native->data = st;
        native->getField = [st](const std::string &f) -> Value {
            if (f == "add")     return std::make_shared<PoolAddFn>(st);
            if (f == "extract") return std::make_shared<PoolExtractFn>(st);
            return std::monostate{};
        };
        return native;
    }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
    (*out)["sha256"]        = std::make_shared<Sha256Fn>();
    (*out)["sha256Bytes"]   = std::make_shared<Sha256BytesFn>();
    (*out)["createSha256"]  = std::make_shared<CreateSha256Fn>();

    (*out)["randomBytes"]   = std::make_shared<RandomBytesFn>();
    (*out)["randomU32"]     = std::make_shared<RandomU32Fn>();
    (*out)["randomU64"]     = std::make_shared<RandomU64Fn>();
    (*out)["randomString"]  = std::make_shared<RandomStringFn>();
    (*out)["random"]        = std::make_shared<RandomFn>();
    (*out)["randrange"]    = std::make_shared<RandRangeFn>();

    (*out)["gatherHardwareEntropy"] = std::make_shared<GatherHwEntropyFn>();
    (*out)["hasRdrand"]     = std::make_shared<HasRdrandFn>();
    (*out)["hasRdseed"]     = std::make_shared<HasRdseedFn>();

    (*out)["createDrbg"]        = std::make_shared<CreateDrbgFn>();
    (*out)["createEntropyPool"] = std::make_shared<CreatePoolFn>();
}
