#include "Signal.h"

#include <gtest/gtest.h>
#include <vector>

/********************************************************
 *          Structure with an impossible copy
 ********************************************************/
struct NoCopy
{
public:
    NoCopy() : int_no_copy(0) {}
    NoCopy(int i) : int_no_copy(i) {}
    NoCopy(const NoCopy &) = delete;
    NoCopy &operator=(const NoCopy &) = delete;
    NoCopy(NoCopy &&other) noexcept : int_no_copy(other.int_no_copy)
    {
    }
    NoCopy &operator=(NoCopy &&other)
    {
        std::swap(int_no_copy, other.int_no_copy);
        return *this;
    }

    int int_no_copy;
};

/********************************************************
 *                    FirstCombiner
 ********************************************************/
/**
 * FirstCombiner
*/
template <typename T>
class FirstCombinerBase
{
    template <typename U>
    void combine(U item)
    {
        // do nothing
    }

    void result()
    {
        // do nothing
    }
};

template <typename T>
class FirstCombiner : public FirstCombinerBase<T>
{
public:
    using result_type = T;

    template <typename U>
    void combine(U item)
    {
        if (m_isResultEmpty) {
            m_firstResult = std::forward<U>(item);
            m_isResultEmpty = false;
        }
    }

    result_type result()
    {
        return std::move(m_firstResult);
    }


private:
    result_type m_firstResult;
    bool m_isResultEmpty = true;
};

// type void
template <>
class FirstCombiner<void> : public FirstCombinerBase<void>
{
public:
    using result_type = void;
};



/********************************************************
 *                Callback functions
 ********************************************************/

void callback_1(int &res)
{
    res = 1;
}

void callback_2(int &counter)
{
    ++counter;
}

int callback_3()
{
    return 1;
}

int callback_4()
{
    return 2;
}

int callback_5()
{
    return 3;
}

char callback_6()
{
    return 'a';
}

char callback_7()
{
    return 'b';
}

int callback_8()
{
    return 97;
}

char callback_9()
{
    return 98;
}

std::unique_ptr<int> callback_10()
{
    auto ptr = std::make_unique<int>(10);
    return ptr;
}

std::unique_ptr<int> callback_11()
{
    auto ptr = std::make_unique<int>(11);
    return ptr;
}

int callback_12(NoCopy nc)
{
    int value = nc.int_no_copy * 5;
    return value;
}

int callback_13(NoCopy nc)
{
    int value = nc.int_no_copy * 6;
    return value;
}

int callback_14(int i1, int i2){
    return i1+i2;
}


/********************************************************
 *                      Tests
 ********************************************************/

/**
 * Connect slots tests
 */

// ID uniqueness
TEST(connectSlot, UniqueID)
{
    sig::Signal<void()> signal;

    std::size_t id1 = signal.connectSlot([]() {});
    std::size_t id2 = signal.connectSlot([]() {});
    std::size_t id3 = signal.connectSlot([]() {});

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id3, id1);
}

// Connect function
TEST(connectSlot, ConnectFunction)
{
    sig::Signal<void(int &)> signal;
    int res = 0;
    signal.connectSlot(&callback_1);
    signal.emitSignal(res);
    EXPECT_EQ(res, 1);
}

// Connect same function
TEST(connectSlot, ConnectSameFunctionTwice)
{
    sig::Signal<void(int &)> signal;
    int counter = 0;
    signal.connectSlot(&callback_2);
    signal.connectSlot(&callback_2);
    signal.emitSignal(counter);
    EXPECT_EQ(counter, 2);
}

// Connect lambda
TEST(connectSlot, connectLambda)
{
    sig::Signal<void(int)> signal;
    int res = 0;
    signal.connectSlot([&res](int x)
                       { res = x; });
    signal.emitSignal(20);
    EXPECT_EQ(res, 20);
}

/**
 * Disconnect slots tests
 */

