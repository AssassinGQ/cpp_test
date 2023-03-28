//
// Created by cafebabe on 3/16/23.
//

#ifndef CPP_TEST_SMARTSHM_H
#define CPP_TEST_SMARTSHM_H

#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <iostream>
#include <cstring>

class SmartSHM {
public:
    explicit SmartSHM(int size) : size_(size) {}
    virtual ~SmartSHM() {
        Detach();
        if (shmid_ > 0) {
            shmctl(shmid_, IPC_RMID, nullptr);
            shmid_ = -1;
        }
    }

    bool Init(int key) {
        if (size_ <= 0) {
            std::cerr << "Size should be larger than zero, got: " << size_ << std::endl;
            return false;
        }
        if (shmid_ > 0) {
            std::cerr << "Already inited." << std::endl;
            return false;
        }
        shmid_ = shmget(key, size_, IPC_CREAT | IPC_EXCL | 0664);
        if (shmid_ == -1) {
            std::cerr << "Create shm failed" << std::endl;
            return false;
        }
        ptr_ = shmat(shmid_, nullptr, 0);
        if (ptr_ == nullptr) {
            std::cerr << "Attach shm failed" << std::endl;
            return false;
        }
        return true;
    }

    template<class T>
    T *Get() const {
        if (ptr_ == nullptr) {
            std::cerr << "Shared memory not initialized." << std::endl;
            return nullptr;
        }
        if (sizeof(T) > size_) {
            std::cerr << "Requested type size exceeds shared memory size." << std::endl;
            return nullptr;
        }
        return reinterpret_cast<T *>(ptr_);
    }

    int GetSize() const { return size_; }

private:
    void Detach() {
        if (ptr_ != nullptr) {
            shmdt(ptr_);
            ptr_ = nullptr;
        }
    }

    int size_{0};
    int shmid_{0};
    void *ptr_{nullptr};
};

#endif //CPP_TEST_SMARTSHM_H
