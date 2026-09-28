#include "carrot_module.h"
#include <algorithm>
#include <cctype>
#include <curl/curl.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct SessionData {
  CURL *curl;
  std::unordered_map<std::string, std::string> headers;

  SessionData() {
    curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
    }
  }

  ~SessionData() {
    if (curl) {
      curl_easy_cleanup(curl);
    }
  }
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  size_t totalSize = size * nmemb;
  auto *str = static_cast<std::string *>(userp);
  str->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

static size_t HeaderCallback(char *buffer, size_t size, size_t nitems,
                             void *userdata) {
  size_t totalSize = size * nitems;
  auto *headersMap =
      static_cast<std::unordered_map<std::string, std::string> *>(userdata);
  std::string line(buffer, totalSize);

  size_t colonPos = line.find(':');
  if (colonPos != std::string::npos) {
    std::string key = line.substr(0, colonPos);
    std::string val = line.substr(colonPos + 1);

    key.erase(0, key.find_first_not_of(" \t\r\n"));
    key.erase(key.find_last_not_of(" \t\r\n") + 1);
    val.erase(0, val.find_first_not_of(" \t\r\n"));
    val.erase(val.find_last_not_of(" \t\r\n") + 1);

    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    (*headersMap)[key] = val;
  }
  return totalSize;
}

static std::string
buildQueryString(CURL *curl, const std::string &url,
                 const std::unordered_map<std::string, std::string> &params) {
  if (params.empty())
    return url;

  std::ostringstream ss;
  ss << url << (url.find('?') == std::string::npos ? '?' : '&');

  bool first = true;
  for (const auto &kv : params) {
    if (!first)
      ss << '&';
    char *encKey = curl_easy_escape(curl, kv.first.c_str(),
                                    static_cast<int>(kv.first.length()));
    char *encVal = curl_easy_escape(curl, kv.second.c_str(),
                                    static_cast<int>(kv.second.length()));
    if (encKey && encVal)
      ss << encKey << '=' << encVal;
    if (encKey)
      curl_free(encKey);
    if (encVal)
      curl_free(encVal);
    first = false;
  }
  return ss.str();
}

static Value performRequest(std::string method, std::vector<Value> &args,
                            std::unordered_map<std::string, Value> &kwargs,
                            SessionData *session = nullptr);

struct RaiseForStatusFn : NinCallable {
  std::weak_ptr<NinInstance> weakResp;

  explicit RaiseForStatusFn(std::weak_ptr<NinInstance> resp)
      : weakResp(std::move(resp)) {}

  int arity() override { return 0; }
  std::string name() override { return "raise_for_status"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }

  Value callWithKwargs(std::vector<Value>,
                       std::unordered_map<std::string, Value> kwargs) override {
    if (!kwargs.empty())
      throw std::runtime_error(
          "'raise_for_status' does not accept keyword arguments.");

    auto inst = weakResp.lock();
    if (!inst)
      throw std::runtime_error("Response instance no longer exists.");

    double status = 0;
    if (inst->fields.count("status_code") &&
        std::holds_alternative<double>(inst->fields["status_code"])) {
      status = std::get<double>(inst->fields["status_code"]);
    }
    if (status >= 400) {
      std::string urlStr = "URL";
      if (inst->fields.count("url") &&
          std::holds_alternative<std::string>(inst->fields["url"])) {
        urlStr = std::get<std::string>(inst->fields["url"]);
      }
      throw std::runtime_error("HTTPError: Status " +
                               std::to_string(static_cast<int>(status)) +
                               " for " + urlStr);
    }
    return std::monostate{};
  }
};

struct GetFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit GetFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "get"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("GET", args, kwargs, session.get());
  }
};

struct PostFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit PostFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 0; }
  bool isVariadic() override { return true; }
  std::string name() override { return "post"; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(std::move(args), {});
  }

  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("POST", args, kwargs, session.get());
  }
};

struct PutFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit PutFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "put"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("PUT", args, kwargs, session.get());
  }
};

struct DeleteFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit DeleteFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "delete"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("DELETE", args, kwargs, session.get());
  }
};

