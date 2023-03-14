#include "Signal.h"

#include <gtest/gtest.h>

TEST(SignalTest, ConnectSlotReturnsUniqueID)
{
    sig::Signal<void()> signal;
    std::size_t id1 = signal.connectSlot([](){});
    std::size_t id2 = signal.connectSlot([](){});

    EXPECT_NE(id1, id2);
}

TEST(SignalTest, MultipleCallbacks) {
    sig::Signal<void(int)> signal;

    std::vector<int> results;

    signal.connectSlot([&results](int i){ results.push_back(i); });
    signal.connectSlot([&results](int i){ results.push_back(i*2); });

    signal.emitSignal(42);

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], 42);
    EXPECT_EQ(results[1], 84);
}

void callback(int param)
{
	std::printf("Hello %i\n", param);
}

TEST(SignalTest, TestSubject)
{
	// define a signal for functions that takes an int
	// and returns void
	sig::Signal<void(int)> sig;
	// connect a simple function
	sig.connectSlot(callback);
	// connect a lambda function
	int res = 0;
	sig.connectSlot([&res](int x)
					{ res = x; });
	// emit the signal
	sig.emitSignal(1);
	// here res equals 1 and "Hello 1" is printed on stdout
}













int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
