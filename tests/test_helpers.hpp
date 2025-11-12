#pragma once

#include <vector>
#include <iostream>

#define ASSERT(condition, message) \
do \
{ \
    if (!(condition)) \
    { \
        std::cerr << "FAILED: " << #condition << " message: " << message << " " << std::endl; \
        return 1; \
    } \
} while (0)

#define VECTOR_COMPARE(v1, v2) \
do \
{ \
    ASSERT(v1.size() == v2.size(), " sizes dont match"); \
    for (size_t i = 0; i < v1.size(); i++) \
    { \
        if (v1[i] != v2[i]) \
        { \
            std::cerr << "FAILED: " << " v1[" << i << "] = " << v1[i] << " != " << " v2[" << i << "] = " << v2[i] << std::endl; \
            return 1; \
        } \
    } \
} while (0)

template <typename T>
bool vector_compare(const std::vector<T> &v1, const std::vector<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "FAILED: sizes don't match" << std::endl;
        return false;
    }

    for (size_t i = 0; i < v1.size(); i++)
    {
        if (v1[i] != v2[i])
        {
            std::cerr << "FAILED: v1[" << i << "] = " << v1[i]
                      << " != v2[" << i << "] = " << v2[i] << std::endl;
            return false;
        }
    }
    return true;
}

#define RUN_TEST(test_func) \
do                                        \
{\
    std::cout << "Running test: " << #test_func << " .... "; \
    int testresult = test_func(); \
    if (testresult == 0) \
    { \
        std::cout << "PASSED" << std::endl; \
    } \
    else \
    { \
        f_tests++; \
    } \
    t_tests++; \
} while (0)