struct PatchFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit PatchFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "patch"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("PATCH", args, kwargs, session.get());
  }
};

struct HeadFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit HeadFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "head"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("HEAD", args, kwargs, session.get());
  }
};

struct OptionsFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit OptionsFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "options"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    return performRequest("OPTIONS", args, kwargs, session.get());
  }
};

struct RequestFn : NinCallable {
  std::shared_ptr<SessionData> session;
  explicit RequestFn(std::shared_ptr<SessionData> s = nullptr)
      : session(std::move(s)) {}

  int arity() override { return 1; }
  std::string name() override { return "request"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    std::string method;
    if (!args.empty() && std::holds_alternative<std::string>(args[0])) {
      method = std::get<std::string>(args[0]);
      args.erase(args.begin());
    } else if (kwargs.count("method") &&
               std::holds_alternative<std::string>(kwargs["method"])) {
      method = std::get<std::string>(kwargs["method"]);
    } else {
      throw std::runtime_error("request expects an HTTP method string.");
    }
    return performRequest(method, args, kwargs, session.get());
  }
};

struct SessionConstructorFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "Session"; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(args, {});
  }
  Value callWithKwargs(std::vector<Value>,
                       std::unordered_map<std::string, Value> kwargs) override {
    if (!kwargs.empty())
      throw std::runtime_error("'Session' does not accept keyword arguments.");

    auto sessionData = std::make_shared<SessionData>();
    auto klass = std::make_shared<NinClass>("Session");
    auto inst = std::make_shared<NinInstance>(klass);

    inst->fields["get"] = std::make_shared<GetFn>(sessionData);
    inst->fields["post"] = std::make_shared<PostFn>(sessionData);
    inst->fields["put"] = std::make_shared<PutFn>(sessionData);
    inst->fields["delete"] = std::make_shared<DeleteFn>(sessionData);
    inst->fields["patch"] = std::make_shared<PatchFn>(sessionData);
    inst->fields["head"] = std::make_shared<HeadFn>(sessionData);
    inst->fields["options"] = std::make_shared<OptionsFn>(sessionData);
    inst->fields["request"] = std::make_shared<RequestFn>(sessionData);

    return inst;
  }
};

