/***************************************************************************
*   Copyright 2012 - 2013 Advanced Micro Devices, Inc.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.

***************************************************************************/

#define TEST_DOUBLE 0
#define TEST_DEVICE_VECTOR 1
#define TEST_CPU_DEVICE 0
#include "common/stdafx.h"

#include "bolt/amp/count.h"
#include "bolt/amp/functional.h"
#include "bolt/miniDump.h"
#include "bolt/unicode.h"

#include <gtest/gtest.h>
#include <boost/shared_array.hpp>
#include <array>
#include <algorithm>
#include "bolt/amp/device_vector.h"

class testCountIfFloatWithStdVector: public ::testing::TestWithParam<int>{
protected:
	int aSize;
public:
	testCountIfFloatWithStdVector():aSize(GetParam()){
	}
};


template<typename T>
// Functor for range checking.
struct InRange {
	InRange (T low, T high) restrict (amp,cpu) {
		_low=low;
		_high=high;
	};

	bool operator()(const T& value) const restrict (amp,cpu) {
		//printf("Val=%4.1f, Range:%4.1f ... %4.1f\n", value, _low, _high);
		return (value >= _low) && (value <= _high) ;
	};

	T _low;
	T _high;
};


TEST_P (testCountIfFloatWithStdVector, countFloatValueInRange)
{
	std::vector<float> A(aSize);
	std::vector<float> B(aSize);

	for (int i=0; i < aSize; i++) {
		A[i] = static_cast<float> (i+1);
		B[i] = A[i];
	};

	size_t stdCount = std::count_if (A.begin(), A.end(), InRange<float>(6,10)) ;
	size_t boltCount = bolt::amp::count_if (B.begin(), B.end(), InRange<float>(6,10)) ;

	EXPECT_EQ (stdCount, boltCount)<<"Failed as: STD Count = "<<stdCount<<"\nBolt Count = "<<boltCount<<std::endl;
	std::cout<<"STD Count = "<<stdCount<<std::endl<<"bolt Count = "<<boltCount<<std::endl;
}

TEST_P (testCountIfFloatWithStdVector, countFloatValueInRange2)
{
	std::vector<float> A(aSize);
	bolt::amp::device_vector<float> B(A.begin(), aSize);

	for (int i=0; i < aSize; i++) {
		A[i] = static_cast<float> (i+1);
		B[i] = A[i];
	};

	size_t stdCount = std::count_if (A.begin(), A.end(), InRange<float>(1,10)) ;
	size_t boltCount = bolt::amp::count_if (B.begin(), B.end(), InRange<float>(1,10)) ;

	EXPECT_EQ (stdCount, boltCount)<<"Failed as: STD Count = "<<stdCount<<"\nBolt Count = "<<boltCount<<std::endl;
	std::cout<<"STD Count = "<<stdCount<<std::endl<<"bolt Count = "<<boltCount<<std::endl;
}

class countFloatValueOccuranceStdVect :public ::testing::TestWithParam<int>{
protected:
	int stdVectSize;
public:
	countFloatValueOccuranceStdVect():stdVectSize(GetParam()){}
};

TEST_P(countFloatValueOccuranceStdVect, floatVectSearchWithSameValue){
	std::vector<float> stdVect(stdVectSize);
	bolt::amp::device_vector<float> boltVect(stdVect.begin(),stdVectSize);
	float myFloatValue = 1.23f;
	for (int i =0; i < stdVectSize; i++){
		stdVect[i] = myFloatValue;
		boltVect[i] = stdVect[i];
	}

	bolt::amp::iterator_traits<bolt::amp::device_vector<float>::iterator>::difference_type stdCount = std::count(stdVect.begin(), stdVect.end(), myFloatValue);
	bolt::amp::iterator_traits<bolt::amp::device_vector<float>::iterator>::difference_type boltCount = bolt::amp::count(boltVect.begin(), boltVect.end(), myFloatValue);

	EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
	std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}

