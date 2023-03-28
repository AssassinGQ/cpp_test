#include "tests/test_process.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "common/smart_shm.h"

namespace {
    void Task1() {
        std::cout << "I'm son process, exec ls:" << std::endl;
        sleep(2);
        execlp("/bin/ls", "ls", NULL);
        //一旦执行完execlp函数,立刻会去执行新的程序,新的程序会替换当前程序 代码段 数据段 堆 栈
        std::cout << "I'm son process, finish exec ls." << std::endl;  //不会被打印,因为调用完execlp函数,当前进程被ls进程换血
        sleep(1);
    }

    void Task2() {
        std::cout << "I'm son process, exec pwd:" << std::endl;
        sleep(1);
        execlp("pwd", "pwd", NULL);
        //一旦执行完execlp函数,立刻会去执行新的程序,新的程序会替换当前程序 代码段 数据段 堆 栈
        std::cout << "I'm son process, finish exec ls." << std::endl;  //不会被打印,因为调用完execlp函数,当前进程被ls进程换血
        sleep(1);
    }
}

void TestFork() {
    int pid = fork();
    if (pid < 0) {
        std::cerr << "Fork process failed" << std::endl;
    } else if (pid == 0) {
        Task1();
    } else {
        std::cout << "I'm father process, waiting for son(pid:" << pid << ") to finish." << std::endl;
        auto ret = wait(nullptr);
        std::cout << "I'm father process, son(pid:" << pid << ") finished. wait ret : " << ret << std::endl;
    }
}

void TestMultiFork() {
    int pid1 = fork();
    if (pid1 < 0) {
        std::cerr << "Fork process1 failed" << std::endl;
        return;
    }
    if (pid1 == 0) {
        Task1();
        return;
    }

    int pid2 = fork();
    if (pid2 < 0) {
        std::cerr << "Fork process2 failed" << std::endl;
        return;
    }
    if (pid2 == 0) {
        Task2();
        return;
    }

    std::cout << "I'm father process, waiting for sons(pid:" << pid1 << ", " << pid2 << ") to finish." << std::endl;
    auto ret = wait(NULL);
    std::cout << "I'm father process, son(pid:" << ret << ") finished" << std::endl;
    ret = wait(NULL);
    std::cout << "I'm father process, son(pid:" << ret << ") finished" << std::endl;
}

namespace {
    constexpr int kPipelineSize = 10;
    class Pipeline {
    public:
        bool Init() {
            buffer_ = new SmartSHM(kPipelineSize);
            auto ret = buffer_->Init(shm_key_buffer);
            if (!ret) {
                std::cerr << "Init `buffer` shm failed." << std::endl;
                return false;
            }
            in_ = new SmartSHM(sizeof(int));
            ret = in_->Init(shm_key_in);
            if (!ret) {
                std::cerr << "Init `in` shm failed." << std::endl;
                return false;
            }
            out_ = new SmartSHM(sizeof(int));
            ret = out_->Init(shm_key_out);
            if (!ret) {
                std::cerr << "Init `out` shm failed." << std::endl;
                return false;
            }
            in_->Get<int>()[0] = 0;
            out_->Get<int>()[0] = 0;
            return true;
        }

        ~Pipeline() {
            delete buffer_;
            delete in_;
            delete out_;
        }
    public:
        SmartSHM *buffer_{nullptr};
        SmartSHM *in_{nullptr};
        SmartSHM *out_{nullptr};
    private:
        static constexpr key_t shm_key_buffer = 1;
        static constexpr key_t shm_key_in = 2;
        static constexpr key_t shm_key_out = 3;
    };

    void ProducerTask(const Pipeline &pipeline, const SmartSHM &interrupt) {
        std::cout << "Start Produce Task." << std::endl;
        auto *in = pipeline.in_->Get<int>();
        auto *out = pipeline.out_->Get<int>();
        auto *buffer = pipeline.buffer_->Get<char>();
        char produce = 'a';
        while(!interrupt.Get<bool>()[0]) {
            buffer[*in] = produce++;
            *in = (*in + 1) % kPipelineSize;
            sleep(1);
            while ((*in + 1) % kPipelineSize == *out && !interrupt.Get<bool>()[0]);
        }
    }

    void ConsumeTask(const Pipeline &pipeline, const SmartSHM &interrupt) {
        std::cout << "Start Consume Task." << std::endl;
        auto *in = pipeline.in_->Get<int>();
        auto *out = pipeline.out_->Get<int>();
        auto *buffer = pipeline.buffer_->Get<char>();
        while(!interrupt.Get<bool>()[0]) {
            while (*in == *out && !interrupt.Get<bool>()[0]) {
                sleep(1);
            }
            if (*in != *out) {
                auto produce = buffer[*out];
                *out = (*out + 1) % kPipelineSize;
                std::cout << "Consume " << produce << std::endl;
            }
        }
    }
}

void TestSharedMem() {
    Pipeline pipeline;
    auto ret = pipeline.Init();
    if (!ret) {
        std::cerr << "Init shm pipeline failed." << std::endl;
        return;
    }
    SmartSHM interrupt(sizeof(bool));
    ret = interrupt.Init(101);
    if (!ret) {
        std::cerr << "Init shm interrupt failed." << std::endl;
        return;
    }
    interrupt.Get<bool>()[0] = false;

    int producer_pid = fork();
    if (producer_pid < 0) {
        std::cerr << "Fork producer process failed" << std::endl;
        return;
    }
    if (producer_pid == 0) {
        ProducerTask(pipeline, interrupt);
        return;
    } else {
        std::cout << "Start producer process: " << producer_pid << std::endl;
    }

    int consumer_pid = fork();
    if (consumer_pid < 0) {
        std::cerr << "Fork consumer process failed" << std::endl;
        interrupt.Get<bool>()[0] = true;
        wait(nullptr);
        return;
    }
    if (consumer_pid == 0) {
        ConsumeTask(pipeline, interrupt);
        return;
    } else {
        std::cout << "Start consumer process: " << consumer_pid << std::endl;
    }

    int flag;
    while(true) {
        std::cout << "Input 3 to stop current test: " << std::endl;
        std::cin >> flag;
        if (flag == 3) {
            interrupt.Get<bool>()[0] = true;
            auto pid = wait(nullptr);
            std::cout << "Subprocess stoped: " << pid << std::endl;
            pid = wait(nullptr);
            std::cout << "Subprocess stoped: " << pid << std::endl;
            break;
        } else {
            std::cout << "Invalid input: " << flag << std::endl;
        }
    }
}