// Disconnect one slot
TEST(disconnectSlot, One)
{
    sig::Signal<void(int)> signal;

    int res1 = 0;
    signal.connectSlot([&res1](int i){ res1 = i; });
    int res2 = 0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2 = i * 2; });

    signal.emitSignal(2);
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 4);

    signal.disconnectSlot(id2);

    signal.emitSignal(3);
    EXPECT_EQ(res1, 3);
    EXPECT_EQ(res2, 4);
}

// Disconnect all slots
TEST(disconnectSlot, All)
{
    sig::Signal<void(char)> signal;

    int res1 = 0;
    std::size_t id1 = signal.connectSlot([&res1](int i){ res1 = i; });
    int res2 = 0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2 = i * 2; });

    signal.emitSignal(2);
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 4);

    signal.disconnectSlot(id1);
    signal.disconnectSlot(id2);

    signal.emitSignal(3);
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 4);
}

// Disconnect one slot after another
TEST(disconnectSlot, KeepOtherAccess)
{
    sig::Signal<void(int)> signal;

    int res1 = 0;
    signal.connectSlot([&res1](int i){ res1 = i; });
    int res2 = 0;
    std::size_t id2 = signal.connectSlot([&res2](int i){ res2 = i * 2; });
    int res3 = 0;
    signal.connectSlot([&res3](int i){ res3 = i * 3; });

    signal.emitSignal(2);
    EXPECT_EQ(res1, 2);
    EXPECT_EQ(res2, 4);
    EXPECT_EQ(res3, 6);

    signal.disconnectSlot(id2);

    signal.emitSignal(3);
    EXPECT_EQ(res1, 3);
    EXPECT_EQ(res2, 4);
    EXPECT_EQ(res3, 9);
}

/**
 * Emit signal tests
 */

// Emit without slot connected and compile
TEST(emitSignal, NoSlotsConnected)
{
    sig::Signal<void()> signal;
    signal.emitSignal();
}

// Passing arguments by reference
TEST(emitSignal, RefArgument)
{
    sig::Signal<void(int &)> signal;
    signal.connectSlot([](int &x){ x = x * 2; });

    int arg = 42;
    signal.emitSignal(arg);

    EXPECT_EQ(arg, 84);
}

// Multiple args types
TEST(emitSignal, multipleArgsType)
{
    sig::Signal<void(int, char)> signal;

    std::vector<int> ints;
    std::vector<char> chars;

    signal.connectSlot([&ints, &chars](int i, char c){ ints.push_back(i); chars.push_back(c); });
    signal.connectSlot([&ints, &chars](int i, char c){ ints.push_back(i*2); chars.push_back(c+1); });

    signal.emitSignal(1, 'a');

    EXPECT_EQ(ints.size(), 2u);
    EXPECT_EQ(chars.size(), 2u);

    EXPECT_EQ(ints[0], 1);
    EXPECT_EQ(ints[1], 2);

    EXPECT_EQ(chars[0], 'a');
    EXPECT_EQ(chars[1], 'b');
}

// Vector of ints as args types
TEST(emitSignal, LambdaWithVector)
{
    sig::Signal<void(std::vector<int>)> signal;
    std::vector<int> v = {1, 2, 3};
    int sum = 0;

    signal.connectSlot([&sum](std::vector<int> vec)
    {
        for (int i : vec) {
            sum += i;
        } 
    });

    signal.emitSignal(v);
    EXPECT_EQ(sum, 6);
}

/**
 * Project statement test
 */
void callback(int param)
{
    printf("Hello %i\n", param);
}

TEST(emitSignal, TestSubject)
{
    // define a signal for functions that takes an int
    // and returns void
    sig::Signal<void(int)> signal;
    // connect a simple function
    signal.connectSlot(callback);
    // connect a lambda function
    int res = 0;
    signal.connectSlot([&res](int x)
                    { res = x; });
    // emit the signal
    signal.emitSignal(1);
    // here res equals 1 and "Hello 1" is printed on stdout
    EXPECT_EQ(res, 1);
}


