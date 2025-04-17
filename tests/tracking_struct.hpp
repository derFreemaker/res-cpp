#ifndef RESCPP_TESTS_TRACKING_STRUCT_HPP
#define RESCPP_TESTS_TRACKING_STRUCT_HPP

#include <cstdint>
#include <optional>
#include <vector>
#include <stacktrace>
#include <gtest/gtest.h>

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
        operations.emplace_back(type, std::stacktrace::current(1));
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
};

template <typename T>
struct TrackingStruct {
private:
    TrackingStruct(TrackingStats* stats, T&& value) noexcept
        : stats(stats), value(std::forward<T>(value)) {}

    friend struct TrackingHelper;

public:
    TrackingStats* stats;
    T value;

    TrackingStruct() noexcept = delete;

    TrackingStruct(const TrackingStruct& other) noexcept
        : stats(other.stats), value(other.value) {
        stats->add_operation(OperationType::CopyConstructor);
    }

    TrackingStruct& operator=(const TrackingStruct& other) noexcept {
        stats = other.stats;
        stats->add_operation(OperationType::CopyAssignment);
        value = other.value;
        return *this;
    }

    TrackingStruct(const TrackingStruct&& other) noexcept
        : stats(other.stats), value(std::move(other.value)) {
        stats->add_operation(OperationType::MoveConstructor);
    }
    TrackingStruct& operator=(const TrackingStruct&& other) noexcept {
        stats = other.stats;
        stats->add_operation(OperationType::MoveAssignment);
        value = std::move(other.value);
        return *this;
    }

    ~TrackingStruct() noexcept {
        stats->add_operation(OperationType::Destructor);
    }
};

struct TrackingHelper {
    static TrackingStats& get_stats() {
        static TrackingStats stats{};
        return stats;
    }

    template <typename T>
    [[nodiscard]]
    static TrackingStruct<T> create_struct(T&& args) {
        return TrackingStruct(&get_stats(), std::forward<T>(args));
    }
};

}

#define FAIL_TRACKING_HAS_OPERATION(Type) \
    if (const auto operation = ::ResCpp::testing::TrackingHelper::get_stats().has_operation(OperationType::Type)) { \
        FAIL() << #Type << " happend at:\n" <<  operation.value().stacktrace; \
    }

#define FAIL_TRACKING_HAS_OPERATIONS() \
    FAIL_TRACKING_HAS_OPERATION(CopyConstructor) \
    FAIL_TRACKING_HAS_OPERATION(CopyAssignment) \
    FAIL_TRACKING_HAS_OPERATION(MoveConstructor) \
    FAIL_TRACKING_HAS_OPERATION(MoveAssignment) \
    FAIL_TRACKING_HAS_OPERATION(Destructor)

#endif //RESCPP_TESTS_TRACKING_STRUCT_HPP
