#ifndef RESCPP_TESTS_TRACKING_STRUCT_HPP
#define RESCPP_TESTS_TRACKING_STRUCT_HPP

#include <cstdint>
#include <optional>
#include <vector>
#include <stacktrace>
#include <gtest/gtest.h>

#include "tracking_struct.hpp"

namespace ResCpp::testing {

enum class OperationType : uint8_t {
    None = 0,
    DefaultConstructor,
    ValueConstructor,
    CopyConstructor,
    MoveConstructor,
    CopyAssignment,
    MoveAssignment,
    Destructor,
};

struct OperationEntry {
    OperationType type;
    std::stacktrace stacktrace;
};

struct TrackingStats {
private:
    std::vector<OperationEntry> operations{};

public:
    void reset() {
        operations.clear();
    }

    void add_operation(OperationType type) {
        operations.emplace_back(type, std::stacktrace::current(2));
    }

    [[nodiscard]]
    std::optional<OperationEntry> has_operation(const OperationType type) const {
        for (const auto& element : operations) {
            if (element.type == type) {
                return element;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]]
    std::vector<OperationEntry> get_operations(const OperationType type) const {
        std::vector<OperationEntry> result;
        for (const auto& element : operations) {
            if (element.type == type) {
                result.push_back(element);
            }
        }
        return result;
    }
};

struct TrackingHelper {
    static TrackingStats* get_stats() {
        static TrackingStats stats{};
        return &stats;
    }
};

template <typename T>
struct TrackingStruct {
    TrackingStats* stats;
    T value;

    TrackingStruct() noexcept
        : stats(TrackingHelper::get_stats()),
          value() {
        stats->add_operation(OperationType::DefaultConstructor);
    }

    TrackingStruct(T&& value) noexcept
        : stats(TrackingHelper::get_stats()),
          value(std::forward<T>(value)) {
        stats->add_operation(OperationType::ValueConstructor);
    }

    TrackingStruct(const TrackingStruct& other) noexcept
        : stats(other.stats), value(other.value) {
        stats->add_operation(OperationType::CopyConstructor);
    }

    TrackingStruct& operator=(const TrackingStruct& other) noexcept {
        if (this == &other) {
            return *this;
        }

        stats = other.stats;
        stats->add_operation(OperationType::CopyAssignment);
        value = other.value;
        return *this;
    }

    TrackingStruct(const TrackingStruct&& other) noexcept
        : stats(other.stats), value(std::move(other.value)) {
        stats->add_operation(OperationType::MoveConstructor);
    }

    TrackingStruct& operator=(TrackingStruct&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        stats = other.stats;
        stats->add_operation(OperationType::MoveAssignment);
        value = std::move(other.value);
        return *this;
    }

    ~TrackingStruct() noexcept {
        stats->add_operation(OperationType::Destructor);
    }
};

}

#define FAIL_TRACKING_HAS_OPERATION(Type) \
    { \
        if (const auto operation = ::ResCpp::testing::TrackingHelper::get_stats()->has_operation(OperationType::Type)) { \
            FAIL() << #Type << " happened at:\n" << operation.value().stacktrace; \
        } \
    }

#define FAIL_TRACKING_HAS_MORE_OPERATION(Type, Count) \
    { \
        const auto operations = ::ResCpp::testing::TrackingHelper::get_stats()->get_operations(OperationType::Type); \
        if (operations.size() > Count) { \
            FAIL() << #Type << " happened more than: " << Count << "\n"; \
        } \
    }


#define FAIL_TRACKING_HAS_OPERATIONS() \
    FAIL_TRACKING_HAS_OPERATION(DefaultConstructor) \
    FAIL_TRACKING_HAS_OPERATION(ValueConstructor) \
    FAIL_TRACKING_HAS_OPERATION(CopyConstructor) \
    FAIL_TRACKING_HAS_OPERATION(CopyAssignment) \
    FAIL_TRACKING_HAS_OPERATION(MoveConstructor) \
    FAIL_TRACKING_HAS_OPERATION(MoveAssignment) \
    FAIL_TRACKING_HAS_OPERATION(Destructor)

#endif //RESCPP_TESTS_TRACKING_STRUCT_HPP
