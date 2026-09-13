#include "carrot_module.h"
extern "C" {
#include "discord-rpc/include/discord_rpc.h"
}

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

static std::atomic<bool> g_connected{false};
static std::mutex g_errorMutex;
static std::string g_lastError;

static void onReady(const DiscordUser *) { g_connected = true; }

static void onDisconnected(int, const char *message) {
  g_connected = false;
  std::lock_guard<std::mutex> lock(g_errorMutex);
  g_lastError = message ? message : "";
}

static void onErrored(int, const char *message) {
  std::lock_guard<std::mutex> lock(g_errorMutex);
  g_lastError = message ? message : "";
}

struct DiscordSession {
  std::atomic<bool> running{false};
  std::thread loopThread;

  explicit DiscordSession(const std::string &clientId) {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = onReady;
    handlers.disconnected = onDisconnected;
    handlers.errored = onErrored;

    Discord_Initialize(clientId.c_str(), &handlers, 1, nullptr);

    running = true;
    loopThread = std::thread([this]() {
      while (running) {
        Discord_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }
    });
  }

  ~DiscordSession() { stop(); }

  void stop() {
    if (running) {
      running = false;
      if (loopThread.joinable())
        loopThread.join();
      Discord_ClearPresence();
      Discord_Shutdown();
    }
  }
};

struct UpdatePresenceFn : NinCallable {
  std::shared_ptr<DiscordSession> session;
  explicit UpdatePresenceFn(std::shared_ptr<DiscordSession> s)
      : session(std::move(s)) {}

  int arity() override { return 0; }
  std::string name() override { return "updatePresence"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value>) override { return callWithKwargs({}, {}); }

  Value callWithKwargs(std::vector<Value>,
                       std::unordered_map<std::string, Value> kwargs) override {
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));

    std::string state, details, largeImageKey, largeImageText, smallImageKey,
        smallImageText, partyId, matchSecret, joinSecret, spectateSecret, button1_label, button2_label, button1_url, button2_url;

    auto str = [&](const char *key, std::string &store) -> const char * {
      auto it = kwargs.find(key);
      if (it != kwargs.end() &&
          std::holds_alternative<std::string>(it->second)) {
        store = std::get<std::string>(it->second);
        return store.c_str();
      }
      return nullptr;
    };

    presence.state = str("state", state);
    presence.details = str("details", details);
    presence.largeImageKey = str("largeImageKey", largeImageKey);
    presence.largeImageText = str("largeImageText", largeImageText);
    presence.smallImageKey = str("smallImageKey", smallImageKey);
    presence.smallImageText = str("smallImageText", smallImageText);
    presence.partyId = str("partyId", partyId);
    presence.matchSecret = str("matchSecret", matchSecret);
    presence.joinSecret = str("joinSecret", joinSecret);
    presence.spectateSecret = str("spectateSecret", spectateSecret);

    presence.button1_label = str("button1_label", button1_label);
    presence.button2_label = str("button2_label", button2_label);
    presence.button1_url = str("button1_url", button1_url);
    presence.button2_url = str("button2_url", button2_url);

    auto num = [&](const char *key) -> long long {
      auto it = kwargs.find(key);
      if (it != kwargs.end() && std::holds_alternative<double>(it->second))
        return (long long)std::get<double>(it->second);
      return 0;
    };

    presence.startTimestamp = num("startTimestamp");
    presence.endTimestamp = num("endTimestamp");
    presence.partySize = (int)num("partySize");
    presence.partyMax = (int)num("partyMax");
    presence.instance = (int8_t)num("instance");

    Discord_UpdatePresence(&presence);
    return std::monostate{};
  }
};

struct ClearPresenceFn : NinCallable {
  std::shared_ptr<DiscordSession> session;
  explicit ClearPresenceFn(std::shared_ptr<DiscordSession> s)
      : session(std::move(s)) {}
  int arity() override { return 0; }
  std::string name() override { return "clearPresence"; }
  Value call(std::vector<Value>) override {
    Discord_ClearPresence();
    return std::monostate{};
  }
};

struct StopFn : NinCallable {
  std::shared_ptr<DiscordSession> session;
  explicit StopFn(std::shared_ptr<DiscordSession> s) : session(std::move(s)) {}
  int arity() override { return 0; }
  std::string name() override { return "stop"; }
  Value call(std::vector<Value>) override {
    session->stop();
    return std::monostate{};
  }
};

struct IsRunningFn : NinCallable {
  std::shared_ptr<DiscordSession> session;
  explicit IsRunningFn(std::shared_ptr<DiscordSession> s)
      : session(std::move(s)) {}
  int arity() override { return 0; }
  std::string name() override { return "isRunning"; }
  Value call(std::vector<Value>) override { return (bool)session->running; }
};

struct IsConnectedFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "isConnected"; }
  Value call(std::vector<Value>) override { return (bool)g_connected; }
};

struct CreateDiscordRPCFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "createDiscordRPC"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error(
          "createDiscordRPC(): argument must be a string client ID.");

    auto session =
        std::make_shared<DiscordSession>(std::get<std::string>(args[0]));

    auto klass = std::make_shared<NinClass>("DiscordRPC");
    auto inst = std::make_shared<NinInstance>(klass);

    inst->fields["updatePresence"] =
        std::make_shared<UpdatePresenceFn>(session);
    inst->fields["clearPresence"] = std::make_shared<ClearPresenceFn>(session);
    inst->fields["stop"] = std::make_shared<StopFn>(session);
    inst->fields["isRunning"] = std::make_shared<IsRunningFn>(session);
    inst->fields["isConnected"] = std::make_shared<IsConnectedFn>();

    return inst;
  }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["createDiscordRPC"] = std::make_shared<CreateDiscordRPCFn>();
}
