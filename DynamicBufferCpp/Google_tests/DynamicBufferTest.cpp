#include "DynamicBuffer.h"
#include <gtest/gtest.h>
#include <vector>
#include <cmath> // For std::isnan

class DynamicBufferTest : public ::testing::Test {
protected:
    DynamicBuffer buffer;
    DynamicBuffer bufferUniqueVariable;

    DynamicBufferTest() : buffer(2, 10), bufferUniqueVariable(1, 10) {
    } // Assuming 2 variables and a window size of 10

    void SetUp() override {
        // Setup code, if needed
    }

    void TearDown() override {
        // Cleanup code, if needed
    }
};

TEST_F(DynamicBufferTest, AddSingleRecordAndCheckNaNTwoVariables) {
    // Add a record for the first variable
    long timestamp = 100; // Example timestamp
    double value1 = 1.1;
    buffer.addOrUpdateRecord(timestamp, 0, value1); // First variable

    // Retrieve the record for the given timestamp
    auto record1 = buffer.getRecordByTimestamp(timestamp);
    ASSERT_EQ(record1.size(), 2u); // Ensure we have exactly 2 values

    // Check the first value is as expected
    EXPECT_NEAR(record1[0], value1, 1e-5); // For floating-point comparison

    // Check the second value is NaN
    EXPECT_TRUE(std::isnan(record1[1]));
}

TEST_F(DynamicBufferTest, AddSingleRecordAndCheckNaNOneVariable) {
    // Add a record for the first variable
    long timestamp = 100; // Example timestamp
    double value1 = 1.1;
    bufferUniqueVariable.addOrUpdateRecord(timestamp, 0, value1); // First variable

    // Retrieve the record for the given timestamp
    auto record1 = bufferUniqueVariable.getRecordByTimestamp(timestamp);
    ASSERT_EQ(record1.size(), 1u); // Ensure we have exactly 1 value

    // Check the first value is as expected
    EXPECT_NEAR(record1[0], value1, 1e-5); // For floating-point comparison
}

TEST_F(DynamicBufferTest, OrderedInsertionAndSliceRetrievalTwoVariables) {
    buffer.addOrUpdateRecord(100, 0, 1.0);
    buffer.addOrUpdateRecord(100, 1, 2.0);
    buffer.addOrUpdateRecord(101, 0, 1.1);
    buffer.addOrUpdateRecord(101, 1, 2.1);
    buffer.addOrUpdateRecord(102, 0, 1.2);
    buffer.addOrUpdateRecord(102, 1, 2.2);

    size_t outSize;
    // Request a slice from the latest timestamp backwards, expecting to get all inserted records
    auto slice = buffer.getSlice(102, 3, outSize); // Assuming getSlice returns std::vector<double>
    auto sliceTimestamps = buffer.getSliceTimestamps(102, 3);

    // Check if the outSize matches the expected number of records times the number of variables
    ASSERT_EQ(outSize, 3 * buffer.getNVariables()); // Assuming each record contains data for all variables

    // Verify the slice contains the correct data, in the expected order
    EXPECT_NEAR(slice[0], 1.0, 1e-5);
    EXPECT_NEAR(slice[1], 2.0, 1e-5);
    EXPECT_NEAR(slice[2], 1.1, 1e-5);
    EXPECT_NEAR(slice[3], 2.1, 1e-5);
    EXPECT_NEAR(slice[4], 1.2, 1e-5);
    EXPECT_NEAR(slice[5], 2.2, 1e-5);
}

TEST_F(DynamicBufferTest, OrderedInsertionAndSliceRetrievalOneVariable) {
    bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.0);
    bufferUniqueVariable.addOrUpdateRecord(101, 0, 1.1);
    bufferUniqueVariable.addOrUpdateRecord(102, 0, 1.2);

    size_t outSize;
    // Request a slice from the latest timestamp backwards, expecting to get all inserted records
    auto slice = bufferUniqueVariable.getSlice(102, 3, outSize); // Assuming getSlice returns std::vector<double>
    auto sliceTimestamps = bufferUniqueVariable.getSliceTimestamps(102, 3);

    // Check if the outSize matches the expected number of records times the number of variables
    ASSERT_EQ(outSize, 3 * bufferUniqueVariable.getNVariables());
    // Assuming each record contains data for all variables

    // Verify the slice contains the correct data, in the expected order
    EXPECT_NEAR(slice[0], 1.0, 1e-5);
    EXPECT_NEAR(slice[1], 1.1, 1e-5);
    EXPECT_NEAR(slice[2], 1.2, 1e-5);
}


