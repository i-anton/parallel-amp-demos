#pragma once

#include <gtest/gtest.h>
#include <amp.h>

#define ACC_TYPE "GPU"

class Environment : public ::testing::Environment {
public:
    ~Environment() override {}
    void SetUp() override {
        using namespace concurrency;
        const auto accs = accelerator::get_all();
        accelerator chosen_one = accs[0];
        std::wcout << chosen_one.description << std::endl;
        bool success = accelerator::set_default(chosen_one.device_path);
    }
    void TearDown() override {}
};

::testing::Environment* const foo_env = ::testing::AddGlobalTestEnvironment(new Environment);