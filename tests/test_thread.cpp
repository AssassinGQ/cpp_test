#include "tests/test_thread.h"
#include <iostream>
#include <unistd.h>

namespace {
    void Task1() {
        std::cout << "I'm son process, exec ls:" << std::endl;
        sleep(2);
        //一旦执行完execlp函数,立刻会去执行新的程序,新的程序会替换当前程序 代码段 数据段 堆 栈
        std::cout << "I'm son process, finish exec ls." << std::endl;  //不会被打印,因为调用完execlp函数,当前进程被ls进程换血
        sleep(1);
    }

    void Task2() {
        std::cout << "I'm son process, exec pwd:" << std::endl;
        sleep(1);
        //一旦执行完execlp函数,立刻会去执行新的程序,新的程序会替换当前程序 代码段 数据段 堆 栈
        std::cout << "I'm son process, finish exec ls." << std::endl;  //不会被打印,因为调用完execlp函数,当前进程被ls进程换血
        sleep(1);
    }
}
