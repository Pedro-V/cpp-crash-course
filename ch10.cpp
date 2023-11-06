#include <stdexcept>
#include <functional>
#include <cstdio>

/*
 * Since all frameworks write the main function themselves, we use macros to
 * control which examples are run. The frameworks are 0-indexed, following the
 * order [Catch2, GTest, Boost]. The index chosen is controlled by the macro
 * TEST_FRAMEWORK. If defined, its value defines the test framework used.
 * Otherwise, the examples without framework are used.
 *
 * The exercies were made with TEST_FRAMEWORK = 0.
 */


#define TEST_FRAMEWORK 0

// -------------------------------------------------------------------------------- 
/*
 * Testing:
 * Tests provide a better way for checking (not proving) the incorrectness of
 * software. The classical way of writing, compiling, running and debugging is
 * neither efficient or modular. 
 *
 * TDD is the development technique of writing test before implementing
 * features, centered around unit testing. 
 *
 * Test harness is code that executes unit tests.
 */


// -------------------------------------------------------------------------------- 
// Brake example

// Utilities
constexpr void assert_that(bool statement, const char* message) {
    if (!statement) throw std::runtime_error{ message };
}

void run_test(void(*unit_test)(), const char* name) {
    try {
        unit_test();
        printf("[+] Test %s successful.\n", name);
    } catch (const std::exception& e) {
        printf("[-] Test failure in %s: %s.\n", name, e.what());
    }
}

// Classes

struct SpeedUpdate {
    double velocity_mps;
};

struct CarDetected {
    double distance_m;
    double velocity_mps;
};

// Exercise 10-1
struct SpeedLimitDetected {
    unsigned short speed_mps;
};

struct BrakeCommand {
    double time_to_collision_s;
};

using SpeedUpdateCallback = std::function<void(const SpeedUpdate&)>;
using CarDetectedCallback = std::function<void(const CarDetected&)>;
using SpeedLimitCallback = std::function<void(const SpeedLimitDetected&)>;

struct IServiceBus {
    virtual ~IServiceBus() = default;
    virtual void publish(const BrakeCommand&) = 0;
    virtual void subscribe(SpeedUpdateCallback) = 0;
    virtual void subscribe(CarDetectedCallback) = 0;
    virtual void subscribe(SpeedLimitCallback) = 0;
};

// Mocks: An implementation generated for the purposes of testing.

struct MockServiceBus : IServiceBus {
    void publish(const BrakeCommand& cmd) override {
        commands_published++;
        last_command = cmd;
    }

    void subscribe(SpeedUpdateCallback callback) override {
        speed_update_callback = callback;
    }

    void subscribe(CarDetectedCallback callback) override {
        car_detected_callback = callback;
    }

    void subscribe(SpeedLimitCallback callback) override {
        speed_limit_callback = callback;
    }

    BrakeCommand last_command{};
    int commands_published{};
    SpeedUpdateCallback speed_update_callback{};
    CarDetectedCallback car_detected_callback{};
    SpeedLimitCallback speed_limit_callback{};
};

struct AutoBrake {
    AutoBrake(IServiceBus& bus)
        : collision_threshold_s{ 5 },
          // Exercise 10-4
          last_known_speed_limit{ 39 },
          speed_mps{} {
        bus.subscribe([this, &bus](const SpeedUpdate& update) {
            // Exercise 10-8
            if (update.velocity_mps > last_known_speed_limit) {
                double time_to_collision_s = 0;
                bus.publish(BrakeCommand{ time_to_collision_s });
            } else {
                speed_mps = update.velocity_mps;
            }
        });

        bus.subscribe([this, &bus](const CarDetected& cd) {
                const auto relative_velocity_mps = speed_mps;
                const auto time_to_collision_s = cd.distance_m / relative_velocity_mps;
                if (time_to_collision_s > 0 &&
                    time_to_collision_s <= collision_threshold_s) {
                    bus.publish(BrakeCommand{ time_to_collision_s });
                }
        });

        // Exercise 10-6
        bus.subscribe([this, &bus](const SpeedLimitDetected& limit) {
            last_known_speed_limit = limit.speed_mps;
            //Exercise 10-10
            if (speed_mps > last_known_speed_limit) {
                double time_to_collision_s = 0;
                bus.publish(BrakeCommand{ time_to_collision_s });
            }
        });
    }

