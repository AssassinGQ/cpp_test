//
// Created by cafebabe on 3/28/23.
//

#include "smart_shm.h"
#include <cassert>

int main() {
    // Test case 1: Create shared memory segment with same key twice
    SmartSHM shm1(1024);
    assert(shm1.Init(1234) == true);
    assert(shm1.Init(1234) == false);

    // Test case 2: Access shared memory with wrong type
    struct Data {
        int a;
        char b[10];
    };
    SmartSHM shm2(sizeof(Data));
    assert(shm2.Init(5678) == true);
    Data *data = shm2.Get<Data>();
    assert(data != nullptr);
    int *int_ptr = shm2.Get<int>();
    assert(int_ptr == nullptr);

    // Test case 3: Access shared memory with larger size than allocated
    SmartSHM shm3(10);
    assert(shm3.Init(9012) == true);
    char *str = shm3.Get<char>();
    assert(str != nullptr);
    strcpy(str, "Hello, world!");
    assert(strlen(str) == 12);
    return 0;
}
