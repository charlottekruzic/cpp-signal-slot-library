#include "Signal.h"

#include <gtest/gtest.h>

#include <list>

/**
 * ID uniqueness tests
*/
TEST(connectSlot, UniqueID)
{
    sig::Signal<void()> signal;

    std::size_t id1 = signal.connectSlot([](){});
    std::size_t id2 = signal.connectSlot([](){});
    std::size_t id3 = signal.connectSlot([](){});

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id3, id1);
}

/**
 * Disconnect slots tests
*/
TEST (disconnectSlot, One) {
    sig::Signal<void(int)> signal;
   
    int res1=0;
    std::size_t id1 = signal.connectSlot([&res1](int i){ res1=i; });
    int res2=0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2=i*2; });

    signal.emitSignal(2);

    EXPECT_EQ(res1,2);
    EXPECT_EQ(res2,4);


    signal.disconnectSlot(id2);

    signal.emitSignal(3);
    EXPECT_EQ(res1,3);
    EXPECT_EQ(res2,4);


}

TEST (disconnectSlot, All) {
    sig::Signal<void(char)> signal;
   
    int res1=0;
    std::size_t id1 = signal.connectSlot([&res1](int i){ res1=i; });
    int res2=0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2=i*2; });

    signal.emitSignal(2);
    EXPECT_EQ(res1,2);
    EXPECT_EQ(res2,4);


    signal.disconnectSlot(id1);
    signal.disconnectSlot(id2);

    signal.emitSignal(3);
    EXPECT_EQ(res1,2);
    EXPECT_EQ(res2,4);
}

TEST (disconnectSlot, KeepOtherAccess) {
    sig::Signal<void(int)> signal;
   
    int res1=0;
    std::size_t id1 = signal.connectSlot([&res1](int i){ res1=i; });
    int res2=0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2=i*2; });
    int res3=0;
    std::size_t id3 = signal.connectSlot([&res3](int i){ res3=i*3; });

    signal.emitSignal(2);
    EXPECT_EQ(res1,2);
    EXPECT_EQ(res2,4);
    EXPECT_EQ(res3,6);

    signal.disconnectSlot(id2);

    signal.emitSignal(3);
    EXPECT_EQ(res1,3);
    EXPECT_EQ(res2,4);
    EXPECT_EQ(res3,9);
}


/**
 * Multiple args type
*/
TEST(signalTest, multipleArgsType) {
    sig::Signal<void(int,char)> signal;

    std::vector<int> ints;
    std::vector<char> chars;
    std::vector<int,char> ints_chars;

    signal.connectSlot([&ints, &chars](int i, char c){ ints.push_back(i); chars.push_back(c);});
    signal.connectSlot([&ints, &chars](int i, char c){ ints.push_back(i*2); chars.push_back(c+1);});

    signal.emitSignal(1,'a');

    EXPECT_EQ(ints.size(), 2);
    EXPECT_EQ(chars.size(), 2);

    EXPECT_EQ(ints[0], 1);
    EXPECT_EQ(ints[1], 2);

    EXPECT_EQ(chars[0], 'a');
    EXPECT_EQ(chars[1], 'b');
}

//tests multiple et bon ordre



/**
 * project statement test
*/
void callback(int param)
{
	printf("Hello %i\n", param);
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
    EXPECT_EQ(res,1);
}




//test différents signaux
//différentes args 
//test args (char)...
//faire int(int)
//






int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
