#ifndef MOLLY_TEST_UTILS_H
#define MOLLY_TEST_UTILS_H

#include <string>
#include <sstream>
#include <fstream>
#include <types.h>
#include <iostream>
#include <bitset>
#include <filesystem>

#define DO_TRY_CATCH(statement)                                     \
    try {                                                           \
        statement                                                   \
    }  catch (const std::runtime_error& e) {                        \
        std::cerr << "Caught runtime error: " << e.what() << "\n";  \
    } catch (const std::exception& e) {                             \
        std::cerr << "Caught exception: " << e.what() << "\n";      \
    } catch (...) {                                                 \
        std::cerr << "Caught unspecified exception" << std::endl;   \
    }

static std::string printBits(u64 num) {
    std::stringstream bits;
    // bits << std::format("{:b}", num); 20++
    bits << std::bitset<64>(num);
    return bits.str();
}

#define FUNNY_STRING "Your mother was a hamester and your father smelt of elderberries"
#define UNFUNNY_STRING "What did the fish say when he swam into the wall? Dam."
#define STRANGE_STRING "It's also the source of Well you're full of shiiake mushrooms."

static void writeToFile(const std::string& path, const std::string& content) {
    std::ofstream tmp(path);
    if (tmp.is_open()) {
        tmp << content;
        tmp.close();
    } else {
        std::cerr << "TEST: Could not create file: " << path << std::endl;
    }
}

static bool removeFile(const std::string& path) {
    if (!std::filesystem::remove(path)) {
        std::cerr << "ERROR::IO: Could not delete file: " << path << std::endl;
        return false;
    }
    return true;
}

#endif // MOLLY_TEST_UTILS_H