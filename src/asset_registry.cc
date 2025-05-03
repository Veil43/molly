#include "asset_registry.h"

#include "utils.h"
#include "types.h"

using namespace tmp;

ARegHandle AssetRegistry::loadAsset(const std::string& str, u32 type) {
    ARegHandle handle = {};
    // 1. Get the asset (in cache or no?)
        // If in cache load from cache
    if (this->m_asset_cache.find(str) != this->m_asset_cache.end()) {
        return this->loadAssetBin(this->m_asset_cache[str], type);
    }

    switch (type) {
        case MOLLY_ASSET_TEXT_FILE_TYPE: {
            // Load shader
            std::string file_data = molly::loadTextFile(str.c_str());
            TextFile file = {};
            file.data = file_data;
            // 2. Store the asset in the appropriate array
            handle = this->storeAsset(std::move(file));
        } break;
        
    }
    
    
    // 2. Store the asset in the appropriate array
    
    // 3. Cache the asset if it is completely
    // 4. Generate a handle for the loaded asset
    // 5. Store the asset handle in the active handle array at the next available slot
    // 6. Return the handle to caller
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