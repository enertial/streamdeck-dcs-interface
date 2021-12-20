// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Protocols/DcsBiosStreamParser.h"

namespace test
{
#define SIZE_OF(x) (sizeof(x) / sizeof((x)[0]))

TEST(DcsBiosStreamParserTest, ReadSampleDataStream)
{
    const char sample_stream[] = {0x55,       0x55,       0x55, 0x55,                         // Sync frame
                                  0x08,       0x00,       0x02, 0x00, 0x6F, 0x72,             // Addr 0x0008 (2 bytes)
                                  0x02,       0x04,       0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, // Addr 0x0402 (4 bytes)
                                  0x0C,       0x74,       0x02, 0x00, 0x00, 0x02,             // Addr 0x740C (2 bytes)
                                  (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00};            // End of frame
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;
    for (int i = 0; i < SIZE_OF(sample_stream); i++) {
        EXPECT_FALSE(parser.at_end_of_frame());
        parser.processByte(sample_stream[i], data_by_address);
    }
    EXPECT_TRUE(parser.at_end_of_frame());
    // Expect 5 address locations, since DCS BIOS groups data into 16-bit (2 bytes) chunks at each address.
    EXPECT_EQ(data_by_address.size(), 5);
    EXPECT_EQ(data_by_address[0x0008], 0x726F);
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // Note that 4 bytes gets split into two 16-bit addresses
    EXPECT_EQ(data_by_address[0x0404], 0x302E);
    EXPECT_EQ(data_by_address[0x740C], 0x0200);
    EXPECT_EQ(data_by_address[0xFFFE], 0x0000);

    // Expect data of frame also to be stored internally as well.
    auto stored_data = parser.get_data_by_address_updated_this_frame();
    EXPECT_EQ(stored_data.size(), 5);
    EXPECT_EQ(stored_data[0x0008], 0x726F);
    EXPECT_EQ(stored_data[0x0402], 0x3031); // Note that 4 bytes gets split into two 16-bit addresses
    EXPECT_EQ(stored_data[0x0404], 0x302E);
    EXPECT_EQ(stored_data[0x740C], 0x0200);
    EXPECT_EQ(stored_data[0xFFFE], 0x0000);
}

TEST(DcsBiosStreamParserTest, HandleEmptyDataStream)
{
    // clang-format off
    const char sample_stream[] = {0x55, 0x55, 0x55, 0x55,              // Sync frame
                                  0x55, 0x55, 0x55, 0x55,              // Another Sync frame
                                  0x02, 0x04, 0x02, 0x00, 0x31, 0x30,  // Addr 0x0402 (2 bytes)
                                  (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;
    for (int i = 0; i < SIZE_OF(sample_stream); i++) {
        EXPECT_FALSE(parser.at_end_of_frame());
        parser.processByte(sample_stream[i], data_by_address);
    }
    EXPECT_TRUE(parser.at_end_of_frame());
    // Expect parser to handle multiple sync frames (and not set anything to address 0x5555).
    EXPECT_EQ(data_by_address.size(), 2);
    EXPECT_EQ(data_by_address[0x0402], 0x3031);
    EXPECT_EQ(data_by_address[0xFFFE], 0x0000);
}

TEST(DcsBiosStreamParserTest, ReadInteruptedDataStream)
{
    const char sample_stream[] = {0x55,       0x55,       0x55, 0x55,              // Sync frame
                                  0x08,       0x00,       0x02,                    // Cut-off frame
                                  0x55,       0x55,       0x55, 0x55,              // Another Sync frame
                                  0x02,       0x04,       0x02, 0x00, 0x31, 0x30,  // Addr 0x0402 (2 bytes)
                                  (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;
    for (int i = 0; i < SIZE_OF(sample_stream); i++) {
        EXPECT_FALSE(parser.at_end_of_frame());
        parser.processByte(sample_stream[i], data_by_address);
    }
    EXPECT_TRUE(parser.at_end_of_frame());
    // Expect parser to still see 2 addresses.
    EXPECT_EQ(data_by_address.size(), 3);
    EXPECT_EQ(data_by_address[0x0008], 0x5555); // Note this reads some of the next sync frame as data into 0x008
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // But successfully reads the next data address
    EXPECT_EQ(data_by_address[0xFFFE], 0x0000);
}

TEST(DcsBiosStreamParserTest, ReadDataStreamWithoutEndOfFrame)
{
    // clang-format off
    const char sample_stream[] = {0x55, 0x55, 0x55, 0x55,              // Sync frame
                                  0x08, 0x00, 0x02,                    // Cut-off frame
                                  0x55, 0x55, 0x55, 0x55,              // Another Sync frame
                                  0x02, 0x04, 0x02, 0x00, 0x31, 0x30}; // Addr 0x0402 (2 bytes)
    // clang-format on
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;
    for (int i = 0; i < SIZE_OF(sample_stream); i++) {
        parser.processByte(sample_stream[i], data_by_address);
        EXPECT_FALSE(parser.at_end_of_frame());
    }
    // Expect parser to still see 2 addresses.
    EXPECT_EQ(data_by_address.size(), 2);
    EXPECT_EQ(data_by_address[0x0008], 0x5555); // Note this reads some of the next sync frame as data into 0x008
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // But successfully reads the next data address
}

TEST(DcsBiosStreamParserTest, ResetCurrentFrameDataWithNextFrame)
{
    // clang-format off
    const char sample_stream_one[] = {0x55,       0x55,       0x55, 0x55,              // Sync frame
                                      0x08,       0x00,       0x02, 0x00, 0x6F, 0x72,  // Addr 0x0008 (2 bytes)
                                      0x02,       0x04,       0x04, 0x00, 0x31, 0x30,
                                      0x2E,       0x30,                                // Addr 0x0402 (4 bytes)
                                      (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    const char sample_stream_two[] = {0x55, 0x55, 0x55, 0x55,                          // Sync frame
                                      0x0C, 0x74, 0x02, 0x00, 0x00, 0x02,              // Addr 0x740C (2 bytes)
                                      (char)0xFE, (char)0xFF, 0x02, 0x00, 0x00, 0x00}; // End of frame
    // clang-format on
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;

    // Read in sample stream one.
    for (int i = 0; i < SIZE_OF(sample_stream_one); i++) {
        EXPECT_FALSE(parser.at_end_of_frame());
        parser.processByte(sample_stream_one[i], data_by_address);
    }
    EXPECT_TRUE(parser.at_end_of_frame());

    EXPECT_EQ(data_by_address.size(), 4);
    EXPECT_EQ(data_by_address[0x0008], 0x726F);
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // Note that 4 bytes gets split into two 16-bit addresses
    EXPECT_EQ(data_by_address[0x0404], 0x302E);
    EXPECT_EQ(data_by_address[0xFFFE], 0x0000);
    // Expect data of frame also to be stored internally as well.
    auto stored_data = parser.get_data_by_address_updated_this_frame();
    EXPECT_EQ(stored_data.size(), 4);
    EXPECT_EQ(stored_data[0x0008], 0x726F);
    EXPECT_EQ(stored_data[0x0402], 0x3031); // Note that 4 bytes gets split into two 16-bit addresses
    EXPECT_EQ(stored_data[0x0404], 0x302E);
    EXPECT_EQ(stored_data[0xFFFE], 0x0000);

    // Read in sample stream two.
    for (int i = 0; i < SIZE_OF(sample_stream_two); i++) {
        parser.processByte(sample_stream_two[i], data_by_address);
    }
    EXPECT_TRUE(parser.at_end_of_frame());
    // Expect 5 address locations, since DCS BIOS groups data into 16-bit (2 bytes) chunks at each address.
    EXPECT_EQ(data_by_address.size(), 5);
    EXPECT_EQ(data_by_address[0x0008], 0x726F);
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // Note that 4 bytes gets split into two 16-bit addresses
    EXPECT_EQ(data_by_address[0x0404], 0x302E);
    EXPECT_EQ(data_by_address[0x740C], 0x0200);
    EXPECT_EQ(data_by_address[0xFFFE], 0x0000);
    // Expect only data from most recent frame in stored data.
    auto stored_data_two = parser.get_data_by_address_updated_this_frame();
    EXPECT_EQ(stored_data_two.size(), 2);
    EXPECT_EQ(data_by_address[0x740C], 0x0200);
    EXPECT_EQ(stored_data_two[0xFFFE], 0x0000);
}
} // namespace test
