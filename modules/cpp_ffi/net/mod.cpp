#include "carrot_module.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

static std::size_t WriteCallback(void *contents, std::size_t size,
                                 std::size_t nmemb, void *userp) {
  size_t totalSize = size * nmemb;
  std::string *str = static_cast<std::string *>(userp);
  str->append(static_cast<char *>(contents), totalSize);
  return totalSize;
}

std::string doGetRequest(const std::string &url) {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
    }

    curl_easy_cleanup(curl);
  }

  return readBuffer;
}

struct GETFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "get"; }
  Value call(std::vector<Value> args) override {
    if (!(std::holds_alternative<std::string>(args[0])))
      throw std::runtime_error("get(): argument must be a string.");
    std::string url = std::get<std::string>(args[0]);

    return doGetRequest(url);
  }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["get"] = std::make_shared<GETFn>();
}