TEST_P(countFloatValueOccuranceStdVect, floatVectSearchWithSameValue2){
	std::vector<float> stdVect(stdVectSize);
	bolt::amp::device_vector<float> boltVect(stdVect.begin(), stdVectSize);
	float myFloatValue2 = 9.87f;
	for (int i =0; i < stdVectSize; i++){
		stdVect[i] = myFloatValue2;
		boltVect[i] = stdVect[i];
	}

	bolt::amp::iterator_traits<bolt::amp::device_vector<float>::iterator>::difference_type stdCount = std::count(stdVect.begin(), stdVect.end(), myFloatValue2);
	bolt::amp::iterator_traits<bolt::amp::device_vector<float>::iterator>::difference_type boltCount = bolt::amp::count(boltVect.begin(), boltVect.end(), myFloatValue2);

	EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
	std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}

INSTANTIATE_TEST_CASE_P (useStdVectWithFloatValues, countFloatValueOccuranceStdVect, ::testing::Values(1, 100, 1000, 10000, 100000));


//Test case id: 7 (Failed)
class countDoubleValueUsedASKeyInStdVect :public ::testing::TestWithParam<int>{
protected:
	int stdVectSize;
public:
	countDoubleValueUsedASKeyInStdVect():stdVectSize(GetParam()){}
};


TEST_P(countDoubleValueUsedASKeyInStdVect, doubleVectSearchWithSameValue){
	std::vector<double> stdVect(stdVectSize);
	bolt::amp::device_vector<double> boltVect(stdVect.begin(),stdVectSize);
	double myDoubleValueAsKeyValue = 1.23456789l;

	for (int i =0; i < stdVectSize; i++){
		stdVect[i] = myDoubleValueAsKeyValue;
		boltVect[i] = stdVect[i];
	}

	bolt::amp::iterator_traits<bolt::amp::device_vector<double>::iterator>::difference_type stdCount = std::count(stdVect.begin(), stdVect.end(), myDoubleValueAsKeyValue);
	bolt::amp::iterator_traits<bolt::amp::device_vector<double>::iterator>::difference_type boltCount = bolt::amp::count(boltVect.begin(), boltVect.end(), myDoubleValueAsKeyValue);

	EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
	std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}

TEST_P(countDoubleValueUsedASKeyInStdVect, doubleVectSearchWithSameValue2){
	std::vector<double> stdVect(stdVectSize);
  bolt::amp::device_vector<double> boltVect(stdVect.begin(), stdVectSize);
	double myDoubleValueAsKeyValue2 = 9.876543210123456789l;

	for (int i =0; i < stdVectSize; i++){
		stdVect[i] = myDoubleValueAsKeyValue2;
		boltVect[i] = stdVect[i];
	}

	bolt::amp::iterator_traits<bolt::amp::device_vector<double>::iterator>::difference_type stdCount = std::count(stdVect.begin(), stdVect.end(), myDoubleValueAsKeyValue2);
	bolt::amp::iterator_traits<bolt::amp::device_vector<double>::iterator>::difference_type boltCount = bolt::amp::count(boltVect.begin(), boltVect.end(), myDoubleValueAsKeyValue2);

	EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
	std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}

//test case: 1, test: 1

TEST (testCountIf, intBtwRange)
{
	int aSize = 1024;
	std::vector<int> A(aSize);

	for (int i=0; i < aSize; i++) {
		A[i] = rand() % 10 + 1;
	}
  bolt::amp::device_vector< int > dA(A.begin(), aSize);
  int intVal = 1;

	bolt::amp::iterator_traits<bolt::amp::device_vector<int>::iterator>::difference_type stdInRangeCount = std::count( A.begin(), A.end(), intVal ) ;
	bolt::amp::iterator_traits<bolt::amp::device_vector<int>::iterator>::difference_type boltInRangeCount = bolt::amp::count( dA.begin(), dA.end(), intVal ) ;

	EXPECT_EQ(stdInRangeCount, boltInRangeCount);
}

