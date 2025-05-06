#ifndef MOLLY_TEXT_FILE_H
#define MOLLY_TEXT_FILE_H

#include <string>
#include "types.h"
/*
    Binary format for text files
    [name_size][name][content_size][content]
*/
struct TextFile {
    std::string name;
    std::string content;
};

auto loadTextFile(const char* path) -> std::string;
auto getFileModTime(const char* path) -> u64;
auto writeTextFileToBinary(const TextFile& file, const std::string& dest_path) -> void;
auto readTextFileFromBinary(const std::string& path) -> TextFile;
#endif // MOLLY_TEXT_FILE_H