TEST_F(DynamicBufferTest, UnorderedInsertionAndSliceRetrievalTwoVariables) {
    // Insert records out of order
    buffer.addOrUpdateRecord(100, 0, 1.0);
    buffer.addOrUpdateRecord(102, 0, 1.2);
    buffer.addOrUpdateRecord(101, 0, 1.1);
    buffer.addOrUpdateRecord(102, 1, 2.2);
    buffer.addOrUpdateRecord(101, 1, 2.1);
    buffer.addOrUpdateRecord(100, 1, 2.0);
    buffer.addOrUpdateRecord(106, 0, 1.6);
    buffer.addOrUpdateRecord(104, 0, 1.4);
    buffer.addOrUpdateRecord(104, 1, 2.4);
    buffer.addOrUpdateRecord(106, 1, 2.6);
    buffer.addOrUpdateRecord(105, 0, 1.5);
    buffer.addOrUpdateRecord(105, 1, 2.5);


    size_t outSize;
    // Request a slice from the latest timestamp backwards, expecting to get all inserted records
    auto slice = buffer.getSlice(105, 3, outSize);

    // Check if the outSize matches the expected number of records times the number of variables
    ASSERT_EQ(outSize, 3 * buffer.getNVariables()); // Assuming each record contains data for all variables

    // Verify the slice contains the correct data, in the expected order
    EXPECT_NEAR(slice[0], 1.2, 1e-5); // -> Timestamp 102 variable 0
    EXPECT_NEAR(slice[1], 2.2, 1e-5); // -> Timestamp 102 variable 1
    EXPECT_NEAR(slice[2], 1.4, 1e-5); // -> Timestamp 104 variable 0
    EXPECT_NEAR(slice[3], 2.4, 1e-5); // -> Timestamp 104 variable 1
    EXPECT_NEAR(slice[4], 1.5, 1e-5); // -> Timestamp 105 variable 0
    EXPECT_NEAR(slice[5], 2.5, 1e-5); // -> Timestamp 105 variable 1
}

TEST_F(DynamicBufferTest, UnorderedInsertionAndSliceRetrievalOneVariable) {
    // Insert records out of order
    bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.0);
    bufferUniqueVariable.addOrUpdateRecord(102, 0, 1.2);
    bufferUniqueVariable.addOrUpdateRecord(101, 0, 1.1);
    bufferUniqueVariable.addOrUpdateRecord(106, 0, 1.6);
    bufferUniqueVariable.addOrUpdateRecord(104, 0, 1.4);
    bufferUniqueVariable.addOrUpdateRecord(105, 0, 1.5);

    size_t outSize;
    // Request a slice from the latest timestamp backwards, expecting to get all inserted records
    auto slice = bufferUniqueVariable.getSlice(105, 3, outSize);

    // Check if the outSize matches the expected number of records times the number of variables
    ASSERT_EQ(outSize, 3 * bufferUniqueVariable.getNVariables());
    // Assuming each record contains data for all variables

    // Verify the slice contains the correct data, in the expected order
    EXPECT_NEAR(slice[0], 1.2, 1e-5);
    EXPECT_NEAR(slice[1], 1.4, 1e-5);
    EXPECT_NEAR(slice[2], 1.5, 1e-5);
}

TEST_F(DynamicBufferTest, OrderedInsertionAndSliceTimestampsRetrievalOneVariable) {
    bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.0);
    bufferUniqueVariable.addOrUpdateRecord(101, 0, 1.1);
    bufferUniqueVariable.addOrUpdateRecord(102, 0, 1.2);

    auto timestamps = bufferUniqueVariable.getSliceTimestamps(102, 3);
    EXPECT_NEAR(timestamps[0], 100, 1e-5);
    EXPECT_NEAR(timestamps[1], 101, 1e-5);
    EXPECT_NEAR(timestamps[2], 102, 1e-5);
}

