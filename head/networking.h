#pragma once
#include <cstddef>
#include <string>
#include <curl/curl.h>

static std::size_t WriteCallback(void* contents, std::size_t size, std::size_t nmemb, void* userp) {
  size_t totalSize = size * nmemb;
  std::string* str = static_cast<std::string*>(userp);
  str->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

std::string doGetRequest(const std::string& url);
