#include "gtest_configreader.h"

//#define DEBUG_CONFIG

constexpr auto CONFIG{ "gtest/test.json" };
constexpr auto TEST_DATA{ "TestData" };

using ::testing::AtLeast;


namespace gtest_configreader {

    TEST_F(GTest_configreader, test_readConfig)
    {
        QJsonObject _objReference{{"General",QJsonObject{{"LogLevel",1}}}};

        ConfigReader * _configreader = new ConfigReader();
        QJsonObject obj{};
        _configreader->readConfig(CONFIG, obj);
        #ifdef DEBUG_CONFIG
        qDebug() << "obj:" << obj;
        #endif
        
        EXPECT_EQ (obj[TEST_DATA], 123);
    }

    TEST_F(GTest_configreader, test2)
    {

    }
}  // namespace gtest_configreader
