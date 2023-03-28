#include <iostream>

#ifndef CPP_TEST_TEST_TEMPLATE_H
#define CPP_TEST_TEST_TEMPLATE_H

template <typename T>
T AutoInc(const T &a) {
    return a + 0.5;
}

void TestTemplate();

#endif
