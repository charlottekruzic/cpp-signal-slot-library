#include "Signal.h"

#include <gtest/gtest.h>
#include <vector>


/***************************************************************************************
 *                                  DiscardCombiner
***************************************************************************************/

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
 * Connect function test
*/
void callback_1(int& res)
{
    res = 1;
}

TEST(SignalTest, ConnectFunction)
{
    sig::Signal<void(int&)> signal;
    int res = 0;
    signal.connectSlot(&callback_1);
    signal.emitSignal(res);
    EXPECT_EQ(res, 1);
}



/**
 * Connect same function test
*/
void callback_2(int& counter) {
    ++counter;
}

TEST(SignalTest, ConnectSameFunctionTwice) {
    sig::Signal<void(int&)> signal;
    int counter = 0;
    signal.connectSlot(&callback_2);
    signal.connectSlot(&callback_2);
    signal.emitSignal(counter);
    EXPECT_EQ(counter, 2);
}


/**
 * Connect lambda test
*/
TEST(SignalTest, connectLambda)
{
    sig::Signal<void(int)> signal;
    int res = 0;
    signal.connectSlot([&res](int x){ res = x; });
    signal.emitSignal(20);
    EXPECT_EQ(res, 20);
}


/**
 * Connect object test ?
*/
class TestClass
{
public:
    void callback_class(int signal) { res = signal + 2; }
    int res = 0;
};

TEST(connectSlot, ObjectSlot)
{
    sig::Signal<void(int)> sig;
    TestClass obj;
    sig.connectSlot(std::bind(&TestClass::callback_class, &obj, std::placeholders::_1));
    sig.emitSignal(4);
    EXPECT_EQ(obj.res, 6);
}



/**
 * Disconnect slots tests
*/
TEST (disconnectSlot, One) {
    sig::Signal<void(int)> signal;
   
    int res1=0;
    signal.connectSlot([&res1](int i){ res1=i; });
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
    signal.connectSlot([&res1](int i){ res1=i; });
    int res2=0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2=i*2; });
    int res3=0;
    signal.connectSlot([&res3](int i){ res3=i*3; });

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
    //std::vector<int,char> ints_chars;

    signal.connectSlot([&ints, &chars](int i, char c){ ints.push_back(i); chars.push_back(c);});
    signal.connectSlot([&ints, &chars](int i, char c){ ints.push_back(i*2); chars.push_back(c+1);});

    signal.emitSignal(1,'a');

    EXPECT_EQ(ints.size(), 2u);
    EXPECT_EQ(chars.size(), 2u);

    EXPECT_EQ(ints[0], 1);
    EXPECT_EQ(ints[1], 2);

    EXPECT_EQ(chars[0], 'a');
    EXPECT_EQ(chars[1], 'b');
}

/**
 * Vector args type
*/
TEST(connectSlot, LambdaWithVector)
{
    sig::Signal<void(std::vector<int>)> sig;
    std::vector<int> v = {1, 2, 3};
    int sum = 0;

    sig.connectSlot([&sum](std::vector<int> vec) {
        for (int i : vec) {
            sum += i;
        }
    });

    sig.emitSignal(v);
    EXPECT_EQ(sum, 6);
}

/**
 * Emit without slot connected test
*/
TEST(emitSignal, NoSlotsConnected)
{
    sig::Signal<void()> sig;
    EXPECT_NO_THROW(sig.emitSignal());
}

TEST(Signal, EmitNoConnectedSlot)
{
    sig::Signal<void(int)> sig;
    EXPECT_NO_THROW(sig.emitSignal(42));
}

/**
 * passing arguments by reference test
*/
TEST(connectSlot, RefArgument)
{
    sig::Signal<void(int&)> sig;
    int value = 0;
    sig.connectSlot([&value](int& x) { value = x;});

    int arg = 42;
    sig.emitSignal(arg);

    EXPECT_EQ(value, arg);
}

/**
 * passing pointer test
*/


/**
 * Priority of slots test
*/




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


/*******************************************************************************
 *                               LastCombiner
*******************************************************************************/

/*TEST(LastCombiner, test)
{
    sig::Signal<void(int), sig::LastCombiner<int>> sig;
    int res = 0;
    std::size_t id1 = sig.connectSlot([&res](int x){ res = x+1; });
    sig.emitSignal(1);
    EXPECT_EQ(res,1);
}*/


/**
 * Connect function test
*/



























//test différents signaux
//différentes args 
//test args (char)...
//faire int(int)
//

//doit-on faire un destructeur ?





int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