/**
 * LastCombiner tests
 */

// Connect one slot and emit signal
TEST(lastCombiner, OneSlot)
{
    sig::Signal<int(), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

// Connect many slots and keep the result of the last only
TEST(lastCombiner, OnlyLast)
{
    sig::Signal<int(), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_5);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 3);
}

// Connect no slot and can compile
TEST(lastCombiner, NoSlot)
{
    sig::Signal<int(int), sig::LastCombiner<int>> signal;
    signal.emitSignal(3);
}

// Connect two same slots and keep the last
TEST(lastCombiner, LastOneSameToFirst)
{
    sig::Signal<int(), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

// Connect two slots with different output types
TEST(lastCombiner, DifferentOutputType)
{
    sig::Signal<int(), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_6);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 97);
}

// LastCombiner with void type
TEST(lastCombiner, returnTypeVoid)
{
    sig::Signal<void(int), sig::LastCombiner<void>> signal;
    int res = 0;
    signal.connectSlot([&res](int x){ res = x + 1; });
    signal.emitSignal(1);
    EXPECT_EQ(res, 2);
}

// Tests to mix different compatible types between signal and slots
TEST(lastCombiner, ReturnIntWithCharSlot)
{
    sig::Signal<int(), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_6);
    signal.connectSlot(&callback_7);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 98);
}

TEST(lastCombiner, ReturnCharWithIntSlot)
{
    sig::Signal<char(), sig::LastCombiner<char>> signal;
    signal.connectSlot(&callback_8);
    signal.connectSlot(&callback_9);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'b');
}

TEST(lastCombiner, CharCombinerWithIntSignal)
{
    sig::Signal<int(), sig::LastCombiner<char>> signal;
    signal.connectSlot(&callback_8);
    signal.connectSlot(&callback_9);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'b');
}

TEST(lastCombiner, IntSignalWithCharSlot)
{
    sig::Signal<int(), sig::LastCombiner<char>> signal;
    signal.connectSlot(&callback_6);
    signal.connectSlot(&callback_7);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'b');
}

TEST(lastCombiner, IntsWithLongs)
{
    sig::Signal<int(long, long), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_14);
    auto res = signal.emitSignal(1.2,4.3);
    EXPECT_EQ(res, 5);
}

/**
 * VectorCombiner tests
 */

// Connect one slot and emit signal
TEST(vectorCombiner, OneSlot)
{
    sig::Signal<int(), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_3);

    auto res = signal.emitSignal();
    std::vector<int> expect = {1};
    EXPECT_EQ(res, expect);
}

// Connect many slots and keep them all
TEST(vectorCombiner, MultipleSlot)
{
    sig::Signal<int(), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_5);

    std::vector<int> expect = {1, 2, 3};
    std::vector<int> res = signal.emitSignal();

    EXPECT_EQ(res, expect);
}

// Connect no slot and returns an empty vector
TEST(vectorCombiner, NoSlot)
{
    sig::Signal<int(), sig::VectorCombiner<int>> signal;
    auto res = signal.emitSignal();
    std::vector<int> expect;
    EXPECT_EQ(res, expect);
}

// Connect two same slots and return its two times
TEST(vectorCombiner, SameTwoTime)
{
    sig::Signal<int(), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    std::vector<int> expect = {1, 2, 1};
    EXPECT_EQ(res, expect);
}

// VectorCombiner with void type
TEST(vectorCombiner, returnTypeVoid)
{
    sig::Signal<void(int), sig::VectorCombiner<void>> signal;
    std::vector<int> res;
    signal.connectSlot([&res](int x)
                    { res.push_back(x); });
    signal.emitSignal(1);
    signal.emitSignal(2);
    signal.emitSignal(3);

    EXPECT_EQ(res.size(), 3u);
    EXPECT_EQ(res[0], 1);
    EXPECT_EQ(res[1], 2);
    EXPECT_EQ(res[2], 3);
}