    void set_collision_threshold_s(double x) {
        if (x < 1) throw std::out_of_range{ "Collision less than 1." };
        collision_threshold_s = x;
    }

    double get_collision_threshold_s() const {
        return collision_threshold_s;
    }

    double get_speed_mps() const {
        return speed_mps;
    }

    // Exercise 10-2
    unsigned short get_last_known_speed_limit() const {
        return last_known_speed_limit;
    }

private:
    double collision_threshold_s;
    double speed_mps;
    unsigned short last_known_speed_limit;
};

#ifndef TEST_FRAMEWORK

// Tests

void initial_speed_is_zero() {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    assert_that(auto_brake.get_speed_mps() == 0L, "Initial speed not equal to 0");
}

void initial_sensitiviy_is_five() {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    assert_that(auto_brake.get_collision_threshold_s() == 5,
                "sensitivity is not 5");
}

void sensitivity_greater_than_one() {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    try {
        auto_brake.set_collision_threshold_s(0.5L);
    } catch (const std::exception& e) {
        return;
    }
    assert_that(false, "no exception thrown");
}

void speed_is_saved() {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };

    bus.speed_update_callback(SpeedUpdate{ 100L });
    assert_that(auto_brake.get_speed_mps() == 100L, "speed not saved to 100");

    bus.speed_update_callback(SpeedUpdate{ 50L });
    assert_that(auto_brake.get_speed_mps() == 50L, "speed not saved to 50");

    bus.speed_update_callback(SpeedUpdate{ 0L });
    assert_that(auto_brake.get_speed_mps() == 0L, "speed not saved to 0");
}

void alert_when_collision_imminent() {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };

    auto_brake.set_collision_threshold_s(10L);
    bus.speed_update_callback(SpeedUpdate{ 100L });
    bus.car_detected_callback(CarDetected{ 100L, 0L });

    assert_that(bus.commands_published == 1, "brake commands published not one");
    assert_that(bus.last_command.time_to_collision_s == 1L,
                "time to collision not computed correctly");
}

void no_alert_when_collision_not_imminent() {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };

    auto_brake.set_collision_threshold_s(2L);
    bus.speed_update_callback(SpeedUpdate{ 100L });
    bus.car_detected_callback(CarDetected{ 1000L, 50L });

    assert_that(bus.commands_published == 0, "brake command published");
}


// Test harnessing
void run_tests() {
    run_test(initial_speed_is_zero, "initial_speed_is_0");
    run_test(initial_sensitiviy_is_five, "initial_sensitiviy_is_five");
    run_test(sensitivity_greater_than_one, "sensitivity_greater_than_one");
    run_test(speed_is_saved, "speed_is_saved");
    run_test(alert_when_collision_imminent, "alert_when_collision_imminent");
    run_test(no_alert_when_collision_not_imminent,
            "no_alert_when_collision_not_imminent");
}

// -------------------------------------------------------------------------------- 
// Unit-testing frameworks

#else
#  if TEST_FRAMEWORK == 0

// Catch2.

// Below we assume the headers of Catch2 2.* is present in the default include
// paths, inside a catch2 directory.
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("initial car speed is negative") {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    // The test fails if the REQUIRE assertion is false.
    // There are many other macros from Catch2 that capture common patterns in
    // testing, such as requiring the thrown of an exception, checking (test
    // won't fail immediately, but will run to completion and then fail) etc.
    REQUIRE(auto_brake.get_speed_mps() < 0);
}

TEST_CASE("AutoBrakeT") {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    // Handles floating-point arithmetic better, since checks for equality may
    // entail rouding errors. We should test for the difference between the
    // values being arbitrarily small.
    REQUIRE(auto_brake.get_collision_threshold_s() == Approx(5L));
}

// Better organization of the setup code of related tests can be better written
// with test cases and sections. Below is the complete rewrite of the bare-hands
// implementation of the test to make use of Catch2.