//
INSTANTIATE_TEST_CASE_P (useStdVectWithDoubleValues, countDoubleValueUsedASKeyInStdVect, ::testing::Values(1, 100, 1000, 10000, 100000));
INSTANTIATE_TEST_CASE_P (serialFloatValueWithStdVector, testCountIfFloatWithStdVector, ::testing::Values(10, 100, 1000, 10000, 100000));
//

struct UDD { 
    int a; 
    int b;

    bool operator() (const UDD& lhs, const UDD& rhs) restrict(amp,cpu) { 
        return ((lhs.a+lhs.b) > (rhs.a+rhs.b));
    } 
    bool operator < (const UDD& other) const restrict(amp,cpu) { 
        return ((a+b) < (other.a+other.b));
    }
    bool operator > (const UDD& other) const restrict(amp,cpu) { 
        return ((a+b) > (other.a+other.b));
    }
    bool operator == (const UDD& other) const restrict(amp,cpu) { 
        return ((a+b) == (other.a+other.b));
    }


    bool operator() (const int &x) restrict(amp,cpu) {
        return (x == a || x == b);
    }

    UDD operator + (const UDD &rhs) const restrict(amp,cpu) {
                UDD tmp = *this;
                tmp.a = tmp.a + rhs.a;
                tmp.b = tmp.b + rhs.b;
                return tmp;
    }
    
    UDD() 
        : a(0),b(0) { } 
    UDD(int _in) 
        : a(_in), b(_in +1)  { } 
        
}; 

TEST(countFloatValueOccuranceStdVect, MulticoreCountUDDTBB){
    const int aSize = 1<<21;
    std::vector<UDD> stdInput(aSize);
    std::vector<UDD> tbbInput(aSize);

    //bolt::amp::device_vector<int> stdInput(aSize);
    //bolt::amp::device_vector<int> tbbInput(aSize);


    UDD myUDD;
    myUDD.a = 3;
    myUDD.b = 5;


    for (int i=0; i < aSize; i++) {
       stdInput[i].a = rand()%10;
       stdInput[i].b = rand()%10;
       tbbInput[i].a = stdInput[i].a;
       tbbInput[i].b = stdInput[i].b;
    }

    bolt::amp::control ctl = bolt::amp::control::getDefault();
    ctl.setForceRunMode(bolt::amp::control::MultiCoreCpu);
    size_t stdCount = std::count(stdInput.begin(), stdInput.end(), myUDD);
    size_t boltCount = bolt::amp::count(ctl, tbbInput.begin(), tbbInput.end(), myUDD);

    EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
    std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}

TEST(countFloatValueOccuranceStdVect,DeviceCountUDDTBB){
    const int aSize = 1<<21;
    std::vector<UDD> stdInput(aSize);
    
    UDD myUDD;
    myUDD.a = 3;
    myUDD.b = 5;
    
    for (int i=0; i < aSize; i++) {
       stdInput[i].a = rand()%10;
       stdInput[i].b = rand()%10;
    }

    bolt::amp::device_vector<UDD> tbbInput(stdInput.begin(),stdInput.end());
    
    bolt::amp::control ctl = bolt::amp::control::getDefault();
    size_t stdCount = std::count(stdInput.begin(), stdInput.end(), myUDD);
    size_t boltCount = bolt::amp::count(ctl, tbbInput.begin(), tbbInput.end(), myUDD);

    EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
    std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}