/**
 * No-copy tests
 */

TEST(noCopy, OutputTypeVectorCombiner)
{
    sig::Signal<std::unique_ptr<int>(), sig::VectorCombiner<std::unique_ptr<int>>> signal;
    signal.connectSlot(&callback_10);
    signal.connectSlot(&callback_11);

    std::vector<std::unique_ptr<int>> expect;
    expect.push_back(std::make_unique<int>(10));
    expect.push_back(std::make_unique<int>(11));
    std::vector<std::unique_ptr<int>> res = signal.emitSignal();

    EXPECT_EQ(*res[0].get(), *expect[0].get());
    EXPECT_EQ(*res[1].get(), *expect[1].get());
}

TEST(noCopy, InputTypeVectorCombiner)
{
    sig::Signal<int(NoCopy), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_12);
    signal.connectSlot(&callback_13);

    NoCopy nc(4);
    std::vector<int> res = signal.emitSignal(std::move(nc));
    std::vector<int> expect = {20, 24};

    EXPECT_EQ(res, expect);
}

TEST(noCopy, OutputTypeVoidVectorCombiner)
{
    sig::Signal<void(NoCopy), sig::VectorCombiner<void>> signal;
    std::vector<NoCopy> res;
    signal.connectSlot([&res](NoCopy nc){ nc.int_no_copy = nc.int_no_copy + 1;
                                          res.push_back(std::move(nc)); });
    signal.connectSlot([&res](NoCopy nc){ nc.int_no_copy = nc.int_no_copy * 2;
                                          res.push_back(std::move(nc)); });
    signal.emitSignal(2);
    EXPECT_EQ(res.size(), 2u);
    EXPECT_EQ(res[0].int_no_copy, 3);
    EXPECT_EQ(res[1].int_no_copy, 4);
}

TEST(noCopy, OutputTypeLastCombiner)
{
    sig::Signal<std::unique_ptr<int>(), sig::LastCombiner<std::unique_ptr<int>>> signal;
    signal.connectSlot(&callback_10);
    signal.connectSlot(&callback_11);

    std::vector<std::unique_ptr<int>> expect;
    expect.push_back(std::make_unique<int>(10));
    expect.push_back(std::make_unique<int>(11));
    std::unique_ptr<int> res = signal.emitSignal();

    EXPECT_EQ(*res.get(), *expect[1].get());
}

TEST(noCopy, InputTypeLastCombiner)
{
    sig::Signal<int(NoCopy), sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_12);
    signal.connectSlot(&callback_13);

    int expect = 30;
    int res = signal.emitSignal(NoCopy(5));

    EXPECT_EQ(res, expect);
}

TEST(noCopy, OutputTypeVoidLastCombiner)
{
    sig::Signal<void(NoCopy), sig::LastCombiner<void>> signal;
    NoCopy res;
    signal.connectSlot([&res](NoCopy nc){ nc.int_no_copy = nc.int_no_copy + 1;
                                          res = std::move(nc); });
    signal.connectSlot([&res](NoCopy nc){ nc.int_no_copy = nc.int_no_copy * 2;
                                          res = std::move(nc); });
    signal.emitSignal(NoCopy(4));
    EXPECT_EQ(res.int_no_copy, 8);
}

TEST(noCopy, OutputTypeDiscardCombiner)
{
    sig::Signal<void(NoCopy &)> signal;
    std::vector<int> int_vector;

    signal.connectSlot([&int_vector](NoCopy &nc){ int_vector.push_back(nc.int_no_copy + 1); });
    signal.connectSlot([&int_vector](NoCopy &nc){ int_vector.push_back(nc.int_no_copy + 2); });

    NoCopy nc(3);
    signal.emitSignal(nc);

    EXPECT_EQ(int_vector.size(), 2u);
    EXPECT_EQ(int_vector[0], 4);
    EXPECT_EQ(int_vector[1], 5);
}