static Value performRequest(std::string method, std::vector<Value> &args,
                            std::unordered_map<std::string, Value> &kwargs,
                            SessionData *session) {
  std::string url;

  if (!args.empty() && std::holds_alternative<std::string>(args[0])) {
    url = std::get<std::string>(args[0]);
  } else if (kwargs.count("url") &&
             std::holds_alternative<std::string>(kwargs["url"])) {
    url = std::get<std::string>(kwargs["url"]);
  } else {
    throw std::runtime_error("request requires a valid URL string.");
  }

  CURL *curl = session ? session->curl : curl_easy_init();
  if (!curl)
    throw std::runtime_error("Failed to initialize libcurl handle.");

  struct CurlGuard {
    CURL *c;
    bool isSession;
    ~CurlGuard() {
      if (c && !isSession)
        curl_easy_cleanup(c);
    }
  } curlGuard{curl, session != nullptr};

  if (session) {
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
  }

  struct curl_slist *chunk = nullptr;
  struct SlistGuard {
    struct curl_slist **list;
    ~SlistGuard() {
      if (list && *list)
        curl_slist_free_all(*list);
    }
  } slistGuard{&chunk};

  std::string responseBody;
  std::unordered_map<std::string, std::string> responseHeaders;

  std::unordered_map<std::string, std::string> queryParams;
  if (kwargs.count("params")) {
    Value pVal = kwargs["params"];
    if (std::holds_alternative<std::shared_ptr<NinInstance>>(pVal)) {
      auto inst = std::get<std::shared_ptr<NinInstance>>(pVal);
      for (const auto &kv : inst->fields)
        queryParams[kv.first] = valueToString(kv.second);
    }
  }
  url = buildQueryString(curl, url, queryParams);
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  std::string upperMethod = method;
  std::transform(upperMethod.begin(), upperMethod.end(), upperMethod.begin(),
                 ::toupper);

  if (upperMethod == "GET") {
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
  } else if (upperMethod == "POST") {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
  } else if (upperMethod == "HEAD") {
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
  } else if (upperMethod == "PUT" || upperMethod == "DELETE" ||
             upperMethod == "PATCH" || upperMethod == "OPTIONS") {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, upperMethod.c_str());
  }

  if (session) {
    for (const auto &kv : session->headers) {
      std::string headerLine = kv.first + ": " + kv.second;
      chunk = curl_slist_append(chunk, headerLine.c_str());
    }
  }

  if (kwargs.count("headers")) {
    Value hVal = kwargs["headers"];
    if (std::holds_alternative<std::shared_ptr<NinInstance>>(hVal)) {
      auto inst = std::get<std::shared_ptr<NinInstance>>(hVal);
      for (const auto &kv : inst->fields) {
        std::string headerLine = kv.first + ": " + valueToString(kv.second);
        chunk = curl_slist_append(chunk, headerLine.c_str());
      }
    }
  }

  std::string postData;
  if (kwargs.count("json")) {
    postData = valueToString(kwargs["json"]);
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
  } else if (kwargs.count("data")) {
    postData = valueToString(kwargs["data"]);
  }

  if (!postData.empty()) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                     static_cast<long>(postData.length()));
  }

  if (kwargs.count("timeout") &&
      std::holds_alternative<double>(kwargs["timeout"])) {
    double sec = std::get<double>(kwargs["timeout"]);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(sec * 1000.0));
  }

  long allowRedirects = 1L;
  if (kwargs.count("allow_redirects") &&
      std::holds_alternative<bool>(kwargs["allow_redirects"])) {
    allowRedirects = std::get<bool>(kwargs["allow_redirects"]) ? 1L : 0L;
  }
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, allowRedirects);

  if (kwargs.count("verify") &&
      std::holds_alternative<bool>(kwargs["verify"])) {
    bool verify = std::get<bool>(kwargs["verify"]);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verify ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verify ? 2L : 0L);
  }

  if (chunk)
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    throw std::runtime_error("cURL Request Failed: " +
                             std::string(curl_easy_strerror(res)));

  long statusCode = 0;
  double totalTime = 0.0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &totalTime);

  static std::shared_ptr<NinClass> respClass =
      std::make_shared<NinClass>("Response");
  auto responseInst = std::make_shared<NinInstance>(respClass);
  responseInst->fields["status_code"] = static_cast<double>(statusCode);
  responseInst->fields["text"] = responseBody;
  responseInst->fields["url"] = url;
  responseInst->fields["ok"] = (statusCode >= 200 && statusCode < 400);
  responseInst->fields["elapsed"] = totalTime;

  std::weak_ptr<NinInstance> weakResp = responseInst;
  responseInst->fields["raise_for_status"] =
      std::make_shared<RaiseForStatusFn>(weakResp);

  std::vector<Value> byteElements;
  byteElements.reserve(responseBody.size());
  for (unsigned char c : responseBody)
    byteElements.push_back(static_cast<uint8_t>(c));
  responseInst->fields["content"] = std::make_shared<NinArray>(byteElements);

  auto headersClass = std::make_shared<NinClass>("Headers");
  auto headersInst = std::make_shared<NinInstance>(headersClass);
  for (const auto &kv : responseHeaders)
    headersInst->fields[kv.first] = kv.second;
  responseInst->fields["headers"] = headersInst;

  return responseInst;
}

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  curl_global_init(CURL_GLOBAL_ALL);

  (*out)["request"] = std::make_shared<RequestFn>();
  (*out)["get"] = std::make_shared<GetFn>();
  (*out)["post"] = std::make_shared<PostFn>();
  (*out)["put"] = std::make_shared<PutFn>();
  (*out)["delete"] = std::make_shared<DeleteFn>();
  (*out)["patch"] = std::make_shared<PatchFn>();
  (*out)["head"] = std::make_shared<HeadFn>();
  (*out)["options"] = std::make_shared<OptionsFn>();

  (*out)["Session"] = std::make_shared<SessionConstructorFn>();
}