TEST_F(DynamicBufferTest, OrderedInsertionAndSliceTimestampsRetrievalTwoVariables) {
    buffer.addOrUpdateRecord(100, 0, 1.0);
    buffer.addOrUpdateRecord(100, 1, 2.0);
    buffer.addOrUpdateRecord(101, 0, 1.1);
    buffer.addOrUpdateRecord(101, 1, 2.1);
    buffer.addOrUpdateRecord(102, 0, 1.2);
    buffer.addOrUpdateRecord(102, 1, 2.2);

    auto timestamps = buffer.getSliceTimestamps(102, 3);
    EXPECT_NEAR(timestamps[0], 100, 1e-5);
    EXPECT_NEAR(timestamps[1], 101, 1e-5);
    EXPECT_NEAR(timestamps[2], 102, 1e-5);
}

TEST_F(DynamicBufferTest, UnorderedInsertionAndSliceTimestampsRetrievalOneVariable) {
    bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.0);
    bufferUniqueVariable.addOrUpdateRecord(102, 0, 1.2);
    bufferUniqueVariable.addOrUpdateRecord(101, 0, 1.1);
    bufferUniqueVariable.addOrUpdateRecord(106, 0, 1.6);
    bufferUniqueVariable.addOrUpdateRecord(104, 0, 1.4);
    bufferUniqueVariable.addOrUpdateRecord(105, 0, 1.5);

    auto timestamps = bufferUniqueVariable.getSliceTimestamps(105, 3);
    EXPECT_NEAR(timestamps[0], 102, 1e-5);
    EXPECT_NEAR(timestamps[1], 104, 1e-5);
    EXPECT_NEAR(timestamps[2], 105, 1e-5);
}

TEST_F(DynamicBufferTest, UnorderedInsertionAndSliceTimestampsRetrievalTwoVariables) {
    buffer.addOrUpdateRecord(100, 0, 1.0);
    buffer.addOrUpdateRecord(102, 0, 1.2);
    buffer.addOrUpdateRecord(101, 0, 1.1);
    buffer.addOrUpdateRecord(102, 1, 2.2);
    buffer.addOrUpdateRecord(101, 1, 2.1);
    buffer.addOrUpdateRecord(100, 1, 2.0);
    buffer.addOrUpdateRecord(106, 0, 1.6);
    buffer.addOrUpdateRecord(104, 0, 1.4);
    buffer.addOrUpdateRecord(104, 1, 2.4);
    buffer.addOrUpdateRecord(106, 1, 2.6);
    buffer.addOrUpdateRecord(105, 0, 1.5);
    buffer.addOrUpdateRecord(105, 1, 2.5);

    auto timestamps = buffer.getSliceTimestamps(105, 3);
    EXPECT_NEAR(timestamps[0], 102, 1e-5);
    EXPECT_NEAR(timestamps[1], 104, 1e-5);
    EXPECT_NEAR(timestamps[2], 105, 1e-5);
}

TEST_F(DynamicBufferTest, OrderedInsertionAndSliceTimestampsRetrievalOneVariableLong) {
    bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.0);
    bufferUniqueVariable.addOrUpdateRecord(101, 0, 1.1);
    bufferUniqueVariable.addOrUpdateRecord(102, 0, 1.2);

    auto timestamps = bufferUniqueVariable.getSliceTimestamps(102, 3);
    EXPECT_NEAR(timestamps[0], 100, 1e-5);
    EXPECT_NEAR(timestamps[1], 101, 1e-5);
    EXPECT_NEAR(timestamps[2], 102, 1e-5);
}

TEST_F(DynamicBufferTest, OrderedInsertionNewEntryCheckOneVariable) {
    bool newEntry = bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.0);
    EXPECT_TRUE(newEntry);
    bool secondEntry = bufferUniqueVariable.addOrUpdateRecord(100, 0, 1.1);
    EXPECT_FALSE(secondEntry);
}

TEST_F(DynamicBufferTest, OrderedInsertionNewEntryCheckTwoVariable) {
    bool newEntry = buffer.addOrUpdateRecord(100, 0, 1.0);
    EXPECT_TRUE(newEntry);
    bool secondEntry = buffer.addOrUpdateRecord(100, 1, 1.1);
    EXPECT_FALSE(secondEntry);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
