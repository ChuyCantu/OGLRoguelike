#ifndef __COMMON_H__
#define __COMMON_H__

//? This should be in a precompiled header

#include <map>
#include <memory>
#include <string>
#include <stdint.h>
#include <unordered_map>
#include <vector>

template <typename T>
using Owned = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Owned<T> MakeOwned(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> MakeRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using WeakRef = std::weak_ptr<T>;

#endif // __COMMON_H__