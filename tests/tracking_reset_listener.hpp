#ifndef RESCPP_TESTS_RESULT_TEST_FIXTURE_HPP
#define RESCPP_TESTS_RESULT_TEST_FIXTURE_HPP

#include <gtest/gtest.h>

#include "tracking_struct.hpp"

namespace ResCpp::testing {

struct TrackingResetListener : ::testing::EmptyTestEventListener {
    TrackingResetListener() = default;

    void OnTestStart(const ::testing::TestInfo& test_info) override {
        TrackingHelper::get_stats()->reset();
    }
};

}

#endif //RESCPP_TESTS_RESULT_TEST_FIXTURE_HPP
