#include <iostream>
#include "tests/test_template.h"
#include "tests/test_process.h"

int main() {
    std::cout << "-------------------------------- Test Template --------------------------------" << std::endl;
//    TestTemplate();
    std::cout << "-------------------------------- Test Fork --------------------------------" << std::endl;
//    TestFork();
    std::cout << "-------------------------------- Test multi-Fork --------------------------------" << std::endl;
//    TestMultiFork();
    std::cout << "-------------------------------- Test producer-consumer-model --------------------------------" << std::endl;
    TestSharedMem();
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    return 0;
}
