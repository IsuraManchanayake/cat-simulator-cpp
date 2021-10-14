//
// Created by isura on 10/13/2021.
//

#include <gtest/gtest.h>
#include <catsim/Exceptions.h>

#include "catsim/ArgParser.h"


using testing::Eq;

TEST(ArgParserTestSuite, Constructor) {
    const char* argv[] = {"--name", "John", "--age", "25"};
    ArgParser argParser(std::size(argv), argv);

    EXPECT_EQ(argParser._raw_args.size(), 4);
}

TEST(ArgParserTestSuite, Parse) {
    const char* argv[] = {"--name", "John", "--age", "25", "--graduated", "--temperature", "37.2"};

    ArgParser argParser(sizeof(argv) / sizeof(argv[0]), argv);
    argParser
            .add_parser<std::string>("--name", "The name", true, "Dave")
            .add_parser<int>("--age", "The age", true, 12)
            .add_parser<double>("--temperature", "The temperature", false, 36.5)
            .add_parser<bool>("--graduated", "Graduated flag", true, true)
            .parse();

    const auto& name = argParser.get_value<std::string>("--name");
    const auto& age = argParser.get_value<int>("--age");
    const auto& temperature = argParser.get_value<double>("--temperature");
    const auto& graduated = argParser.get_value<bool>("--graduated");

    EXPECT_EQ(argParser._raw_args.size(), 0);
    EXPECT_EQ(age, 25);
    EXPECT_EQ(name, "John");
    EXPECT_NEAR(temperature, 37.2, 1e-8);
    EXPECT_TRUE(graduated);
}

TEST(ArgParserTestSuite, DefaultValues) {
    const char* argv[] = {""};

    ArgParser argParser(sizeof(argv) / sizeof(const char*), argv);
    argParser
            .add_parser<std::string>("--name", "The name", false, "Dave")
            .add_parser<int>("--age", "The age", false, 12)
            .add_parser<double>("--temperature", "The temperature", false, 36.5)
            .add_parser<bool>("--graduated", "Graduated flag", false, true)
            .parse();

    const auto& name = argParser.get_value<std::string>("--name");
    const auto& age = argParser.get_value<int>("--age");
    const auto& temperature = argParser.get_value<double>("--temperature");
    const auto& graduated = argParser.get_value<bool>("--graduated");

    EXPECT_EQ(argParser._raw_args.size(), 0);
    EXPECT_EQ(age, 12);
    EXPECT_EQ(name, "Dave");
    EXPECT_NEAR(temperature, 36.5, 1e-8);
    EXPECT_TRUE(graduated);
}


TEST(ArgParserTestSuite, RequiredNotProvidedException) {
    const char* argv[] = {"--name", "John"};
    ArgParser argParser(sizeof(argv) / sizeof(argv[0]), argv);

    EXPECT_THROW({
        try
        {
         argParser
             .add_parser<std::string>("--name", "The name", true, "Dave")
             .add_parser<int>("--age", "The age", true, 12)
             .parse();
        }
        catch( const ArgumentException& e )
        {
         EXPECT_STREQ("--age: Required but not provided.", e.what());
         throw;
        }
    }, ArgumentException);
}
