#include "Signal.h"

#include <gtest/gtest.h>
#include <vector>


void callback_1(int& res)
{
    res = 1;
}

void callback_2(int& counter) {
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

/**
 * Connect slots tests
*/

//ID uniqueness test
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

//Connect function test

TEST(connectSlot, ConnectFunction)
{
    sig::Signal<void(int&)> signal;
    int res = 0;
    signal.connectSlot(&callback_1);
    signal.emitSignal(res);
    EXPECT_EQ(res, 1);
}


//Connect same function test


TEST(connectSlot, ConnectSameFunctionTwice) {
    sig::Signal<void(int&)> signal;
    int counter = 0;
    signal.connectSlot(&callback_2);
    signal.connectSlot(&callback_2);
    signal.emitSignal(counter);
    EXPECT_EQ(counter, 2);
}

//Connect lambda test
TEST(connectSlot, connectLambda)
{
    sig::Signal<void(int)> signal;
    int res = 0;
    signal.connectSlot([&res](int x){ res = x; });
    signal.emitSignal(20);
    EXPECT_EQ(res, 20);
}

//Connect object test
class MyClass
{
public:
    void callback_class(int signal) { res = signal + 2; }
    int res = 0;
};

TEST(connectSlot, ObjectSlot)
{
    sig::Signal<void(int)> sig;
    MyClass obj;
    sig.connectSlot(std::bind(&MyClass::callback_class, &obj, std::placeholders::_1));
    sig.emitSignal(4);
    EXPECT_EQ(obj.res, 6);
}


/**
 * Disconnect slots tests
*/

//Disconnect one slot
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

//Disconnect all slots
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

//Disconnecting one slot after another test
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
 * Emit signal tests
*/

//Emit without slot connected test and compile
TEST(emitSignal, NoSlotsConnected)
{
    sig::Signal<void()> sig;
    sig.emitSignal();
}


//Passing arguments by reference test
TEST(emitSignal, RefArgument)
{
    sig::Signal<void(int&)> sig;
    sig.connectSlot([](int& x) { x = x*2;});

    int arg = 42;
    sig.emitSignal(arg);

    EXPECT_EQ(arg, 84);
}

//Multiple args type
TEST(emitSignal, multipleArgsType) {
    sig::Signal<void(int,char)> signal;

    std::vector<int> ints;
    std::vector<char> chars;

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

//Vector args type
TEST(emitSignal, LambdaWithVector)
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


/**
 * LastCombiner tests
*/

//Connect one slot and emit signal
TEST(lastCombiner, OneSlot)
{
    sig::Signal<int() , sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}



//Connect many slots and keep the result of the last only
TEST(lastCombiner, OnlyLast)
{
    sig::Signal<int() , sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_5);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 3);
}

//Connect no slot and can compile
TEST(lastCombiner, NoSlot)
{
    sig::Signal<int(int) , sig::LastCombiner<int>> signal;
    signal.emitSignal(3);
}

//Connect two same slots and keep the last
TEST(lastCombiner, LastOneSameToFirst)
{
    sig::Signal<int() , sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 1);
}

//Connect two same slots with different output types
TEST(lastCombiner, DifferentOutputType)
{
    sig::Signal<int() , sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_6);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 97);
}


//LastCombiner with void type

TEST(LastCombiner, returnTypeVoid)
{
    sig::Signal<void(int), sig::LastCombiner<void>> sig;
    int res = 0;
    sig.connectSlot([&res](int x){ res = x+1; });
    sig.emitSignal(1);
    EXPECT_EQ(res,2);
}


//Tests the mix of different compatible types between signal and slots
TEST(lastCombiner, ReturnIntWithCharSlot)
{
    sig::Signal<int() , sig::LastCombiner<int>> signal;
    signal.connectSlot(&callback_6);
    signal.connectSlot(&callback_7);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 98);
}

TEST(lastCombiner, ReturnCharWithIntSlot)
{
    sig::Signal<char() , sig::LastCombiner<char>> signal;
    signal.connectSlot(&callback_8);
    signal.connectSlot(&callback_9);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'b');
}

TEST(lastCombiner, CharCombinerWithIntSignal)
{
    sig::Signal<int() , sig::LastCombiner<char>> signal;
    signal.connectSlot(&callback_8);
    signal.connectSlot(&callback_9);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'b');
}

TEST(lastCombiner, IntSignalWithCharSlot)
{
    sig::Signal<int() , sig::LastCombiner<char>> signal;
    signal.connectSlot(&callback_6);
    signal.connectSlot(&callback_7);
    auto res = signal.emitSignal();
    EXPECT_EQ(res, 'b');
}

/**
 * VectorCombiner tests
*/

//Connect one slot and emit signal
TEST(vectorCombiner, OneSlot)
{
    sig::Signal<int() , sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_3);

    auto res = signal.emitSignal();
    std::vector<int> expect = {1};
    EXPECT_EQ(res, expect);
}

