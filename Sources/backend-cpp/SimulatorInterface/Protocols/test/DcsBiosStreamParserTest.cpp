// Copyright 2021 Charles Tytler

#include "gtest/gtest.h"

#include "SimulatorInterface/Protocols/DcsBiosStreamParser.h"

namespace test
{
#define SIZE_OF(x) (sizeof(x) / sizeof((x)[0]))

TEST(DcsBiosStreamParserTest, ReadSampleDataStream)
{
    const char sample_stream[] = {0x55, 0x55, 0x55, 0x55,                         // Sync frame
                                  0x08, 0x00, 0x02, 0x00, 0x6F, 0x72,             // Addr 0x0008 (2 bytes)
                                  0x02, 0x04, 0x04, 0x00, 0x31, 0x30, 0x2E, 0x30, // Addr 0x0402 (4 bytes)
                                  0x0C, 0x74, 0x02, 0x00, 0x00, 0x02};            // Addr 0x740C (2 bytes)
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;
    for (int i = 0; i < SIZE_OF(sample_stream); i++) {
        parser.processByte(sample_stream[i], data_by_address);
    }
    // Expect 4 address locations, since DCS BIOS groups data into 16-bit (2 bytes) chunks at each address.
    EXPECT_EQ(data_by_address.size(), 4);
    EXPECT_EQ(data_by_address[0x0008], 0x726F);
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // Note that 4 bytes gets split into two 16-bit addresses
    EXPECT_EQ(data_by_address[0x0404], 0x302E);
    EXPECT_EQ(data_by_address[0x740C], 0x0200);
}

TEST(DcsBiosStreamParserTest, HandleEmptyDataStream)
{
    // clang-format off
    const char sample_stream[] = {0x55, 0x55, 0x55, 0x55,              // Sync frame
                                  0x55, 0x55, 0x55, 0x55,              // Another Sync frame
                                  0x02, 0x04, 0x02, 0x00, 0x31, 0x30}; // Addr 0x0402 (2 bytes)
    // clang-format on
    std::unordered_map<unsigned int, unsigned int> data_by_address;
    DcsBiosStreamParser parser;
    for (int i = 0; i < SIZE_OF(sample_stream); i++) {
        parser.processByte(sample_stream[i], data_by_address);
    }
    // Expect parser to handle multiple sync frames (and not set anything to address 0x5555).
    EXPECT_EQ(data_by_address.size(), 1);
    EXPECT_EQ(data_by_address[0x0402], 0x3031);
}

TEST(DcsBiosStreamParserTest, ReadInteruptedDataStream)
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
    }
    // Expect parser to still see 2 addresses.
    EXPECT_EQ(data_by_address.size(), 2);
    EXPECT_EQ(data_by_address[0x0008], 0x5555); // Note this reads some of the next sync frame as data into 0x008
    EXPECT_EQ(data_by_address[0x0402], 0x3031); // But successfully reads the next data address
}
} // namespace test
