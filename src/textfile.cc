#include "textfile.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

auto loadTextFile(const char* path) -> std::string {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "UTIL::IO: Could not open file: " << path << std::endl;
        return "";
    }

    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

auto getFileModTime(const char* path) -> u64 {
    struct stat st;
    if (stat(path, &st) != 0) {
        std::cerr << "UTIL::IO: Could not query mod time for file: " << path << std::endl;
        return 0;
    }
    return (u64)st.st_mtime;
}


auto writeTextFileToBinary(const TextFile& file, const std::string& dest_path) -> void {
    std::ofstream bin(dest_path, std::ios::binary);
    if (!bin.is_open()) {
        std::cerr << "ERROR: Could not write asset: " << file.name << " to binary at: " << dest_path << std::endl;;
        return;
    }

    size_t name_size = file.name.size();
    size_t content_size = file.content.size();
    
    bin.write(reinterpret_cast<const char*>(&name_size), sizeof(name_size));
    bin.write(file.name.data(), name_size);
    bin.write(reinterpret_cast<const char*>(&content_size), sizeof(content_size));
    bin.write(file.content.data(), content_size);
}

auto readTextFileFromBinary(const std::string& path) -> TextFile {
    TextFile text_file = {};
            
    // NOTE: The hope here is that loading a whole chunk is faster than line by line
    // TODO: Test to see if this is actually faster
    std::ifstream asset_file(path, std::ios::binary);
    if (!asset_file.is_open()) {
        std::cerr << "ERROR: could not load: " << path << " from binary.\n";
        return text_file;
    }

    size_t name_size;
    size_t content_size;

    asset_file.read(reinterpret_cast<char*>(&name_size), sizeof(name_size));
    std::string name(name_size, '\0');
    asset_file.read(name.data(), name_size);

    asset_file.read(reinterpret_cast<char*>(&content_size), sizeof(content_size));
    std::string content(content_size, '\0');
    asset_file.read(content.data(), content_size);    
    
    // TODO: check total file size
    text_file.name = std::move(name);
    text_file.content = std::move(content);

    return std::move(text_file);
}