TEST(countFloatValueOccuranceStdVect, STDCountUDDTBB){
    const int aSize = 1<<21;
    std::vector<UDD> stdInput(aSize);
    std::vector<UDD> tbbInput(aSize);

    UDD myUDD;
    myUDD.a = 3;
    myUDD.b = 5;


    for (int i=0; i < aSize; i++) {
       stdInput[i].a = rand()%10;
       stdInput[i].b = rand()%10;
       tbbInput[i].a = stdInput[i].a;
       tbbInput[i].b = stdInput[i].b;
    }

    bolt::amp::control ctl = bolt::amp::control::getDefault();
    ctl.setForceRunMode(bolt::amp::control::MultiCoreCpu);
    size_t stdCount = std::count(stdInput.begin(), stdInput.end(), myUDD);
    size_t boltCount = bolt::amp::count(ctl, tbbInput.begin(), tbbInput.end(), myUDD);

    EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
    std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}


TEST(countFloatValueOccuranceStdVect, MulticoreCountifIntTBB){
    const int aSize = 1<<24;
    std::vector<int> stdInput(aSize);

    int myintValue = 2;

    for (int i=0; i < aSize; i++) {
    stdInput[i] = rand() % 10 + 1;
    //tbbInput[i] = stdInput[i];
    }
    bolt::amp::device_vector<int> tbbInput(stdInput.begin(),stdInput.end());


    bolt::amp::control ctl = bolt::amp::control::getDefault();
    ctl.setForceRunMode(bolt::amp::control::SerialCpu);
    size_t stdCount = std::count_if(stdInput.begin(), stdInput.end(), InRange<int>(2,10000));
    size_t boltCount = bolt::amp::count_if(ctl, tbbInput.begin(), tbbInput.end(), InRange<int>(2,10000));

  EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
  //std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}

TEST(countIntegerUDD,DeviceCountUDD_EPR_Test){
    const int aSize = 1<<21;
    std::vector<UDD> stdInput(aSize);
    
    UDD myUDD;
    myUDD.a = 3;
    myUDD.b = 5;
    
    for (int i=0; i < aSize; i++) {
       stdInput[i].a = rand()%10;
       stdInput[i].b = rand()%10;
    }

    
    bolt::amp::control ctl = bolt::amp::control::getDefault();
    size_t stdCount = std::count(stdInput.begin(), stdInput.end(), myUDD);
    size_t boltCount = bolt::amp::count( stdInput.begin(), stdInput.end(), myUDD);

    EXPECT_EQ(stdCount, boltCount)<<"Failed as: \nSTD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
    std::cout<<"STD Count = "<<stdCount<<std::endl<<"Bolt Count = "<<boltCount<<std::endl;
}



int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, &argv[ 0 ] );

    //  Register our minidump generating logic
    bolt::miniDumpSingleton::enableMiniDumps( );

    int retVal = RUN_ALL_TESTS( );

    //  Reflection code to inspect how many tests failed in gTest
    ::testing::UnitTest& unitTest = *::testing::UnitTest::GetInstance( );

    unsigned int failedTests = 0;
    for( int i = 0; i < unitTest.total_test_case_count( ); ++i )
    {
        const ::testing::TestCase& testCase = *unitTest.GetTestCase( i );
        for( int j = 0; j < testCase.total_test_count( ); ++j )
        {
            const ::testing::TestInfo& testInfo = *testCase.GetTestInfo( j );
            if( testInfo.result( )->Failed( ) )
                ++failedTests;
        }
    }

    //  Print helpful message at termination if we detect errors, to help users figure out what to do next
    if( failedTests )
    {
        bolt::tout << _T( "\nFailed tests detected in test pass; please run test again with:" ) << std::endl;
        bolt::tout << _T( "\t--gtest_filter=<XXX> to select a specific failing test of interest" ) << std::endl;
        bolt::tout << _T( "\t--gtest_catch_exceptions=0 to generate minidump of failing test, or" ) << std::endl;
        bolt::tout << _T( "\t--gtest_break_on_failure to debug interactively with debugger" ) << std::endl;
        bolt::tout << _T( "\t    (only on googletest assertion failures, not SEH exceptions)" ) << std::endl;
    }
    std::cout << "Test Completed. Press Enter to exit.\n .... ";
    getchar();
	return retVal;
}