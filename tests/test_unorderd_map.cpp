#include "tests/test_unorderd_map.h"

namespace hash_map {
void TestHashMap() {
    auto registry = Registry::Instance();
    registry.Reg({kCPU, kConv, format0, kBuiltin}, 1);
    registry.Reg({kCPU, kConv, format2, kCustom1}, 2);
    registry.Reg({kCPU, kConv, format0, kCustom2}, 3);
    registry.Reg({kCPU, kConv, format1, kCustom3}, 4);
    printf("Size %ld\r\n", registry.Size());
    registry.PrintBucket({kCPU, kConv, format0, kBuiltin});
}
} // namespace hash_map