TEST_CASE("AutoBrake") {
    // Setup code.
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };

    SECTION("initializes speed to zero") {
        REQUIRE(auto_brake.get_speed_mps() == Approx(0));
    }

    // Exercise 10-3
    SECTION("initializes last known speed to zero") {
        REQUIRE(auto_brake.get_last_known_speed_limit() == 39);
    }

    SECTION("initializes sensitivity to five") {
        REQUIRE(auto_brake.get_collision_threshold_s() == Approx(5));
    }

    SECTION("throws when sensitivity less than one") {
        REQUIRE_THROWS(auto_brake.set_collision_threshold_s(0.5L));
    }

    SECTION("saves speed after update") {
        bus.speed_limit_callback(SpeedLimitDetected{ 100L });

        bus.speed_update_callback(SpeedUpdate{ 100L });
        REQUIRE(auto_brake.get_speed_mps() == Approx(100L));

        bus.speed_update_callback(SpeedUpdate{ 50L });
        REQUIRE(auto_brake.get_speed_mps() == Approx(50L));

        bus.speed_update_callback(SpeedUpdate{ 0L });
        REQUIRE(auto_brake.get_speed_mps() == Approx(0L));
    }

    // Exercise 10-5
    SECTION("saves last known speed") {
        bus.speed_limit_callback(SpeedLimitDetected{ 20 });
        REQUIRE(auto_brake.get_last_known_speed_limit() == 20);
        
        bus.speed_limit_callback(SpeedLimitDetected{ 100 });
        REQUIRE(auto_brake.get_last_known_speed_limit() == 100);
        
        bus.speed_limit_callback(SpeedLimitDetected{ 820 });
        REQUIRE(auto_brake.get_last_known_speed_limit() == 820);
    }

    // Exercise 10-7
    SECTION("no brake when under speed limit") {
        bus.speed_limit_callback(SpeedLimitDetected{ 35 });
        bus.speed_update_callback(SpeedUpdate{ 34L });

        REQUIRE(bus.commands_published == 0);
    }

    // Exercise 10-9
    SECTION("one brake issued when over speed limit") {
        bus.speed_limit_callback(SpeedLimitDetected{ 35 });
        bus.speed_update_callback(SpeedUpdate{ 40L });
        REQUIRE(bus.commands_published == 1);
        REQUIRE(bus.last_command.time_to_collision_s == Approx(0));
    }

    // Exercise 10-11
    SECTION("brake issued when smaller speed limit is encountered") {
        bus.speed_limit_callback(SpeedLimitDetected{ 35 });
        bus.speed_update_callback(SpeedUpdate{ 30L });
        bus.speed_limit_callback(SpeedLimitDetected{ 25 });
    }

    SECTION("alerts when collision is imminent") {
        bus.speed_limit_callback(SpeedLimitDetected{ 100 });

        auto_brake.set_collision_threshold_s(10L);
        bus.speed_update_callback(SpeedUpdate{ 100L });
        bus.car_detected_callback(CarDetected{ 100L, 0L });

        REQUIRE(bus.commands_published == 1);
        REQUIRE(bus.last_command.time_to_collision_s == Approx(1L));
    }

    SECTION("doesn't alerts when collision isn't imminent") {
        bus.speed_limit_callback(SpeedLimitDetected{ 100 });

        auto_brake.set_collision_threshold_s(2L);
        bus.speed_update_callback(SpeedUpdate{ 100L });
        bus.car_detected_callback(CarDetected{ 1000L, 50L });

        REQUIRE(bus.commands_published == 0);
    }
}

// Exercises: The exercises for unit tests were implemented in the test case
// with the others. The exercises for new types where implemented close to the
// other classes.


#  elif TEST_FRAMEWORK == 1
// Google Test. Must be compiled with -lgtest{,_main} flags. Assertions aren't
// parsed to identify the operation used.

#include <gtest/gtest.h>

TEST(AutoBrake, UnitTestName) {}

// A test that intentionally fails
TEST(AutoBrake, InitialCarSpeedIsNegative) {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    ASSERT_LT(auto_brake.get_speed_mps(), 0);
}

// Setup and teardown code must be put in a class that inherits from the Test
// class provided by GTest, that is, either as public/protected members or (if
// the setup is a computation) inside its constructors and destructors.
// These are called test fixture classes.
struct AutoBrakeTest : ::testing::Test {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
};

// TEST_F macro is analogous to SECTION macro.
TEST_F(AutoBrakeTest, InitialCarSpeedIsZero) {
    ASSERT_DOUBLE_EQ(0, auto_brake.get_speed_mps());
}

TEST_F(AutoBrakeTest, InitialSensitivityIsFive) {
    ASSERT_DOUBLE_EQ(5, auto_brake.get_collision_threshold_s());
}

TEST_F(AutoBrakeTest, SensitivityGreaterThanOne) {
    ASSERT_ANY_THROW(auto_brake.set_collision_threshold_s(0.5L));
}