/**
 * Multiple signals tests
*/
TEST(multipleSignals, Default)
{
    sig::Signal<int(), sig::LastCombiner<int>> signal1;
    sig::Signal<int(), sig::LastCombiner<int>> signal2;
    signal1.connectSlot(&callback_6);
    signal2.connectSlot(&callback_7);
    auto res1 = signal1.emitSignal();
    auto res2 = signal2.emitSignal();
    EXPECT_EQ(res1, 97);
    EXPECT_EQ(res2, 98);
}

/**
 * Own combiner : FirstCombiner tests
*/

// Connect one slot and emit signal
TEST(firstCombiner, OneSlot)
{
    sig::Signal<int(), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

// Connect many slots and keep the result of the first only
TEST(firstCombiner, OnlyFirst)
{
    sig::Signal<int(), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_5);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

// Connect no slot and can compile
TEST(firstCombiner, NoSlot)
{
    sig::Signal<int(int), FirstCombiner<int>> signal;
    signal.emitSignal(3);
}

// Connect two same slots and keep the first
TEST(firstCombiner, LastOneSameToFirst)
{
    sig::Signal<int(), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

// Connect two same slots with different output types
TEST(firstCombiner, DifferentOutputType)
{
    sig::Signal<int(), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_6);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

// FirstCombiner with void type
TEST(firstCombiner, returnTypeVoid)
{
    sig::Signal<void(int), FirstCombiner<void>> signal;
    int res = 0;
    signal.connectSlot([&res](int x){ res = x + 1; });
    signal.emitSignal(1);
    EXPECT_EQ(res, 2);
}

// Tests the mix of different compatible types between signal and slots
TEST(firstCombiner, ReturnIntWithCharSlot)
{
    sig::Signal<int(), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_6);
    signal.connectSlot(&callback_7);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 97);
}

TEST(firstCombiner, ReturnCharWithIntSlot)
{
    sig::Signal<char(), FirstCombiner<char>> signal;
    signal.connectSlot(&callback_8);
    signal.connectSlot(&callback_9);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'a');
}

TEST(firstCombiner, CharCombinerWithIntSignal)
{
    sig::Signal<int(), FirstCombiner<char>> signal;
    signal.connectSlot(&callback_8);
    signal.connectSlot(&callback_9);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'a');
}

TEST(firstCombiner, IntSignalWithCharSlot)
{
    sig::Signal<int(), FirstCombiner<char>> signal;
    signal.connectSlot(&callback_6);
    signal.connectSlot(&callback_7);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'a');
}

TEST(firstCombiner, IntWithLong)
{
    sig::Signal<int(long, long), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_14);
    auto res = signal.emitSignal(1.2,4.3);
    EXPECT_EQ(res, 5);
}

//Tests no-copy
TEST(noCopy, OutputTypeFirstCombiner)
{
    sig::Signal<std::unique_ptr<int>(), FirstCombiner<std::unique_ptr<int>>> signal;
    signal.connectSlot(&callback_10);
    signal.connectSlot(&callback_11);

    std::vector<std::unique_ptr<int>> expect;
    expect.push_back(std::make_unique<int>(10));
    expect.push_back(std::make_unique<int>(11));
    std::unique_ptr<int> res = signal.emitSignal();

    EXPECT_EQ(*res.get(), *expect[0].get());
}

TEST(noCopy, InputTypeFirstCombiner)
{
    sig::Signal<int(NoCopy), FirstCombiner<int>> signal;
    signal.connectSlot(&callback_12);
    signal.connectSlot(&callback_13);

    int expect = 25;
    int res = signal.emitSignal(NoCopy(5));

    EXPECT_EQ(res, expect);
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
