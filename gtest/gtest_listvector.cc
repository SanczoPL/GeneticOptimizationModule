#include "gtest_listvector.h"

constexpr auto CONFIG{ "test.json" };
constexpr auto TEST_DATA{ "TestData" };

using ::testing::AtLeast;


namespace gtest_listvector {

	TEST_F(GTest_listvector, test_leaseItem_addItem)
	{
		ListVector<int> listVector(3);
		
		int temp1 = listVector.leaseItem();
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.leaseItem();
		EXPECT_EQ (temp2, 1);
		int temp3 = listVector.leaseItem();
		EXPECT_EQ (temp3, 2);
		int temp4 = listVector.leaseItem();
		EXPECT_EQ (temp4, -1);

		bool b1 = listVector.addItem(temp1, temp1);
		EXPECT_EQ (b1, true);
		bool b2 = listVector.addItem(temp2, temp2);
		EXPECT_EQ (b2, true);
		bool b3 = listVector.addItem(temp3, temp3);
		EXPECT_EQ (b3, true);
		bool b4 = listVector.addItem(temp4, temp4);
		EXPECT_EQ (b4, false);
	}

	TEST_F(GTest_listvector, test_addItem)
	{
		ListVector<int> listVector(3);
		
		int temp1 = listVector.addItem(10);
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.addItem(11);
		EXPECT_EQ (temp2, 1);
		int temp3 = listVector.addItem(12);
		EXPECT_EQ (temp3, 2);
		int temp4 = listVector.addItem(13);
		EXPECT_EQ (temp4, -1);
	}

	TEST_F(GTest_listvector, test_deleteItem)
	{
		ListVector<int> listVector(3);
		
		int temp1 = listVector.addItem(10);
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.addItem(11);
		EXPECT_EQ (temp2, 1);
		int temp3 = listVector.addItem(12);
		EXPECT_EQ (temp3, 2);

		bool b1 = listVector.deleteItem(temp3);
		EXPECT_EQ (b1, true);

		bool b2 = listVector.deleteItem(-2);
		EXPECT_EQ (b2, false);

		bool b3 = listVector.deleteItem(22);
		EXPECT_EQ (b3, false);

		int temp4 = listVector.addItem(13);
		EXPECT_EQ (temp4, 2);
	}

	TEST_F(GTest_listvector, test_deleteItems)
	{
		ListVector<int> listVector(3);
		
		int temp1 = listVector.addItem(10);
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.addItem(11);
		EXPECT_EQ (temp2, 1);
		int temp3 = listVector.addItem(12);
		EXPECT_EQ (temp3, 2);

		bool b1 = listVector.deleteItems();
		EXPECT_EQ (b1, true);

		int temp4 = listVector.addItem(13);
		EXPECT_EQ (temp4, 0);
		int temp5 = listVector.addItem(14);
		EXPECT_EQ (temp5, 1);
		int temp6 = listVector.addItem(15);
		EXPECT_EQ (temp6, 2);
	}

	TEST_F(GTest_listvector, test_operator)
	{
		ListVector<int> listVector(3);
		
		int temp1 = listVector.addItem(10);
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.addItem(11);
		EXPECT_EQ (temp2, 1);
		int temp3 = listVector.addItem(12);
		EXPECT_EQ (temp3, 2);

		EXPECT_EQ (listVector[0], 10);
		EXPECT_EQ (listVector[1], 11);
		EXPECT_EQ (listVector[2], 12);
	}

	TEST_F(GTest_listvector, test_operator2)
	{
		ListVector<int> listVector(3);
		
		int temp1 = listVector.addItem(10);
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.addItem(11);
		EXPECT_EQ (temp2, 1);
		int temp3 = listVector.addItem(12);
		EXPECT_EQ (temp3, 2);

		listVector[0] = 111;
		listVector[1] = 222;
		listVector[2] = 333;

		EXPECT_EQ (listVector[0], 111);
		EXPECT_EQ (listVector[1], 222);
		EXPECT_EQ (listVector[2], 333);
	}

	TEST_F(GTest_listvector, test_operator_not_ok)
	{
		ListVector<int> listVector(2);
		
		int temp1 = listVector.addItem(10);
		EXPECT_EQ (temp1, 0);
		int temp2 = listVector.addItem(11);
		EXPECT_EQ (temp2, 1);
		try
		{
			listVector[123] = 111;
		}
		catch(const std::out_of_range& e)
		{
			listVector[0] = 111;
		}
		try
		{
			listVector[-2] = 222;
		}
		catch(const std::out_of_range& e)
		{
			listVector[1] = 222;
		}

		EXPECT_EQ (listVector[0], 111);
		EXPECT_EQ (listVector[1], 222);
	}

}  // namespace gtest_configreader
