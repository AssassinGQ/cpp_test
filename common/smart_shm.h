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

// This header file defines a class for managing shared memory segments.
// It provides a convenient way to create, attach, and detach shared memory
// segments, as well as to access the data stored in them.
class SmartSHM {
public:
    // Constructor that takes the size of the shared memory segment as an argument.
    explicit SmartSHM(int size) : size_(size) {}

    // Destructor that detaches the shared memory segment and removes it from the system.
    virtual ~SmartSHM() {
        Detach();
        if (shmid_ > 0) {
            shmctl(shmid_, IPC_RMID, nullptr);
            shmid_ = -1;
        }
    }

    // Initializes the shared memory segment with the given key.
    // Returns true if successful, false otherwise.
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
            if (errno == EEXIST) {
                shmid_ = shmget(key, size_, 0664);
                if (shmid_ == -1) {
                    std::cerr << "Open shm failed" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Create shm failed" << std::endl;
                return false;
            }
        }
        ptr_ = shmat(shmid_, nullptr, 0);
        if (ptr_ == nullptr) {
            std::cerr << "Attach shm failed" << std::endl;
            return false;
        }
        return true;
    }

    // Returns a pointer to the data stored in the shared memory segment.
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
    // Detaches the shared memory segment from the current process.
    void Detach() {
        if (ptr_ != nullptr) {
            shmdt(ptr_);
            ptr_ = nullptr;
        }
    }

private:
    int size_;  // The size of the shared memory segment.
    int shmid_ = -1;  // The ID of the shared memory segment.
    void* ptr_ = nullptr;  // A pointer to the data stored in the shared memory segment.
};

#endif //CPP_TEST_SMARTSHM_H