//Connect many slots and keep them all
TEST(vectorCombiner, MultipleSlot)
{
    sig::Signal<int(), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_5);

    std::vector<int> expect = {1, 2, 3};
    std::vector<int> actual_results = signal.emitSignal();

    EXPECT_EQ(actual_results, expect);
}

//Connect no slot and returns an empty vector
TEST(vectorCombiner, NoSlot)
{
    sig::Signal<int() , sig::VectorCombiner<int>> signal;
    auto res = signal.emitSignal();
    std::vector<int> expect;
    EXPECT_EQ(res, expect);
}

//Connect two same slots and return its two times
TEST(vectorCombiner, SameTwoTime)
{
    sig::Signal<int() , sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_3);
    signal.connectSlot(&callback_4);
    signal.connectSlot(&callback_3);
    auto res = signal.emitSignal();
    std::vector<int> expect = {1, 2, 1};
    EXPECT_EQ(res, expect);
}

//VectorCombiner with void type
TEST(VectorCombiner, returnTypeVoid)
{
    sig::Signal<void(int), sig::VectorCombiner<void>> sig;
    std::vector<int> res;
    sig.connectSlot([&res](int x){ res.push_back(x); });
    sig.emitSignal(1);
    sig.emitSignal(2);
    sig.emitSignal(3);

    EXPECT_EQ(res.size(), 3u);
    EXPECT_EQ(res[0], 1);
    EXPECT_EQ(res[1], 2);
    EXPECT_EQ(res[2], 3);
}


std::unique_ptr<int> callback_10(){
    auto ptr = std::make_unique<int>(10);
    return ptr;
}

std::unique_ptr<int> callback_11(){
    auto ptr = std::make_unique<int>(11);
    return ptr;
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
    std::vector<std::unique_ptr<int>> actual_results = signal.emitSignal();

    EXPECT_EQ(*actual_results[0].get(), *expect[0].get());
    EXPECT_EQ(*actual_results[1].get(), *expect[1].get());
}

int callback_12(std::unique_ptr<int> ptr){
    int value = *ptr.get()*5;
    return value;
}

int callback_13(std::unique_ptr<int> ptr){
    int value = *ptr.get()*6; //deja déplacé ca ne peux pas marcher
    return value;
} 

//Faire sa propre structure avec un int et interdire la copie pour tester

TEST(noCopy, InputTypeLastCombiner)
{
    sig::Signal<int(std::unique_ptr<int>), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_12);
    signal.connectSlot(&callback_13);

    auto ptr = std::make_unique<int>(5);
    std::vector<int> actual_results = signal.emitSignal(std::move(ptr));

    //std::vector<int> expect = {1, 2, 3};

    //EXPECT_EQ(actual_results, expect);
}


TEST(noCopy, OutputTypeLastCombiner)
{
    sig::Signal<std::unique_ptr<int>(), sig::LastCombiner<std::unique_ptr<int>>> signal;
    signal.connectSlot(&callback_10);
    signal.connectSlot(&callback_11);

    std::vector<std::unique_ptr<int>> expect;
    expect.push_back(std::make_unique<int>(10));
    expect.push_back(std::make_unique<int>(11));
    std::unique_ptr<int> actual_results = signal.emitSignal();

    EXPECT_EQ(*actual_results.get(), *expect[1].get());
}

/*TEST(noCopy, InputTypeVectorCombiner)
{
    sig::Signal<int(std::unique_ptr<int>), sig::VectorCombiner<int>> signal;
    signal.connectSlot(&callback_12);
    signal.connectSlot(&callback_13);

    auto ptr = std::make_unique<int>(5);

    std::vector<int> expect = {1, 2, 3};
    std::vector<int> actual_results = signal.emitSignal(ptr);

    EXPECT_EQ(actual_results, expect);
}*/

//no-copy void


void print(int x) {
    std::cout << "Hello " << x << std::endl;
}

void square(int x) {
    std::cout << "Square of " << x << " is " << x*x << std::endl;
}

TEST(VectorCombiner, test)
{
    // test discard combiner
    sig::Signal<void(int), sig::DiscardCombiner> sig1;
    sig1.connectSlot(print);
    sig1.emitSignal(1); // "Hello 1"

    // test last combiner
    sig::Signal<void(int), sig::LastCombiner<void>> sig2;
    sig2.connectSlot(print);
    sig2.connectSlot(square);
    sig2.emitSignal(2); // "Hello 2"  "Square of 2 is 4"

    // test vector combiner
    sig::Signal<void(int), sig::VectorCombiner<void>> sig3;
    sig3.connectSlot(square);
    sig3.connectSlot(print);
    sig3.emitSignal(3); //"Square of 3 is 9"  "Hello 3"

    //add tests

}




















//Eviter copie item (test uniptr)

//passer int a fonction long
//tester avec fonction qui renvoie mauvais trucs
//test différents signaux en meme temps
//différentes args 
//test args (char)...
//faire int(int)
//

//doit-on faire un destructeur ?


/**
 * passing pointer test
*/


/**
 * Priority of slots test
*/




//tests multiple et bon ordre


int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
