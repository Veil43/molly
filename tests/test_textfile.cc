#include <catch2/catch_test_macros.hpp>
#include "test_utils.h"
#include "textfile.h"

// TODO: Add more complicated and exotic cases
TEST_CASE("struct TextFile: loadTextFile(const char*): Can load one file.") {
    const std::string path = "temporary_file.txt";
    writeToFile(path, FUNNY_STRING);
    std::string file_content = loadTextFile(path.c_str());
    CHECK(file_content == FUNNY_STRING);
    removeFile(path);
}

// TODO: add more exotic cases (though chances are nothing's gonna change)
TEST_CASE("struct TextFile: writeTextFileToBinary/readTextFileToBinary: Can read and write text file to/from bin") {
    const std::string path = "temporary_file.bin";
    // writeToFile(path, UNFUNNY_STRING);
    TextFile expected = {};
    expected.content = UNFUNNY_STRING;
    expected.name = "unfunny_file";
    writeTextFileToBinary(expected, path);
    TextFile output = readTextFileFromBinary(path);
    REQUIRE(output.name == expected.name);
    REQUIRE(output.content == expected.content);
}