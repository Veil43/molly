#include "asset_registry.h"

#include <fstream>
#include <optional>
#include <sstream>

#include "types.h"

using namespace tmp;

std::string loadTextFile(const char* path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "UTIL::IO: Could not open file: " << path << std::endl;
        return "";
    }

    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

auto AssetRegistry::getTextFile(ARegHandle h) -> std::shared_ptr<TextFile> { 
    if (h.type() != MOLLY_ASSET_TEXT_FILE_TYPE) {
        std::cerr << "WARNING: Cannot find asset with handle: " << std::hex <<  h.value << " you will receive empty asset\n";
        return nullptr;
    }

    return std::make_shared<TextFile>(m_text_files[h.index()]); 
}

ARegHandle AssetRegistry::loadAsset(const std::string& path, u32 type, std::optional<std::string> name) {
    ARegHandle handle = {};
    // TODO: Invalidate cache when path changes
    // periodically (maybe at delete calls) check for files and see if they have changed and invalidate the cache

    // NOTE: If we load an already loaded asset we will load from the binary format but we will not avoid in-memory duplication
    // NOTE: keeping track of a live handle + path might be useful to solve this problem

    if (this->inCache(path)) {
        std::cout << "HERE!\n";
        return this->readAssetFromBinary(this->m_asset_cache[path], type);
    }

    switch (type) {
        case MOLLY_ASSET_TEXT_FILE_TYPE: {
            std::string file_data = loadTextFile(path.c_str());
            TextFile file = {};
            file.data = file_data;
            
            if (name) {
                file.name = *name;
            } else {
                file.name = path;
            }

            handle = this->storeAsset(std::move(file));
        } break;
    }

    std::string bin_path = this->writeAssetToBinary(handle);
    this->m_asset_cache[path] = bin_path; // NOTE: maybe we could store the handle also
    return handle;
}

ARegHandle AssetRegistry::storeAsset(TextFile&& file) {
    ARegHandle handle = {};
    // find the next available slot for this text file
    auto& text_free_indices = this->m_free_handle_indices[MOLLY_ASSET_TEXT_FILE_TYPE];
    u32 index = m_text_files.empty()? 0 : this->m_text_files.size();
    u32 generation = 0;

    if (!text_free_indices.empty()) {
        index = text_free_indices.front();
        text_free_indices.pop();
        auto& handles_at_index = this->m_live_handles[index];
        generation = handles_at_index[MOLLY_ASSET_TEXT_FILE_TYPE].generation() + 1;
    }

    handle.value = static_cast<u64>(index) | 
                   static_cast<u64>(generation) << 32 |
                   static_cast<u64>(MOLLY_ASSET_TEXT_FILE_TYPE) << 56;

    if (index >= this->m_text_files.size()) {
        // store the file
        this->m_text_files.push_back(file); 
        // store the handle
        if (index >= this->m_live_handles.size()) {
            std::array<ARegHandle, MOLLY_ASSET_TYPE_COUNT> new_index_array;
            new_index_array[MOLLY_ASSET_TEXT_FILE_TYPE] = handle;
            this->m_live_handles.push_back(std::move(new_index_array));
        } else {
            this->m_live_handles[index][MOLLY_ASSET_TEXT_FILE_TYPE] = handle;
        }
    } else {
        this->m_text_files[index] = file;
        this->m_live_handles[index][MOLLY_ASSET_TEXT_FILE_TYPE] = handle;
    }

    return handle;
}

auto AssetRegistry::writeAssetToBinary(ARegHandle h) -> std::string {
    u32 type = h.type();
    std::string output_path = this->asset_dir;

    switch (type) {
        case MOLLY_ASSET_TEXT_FILE_TYPE: {
            TextFile file = this->m_text_files[h.index()];
            output_path += file.name + ".asset";
            writeTextFileToBinary(file, output_path);
        } break;
    }

    return output_path;
}

auto AssetRegistry::readAssetFromBinary(const std::string& path, u32 type) -> ARegHandle {
    
    switch(type) {
        case MOLLY_ASSET_TEXT_FILE_TYPE: {
            TextFile file = readTextFileFromBinary(path);
            return this->storeAsset(std::move(file));
        } break;
    }

    return {};
}

auto AssetRegistry::storeCache(const std::string& dest) -> void {
    std::ofstream output_file(dest, std::ios::binary);

    if (!output_file.is_open()) {
        std::cerr << "ERROR: Could not store cache to path: " << dest << std::endl;
        return;
    }
    
    size_t size = this->m_asset_cache.size();
    output_file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& [key, value] : this->m_asset_cache) {
        size_t key_size = key.size();
        size_t value_size = value.size();

        output_file.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
        output_file.write(key.data(), key_size);
        output_file.write(reinterpret_cast<const char*>(&value_size), sizeof(value_size));
        output_file.write(value.data(), value_size);
    }
}

auto AssetRegistry::loadCache(const std::string& src) -> std::unordered_map<std::string, std::string> {
    std::ifstream input_file(src, std::ios::binary);
    std::unordered_map<std::string, std::string> output{};

    if (!input_file.is_open()) {
        std::cerr << "Warning: Could not load cache from path: " << src << std::endl;
        return output;
    }

    size_t size;
    input_file.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    for (size_t i = 0; i < size; i++) {
        size_t key_size;
        size_t value_size;

        input_file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        std::string key(key_size, '\0'); // fill it with null
        input_file.read(&key[0], key_size);

        input_file.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
        std::string value(value_size, '\0');
        input_file.read(&value[0], value_size);

        output[key] = value;
    }

    return output;
}

// --------------------------- NON ASSET REGISTRY FUNCTIONS ------------------------------------
auto writeTextFileToBinary(const tmp::TextFile& file, const std::string& dest_path) -> void {
    std::ofstream bin(dest_path, std::ios::binary);
    if (!bin.is_open()) {
        std::cerr << "ERROR: Could not write asset: " << file.name << " to binary at: " << dest_path << std::endl;;
        return;
    }

    size_t name_size = file.name.size();
    size_t content_size = file.data.size();

    bin.write(reinterpret_cast<const char*>(&name_size), sizeof(name_size));
    bin.write(file.name.data(), name_size);
    bin.write(reinterpret_cast<const char*>(&content_size), sizeof(content_size));
    bin.write(file.data.data(), content_size);
}

auto readTextFileFromBinary(const std::string& path) -> tmp::TextFile {
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
    text_file.data = std::move(content);

    return std::move(text_file);
}