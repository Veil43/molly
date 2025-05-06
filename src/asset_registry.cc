#include "asset_registry.h"

#include <fstream>
#include <optional>
#include <sstream>

#include "types.h"
#include "textfile.h"

auto AssetRegistry::init(const std::string& cache_path, const std::string& asset_path, bool load_cache) -> bool {
    this->cache_dir = cache_path;
    this->asset_dir = asset_path;
    
    // init m_live_handles
    // init m_free_handle_indices
    // init m_free_handles

    this->m_free_handle_indices = std::vector<std::queue<u32>>{MOLLY_ASSET_TYPE_COUNT};
    this->m_asset_cache = std::unordered_map<std::string, std::pair<std::string, u64>>{};

    fs::create_directory(this->cache_dir);
    fs::create_directory(this->asset_dir);

    // Load the cache if it exists
    if (load_cache) {        
        auto cache_map = loadCache(this->cache_dir + "cache.bin");
        this->m_asset_cache = cache_map;
    }

    return true; 
}

auto AssetRegistry::getTextFile(ARegHandle h) -> std::shared_ptr<TextFile> { 
    if (h.type() != MOLLY_ASSET_TEXT_FILE_TYPE) {
        std::cerr << "WARNING: Cannot find asset with handle: " << std::hex <<  h.value << " you will receive empty asset\n";
        return nullptr;
    }

    return std::make_shared<TextFile>(m_text_files[h.index()]); 
}

ARegHandle AssetRegistry::loadAsset(const std::string& path, u32 type, const std::string& name) {
    // TODO: Invalidate cache when path changes
    // periodically (maybe at delete calls) check for files and see if they have changed and invalidate the cache

    // NOTE: If we load an already loaded asset we will load from the binary format but we will not avoid in-memory duplication
    // NOTE: keeping track of a live handle + path might be useful to solve this problem

    u64 mod_time = getFileModTime(path.c_str());

    if (this->inCache(path)) {
        // TODO: Check for last date modified
        std::cout << "Cahe Hit!\n";
        
        if (fs::exists(path) && mod_time <= this->m_asset_cache[path].second) {
            return this->readAssetFromBinary(this->m_asset_cache[path].first, type);
        } else {
            std::cout << "Stale File: Erase!\n";
            this->m_asset_cache.erase(path);
        }
    }

    ARegHandle handle = {};
    switch (type) {
        case MOLLY_ASSET_TEXT_FILE_TYPE: {
            std::string file_data = loadTextFile(path.c_str());
            TextFile file = {};
            if (file_data == "") {
                return handle;
            }
            file.content = file_data;
            file.name = name;
            handle = this->storeAsset(std::move(file));
        } break;
    }

    std::string bin_path = this->writeAssetToBinary(handle);
    this->m_asset_cache[path].first = bin_path; // NOTE: maybe we could store the handle also
    this->m_asset_cache[path].second = mod_time;
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

/*
    [count][[size]raw_path][[size](bin_path),[size](last_store)]
*/
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
        size_t value_size1 = value.first.size();
        size_t value_size2 = sizeof(value.second);
        u64 last_mod_time = getFileModTime(value.first.c_str());

        output_file.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
        output_file.write(key.data(), key_size);
        output_file.write(reinterpret_cast<const char*>(&value_size1), sizeof(value_size1));
        output_file.write(value.first.data(), value_size1);
        output_file.write(reinterpret_cast<const char*>(&value_size2), sizeof(value_size2));
        output_file.write(reinterpret_cast<const char*>(&last_mod_time), value_size2);
    }
}

/*
    [count][[size]raw_path][[size](bin_path),[size](last_store)]
*/
auto AssetRegistry::loadCache(const std::string& src) -> std::unordered_map<std::string, std::pair<std::string, u64>> {
    std::ifstream input_file(src, std::ios::binary);
    std::unordered_map<std::string, std::pair<std::string, u64>> output{};

    if (!input_file.is_open()) {
        std::cerr << "Warning: Could not load cache from path: " << src << std::endl;
        return output;
    }

    size_t size;
    input_file.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (size_t i = 0; i < size; i++) {
        size_t key_size;
        size_t value_size1;
        size_t value_size2;

        input_file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        std::string key(key_size, '\0'); // fill it with null
        input_file.read(&key[0], key_size);

        input_file.read(reinterpret_cast<char*>(&value_size1), sizeof(value_size1));
        std::string value1(value_size1, '\0');
        input_file.read(&value1[0], value_size1);

        input_file.read(reinterpret_cast<char*>(&value_size2), sizeof(value_size2));
        u64 value2;
        input_file.read(reinterpret_cast<char*>(&value2), value_size2);
        
        output[key].first = value1;
        output[key].second = value2;
    }

    return std::move(output);
}

// --------------------------- NON ASSET REGISTRY FUNCTIONS ------------------------------------