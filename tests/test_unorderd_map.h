#include <iostream>

#ifndef CPP_TEST_TEST_UNORDERD_MAP_H
#define CPP_TEST_TEST_UNORDERD_MAP_H

#include <unordered_map>

namespace hash_map {
// provider
constexpr char kBuiltin[] = "Builtin";
constexpr char kCustom1[] = "Custom1";
constexpr char kCustom2[] = "Custom2";
constexpr char kCustom3[] = "Custom3";

// backend
constexpr char kCPU[] = "CPU";
constexpr char kGPU[] = "GPU";
constexpr char kNPU[] = "NPU";

// format
constexpr size_t format0 = 0;
constexpr size_t format1 = 1;
constexpr size_t format2 = 2;

// type
constexpr char kConv[] = "Conv";
constexpr char kMatmul[] = "MM";
constexpr char kAdd[] = "Add";

struct Key {
  std::string backend{};
  std::string type{};
  size_t format{format0};
  std::string provider{kBuiltin};

  size_t Hash() const {
    return std::hash<std::string>{}(backend) ^ std::hash<std::string>{}(type) ^
           std::hash<size_t>{}(format);
  }

  bool operator==(const Key &other) const {
    return backend == other.backend && type == other.type &&
           format == other.format && provider == other.provider;
  }
};

struct KeyHash {
  std::size_t operator()(const Key &key) const { return key.Hash(); }
};

class Registry {
public:
  static Registry &Instance() {
    static Registry instance;
    return instance;
  }

  void Reg(const Key &key, int value) { kernels_[key] = value; }

  size_t Size() const { return kernels_.size(); }

  void PrintBucket(const Key &key) {
    auto id = kernels_.bucket(key);
    for (auto iter = kernels_.begin(id); iter != kernels_.end(id); iter++) {
      printf("Value %d \r\n", iter->second);
    }
  };

private:
  Registry() = default;

private:
  std::unordered_map<Key, int, KeyHash> kernels_;
};

void TestHashMap();
} // namespace hash_map

#endif
