#include "common/smart_shm.h"
#include <gtest/gtest.h>

TEST(SmartSHMTest, InitAndGet) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));

  // Test Get() method with a simple integer.
  int* data = shm.Get<int>();
  ASSERT_NE(data, nullptr);
  *data = 42;
  ASSERT_EQ(*data, 42);

  // Test Get() method with a struct.
  struct MyStruct {
    int a;
    char b;
  };
  MyStruct* struct_data = shm.Get<MyStruct>();
  ASSERT_NE(struct_data, nullptr);
  struct_data->a = 123;
  struct_data->b = 'x';
  ASSERT_EQ(struct_data->a, 123);
  ASSERT_EQ(struct_data->b, 'x');
}

// Test constructor with negative size.
TEST(SmartSHMTest, ConstructorNegativeSize) {
  SmartSHM shm(-1);
  ASSERT_EQ(shm.GetSize(), -1);
}

// Test Init() method with negative size.
TEST(SmartSHMTest, InitNegativeSize) {
  SmartSHM shm(1024);
  ASSERT_FALSE(shm.Init(-1));
}

// Test Init() method with already initialized shared memory.
TEST(SmartSHMTest, InitAlreadyInitialized) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
  ASSERT_FALSE(shm.Init(1234));
}

// Test Init() method with existing shared memory.
TEST(SmartSHMTest, InitExistingMemory) {
  SmartSHM shm1(1024);
  ASSERT_TRUE(shm1.Init(1234));
  int shmid = shm1.GetShmid();
  SmartSHM shm2(1024);
  ASSERT_TRUE(shm2.Init(1234));
  ASSERT_EQ(shm2.GetShmid(), shmid);
}

// Test Get() method with uninitialized shared memory.
TEST(SmartSHMTest, GetUninitializedMemory) {
  SmartSHM shm(1024);
  ASSERT_EQ(shm.Get<int>(), nullptr);
}

// Test Get() method with invalid type size.
TEST(SmartSHMTest, GetInvalidTypeSize) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
  ASSERT_EQ(shm.Get<char>(), nullptr);
}

// Test destructor with initialized shared memory.
TEST(SmartSHMTest, DestructorInitializedMemory) {
  {
    SmartSHM shm(1024);
    ASSERT_TRUE(shm.Init(1234));
  }
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
  ASSERT_EQ(shm.GetShmid(), -1);
}

// Test constructor with positive size.
TEST(SmartSHMTest, ConstructorPositiveSize) {
  SmartSHM shm(1024);
  ASSERT_EQ(shm.GetSize(), 1024);
}

// Test Init() method with positive size.
TEST(SmartSHMTest, InitPositiveSize) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
}

// Test Get() method with initialized shared memory.
TEST(SmartSHMTest, GetInitializedMemory) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
  int* ptr = shm.Get<int>();
  ASSERT_NE(ptr, nullptr);
  *ptr = 42;
  ASSERT_EQ(*ptr, 42);
}

// Test GetSize() method with initialized shared memory.
TEST(SmartSHMTest, GetSizeInitializedMemory) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
  ASSERT_EQ(shm.GetSize(), 1024);
}

// Test GetShmid() method with initialized shared memory.
TEST(SmartSHMTest, GetShmidInitializedMemory) {
  SmartSHM shm(1024);
  ASSERT_TRUE(shm.Init(1234));
  ASSERT_NE(shm.GetShmid(), -1);
}

// Test destructor with uninitialized shared memory.
TEST(SmartSHMTest, DestructorUninitializedMemory) {
  SmartSHM* shm = new SmartSHM(1024);
  delete shm;
}

// Test destructor with initialized shared memory.
TEST(SmartSHMTest, DestructorInitializedMemory) {
  SmartSHM* shm = new SmartSHM(1024);
  ASSERT_TRUE(shm->Init(1234));
  delete shm;
}