TEST_F(AutoBrakeTest, SpeedIsSaved) {
    bus.speed_update_callback(SpeedUpdate{ 100L });
    ASSERT_EQ(100, auto_brake.get_speed_mps());

    bus.speed_update_callback(SpeedUpdate{ 50L });
    ASSERT_EQ(50, auto_brake.get_speed_mps());

    bus.speed_update_callback(SpeedUpdate{ 0L });
    ASSERT_EQ(0, auto_brake.get_speed_mps());
}

TEST_F(AutoBrakeTest, NotAlertWhenNotImminent) {
    auto_brake.set_collision_threshold_s(2L);
    bus.speed_update_callback(SpeedUpdate{ 100L });
    bus.car_detected_callback(CarDetected{ 1000L, 50L });
    ASSERT_EQ(0, bus.commands_published);
}

TEST_F(AutoBrakeTest, AlertWhenImminent) {
    auto_brake.set_collision_threshold_s(10L);
    bus.speed_update_callback(SpeedUpdate{ 100L });
    bus.car_detected_callback(CarDetected{ 100L, 0L });
    ASSERT_EQ(1, bus.commands_published);
    ASSERT_DOUBLE_EQ(1L, bus.last_command.time_to_collision_s);
}

#  elif TEST_FRAMEWORK == 2

// Boost Test. Must be compiled with -lboost_unit_test_framework, if used with
// dynamic linking.

// Allows for multiple modes: header-only, static or shared library. Here we use
// dynamic. 
#    define BOOST_TEST_MODULE AutoBrakeTest
#    define BOOST_TEST_DYN_LINK
#    include <boost/test/unit_test.hpp>

// Also makes use of test fixture classes, but without requiring inheritance.
struct AutoBrakeTest {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
};

// Tests for fixture classes are defined with BOOST_FIXTURE_TEST_CASE. For
// standalone tests, BOOST_AUTO_TEST_CASE is used.

// Intentional test failure.
BOOST_AUTO_TEST_CASE(InitialCarSpeedIsNegative) {
    MockServiceBus bus{};
    AutoBrake auto_brake{ bus };
    BOOST_TEST(auto_brake.get_speed_mps() < 0);
}

BOOST_FIXTURE_TEST_CASE(InitialCarSpeedIsZero, AutoBrakeTest) {
    // Apparently, it automatically handles parsing of the operations and of
    // floating-point comparison.
    BOOST_TEST(0 == auto_brake.get_speed_mps());
}

BOOST_FIXTURE_TEST_CASE(InitialSensitivityIsFive, AutoBrakeTest) {
    BOOST_TEST(5 == auto_brake.get_collision_threshold_s());
}

BOOST_FIXTURE_TEST_CASE(SensitivityAboveOne, AutoBrakeTest) {
    BOOST_REQUIRE_THROW(auto_brake.set_collision_threshold_s(0.5L), std::exception);
}

BOOST_FIXTURE_TEST_CASE(SpeedIsSaved, AutoBrakeTest) {
    bus.speed_update_callback(SpeedUpdate{ 100L });
    BOOST_TEST(100 == auto_brake.get_speed_mps());
    
    bus.speed_update_callback(SpeedUpdate{ 50L });
    BOOST_TEST(50 == auto_brake.get_speed_mps());
    
    bus.speed_update_callback(SpeedUpdate{ 0L });
    BOOST_TEST(0 == auto_brake.get_speed_mps());
}

BOOST_FIXTURE_TEST_CASE(NoAlertWhenNotImminent, AutoBrakeTest) {
    auto_brake.set_collision_threshold_s(2L);
    bus.speed_update_callback(SpeedUpdate{ 100L });
    bus.car_detected_callback(CarDetected{ 1000L, 50L });
    BOOST_TEST(0 == bus.commands_published);
}

BOOST_FIXTURE_TEST_CASE(AlertWhenImminent, AutoBrakeTest) {
    auto_brake.set_collision_threshold_s(10L);
    bus.speed_update_callback(SpeedUpdate{ 100L });
    bus.car_detected_callback(CarDetected{ 100L, 0L });
    BOOST_TEST(1 == bus.commands_published);
    BOOST_TEST(1L == bus.last_command.time_to_collision_s);
}

#  else

int main(void) {
    printf("No tests here!\n");
}

#  endif
#endif
