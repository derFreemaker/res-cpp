#include "tracking_reset_listener.hpp"

#include <mutex>

namespace ResCpp::testing {

namespace {
struct GlobalListenerRegistration {
    std::unique_ptr<TrackingResetListener> tracking_reset;

    GlobalListenerRegistration()
        : tracking_reset(std::make_unique<TrackingResetListener>()) {
        ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
        listeners.Append(tracking_reset.get());
    }
};

[[maybe_unused]] GlobalListenerRegistration global_registration;
}

}
