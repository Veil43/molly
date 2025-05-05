#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <bitset>
#include <filesystem>
#include <memory>

#include "context.h"
#include "types.h"

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
    
TEST_CASE("RendererContext::init") {
    // Paths are set correctly
    g_context.init();
    const tmp::AssetRegistry& reg = g_context.asset_registry;
    CHECK(reg.asset_dir == g_context.asset_dir);
    CHECK(reg.cache_dir == g_context.cache_dir);
    // Free handles vector init correctly
    CHECK(reg.m_free_handle_indices.size() == 7);
    CHECK(reg.m_free_handle_indices[MOLLY_ASSET_TEXT_FILE_TYPE].size() == 0);
    CHECK(reg.m_free_handle_indices[MOLLY_ASSET_IMAGE_TYPE].size() == 0);
    CHECK(reg.m_free_handle_indices[MOLLY_ASSET_TEXTURE_TYPE].size() == 0);
    CHECK(reg.m_free_handle_indices[MOLLY_ASSET_MATERIAL_TYPE].size() == 0);
    CHECK(reg.m_free_handle_indices[MOLLY_ASSET_STATIC_MESH_TYPE].size() == 0);
    CHECK(reg.m_free_handle_indices[MOLLY_ASSET_MODEL_TYPE].size() == 0);
}

TEST_CASE("Serialisation: Textfile read/write to binary") {
    tmp::TextFile expected = {};
    expected.data = FUNNY_STRING;
    expected.name = "funny_file.bin";

    // TODO: Add more types of file. tbh it shouldn't really matter but who knows.
    writeTextFileToBinary(expected, expected.name);
    auto file = readTextFileFromBinary(expected.name);

    CHECK(file.name == expected.name);
    CHECK(file.data == expected.data);

    removeFile("funny_file.bin");
}

TEST_CASE("AssetRegistry, TextFile: Load 1 TextFile asset to registry") {
    const std::string path = "tmp.txt";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init();
    tmp::AssetRegistry& reg = g_context.asset_registry;
    tmp::ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, "funny_string");
    // is a text file loaded?
    CHECK(reg.m_text_files.size() == 1);
    // is it OURS?
    auto ffrom_reg = reg.getTextFile(handle);
    REQUIRE(ffrom_reg != nullptr);
    CHECK(ffrom_reg->data == FUNNY_STRING);

    removeFile(path);
}

TEST_CASE("AssetRegistry, TextFile: Load 2 TextFile assets to registry") {
    const std::string path1 = "funny.txt";
    const std::string path2 = "unfunny.txt";
    const std::string name1 = "funny";
    const std::string name2 = "unfunny";
    writeToFile(path1, FUNNY_STRING);
    writeToFile(path2, UNFUNNY_STRING);

    g_context.init();
    tmp::AssetRegistry& reg = g_context.asset_registry;
    auto funny_handle = reg.loadAsset(path1, MOLLY_ASSET_TEXT_FILE_TYPE, name1);
    auto funny_text_file = reg.getTextFile(funny_handle);
    REQUIRE(funny_text_file != nullptr);
    CHECK(funny_text_file->data == FUNNY_STRING);

    auto unfunny_handle = reg.loadAsset(path2, MOLLY_ASSET_TEXT_FILE_TYPE, name2);
    auto unfunny_text_file = reg.getTextFile(unfunny_handle);
    REQUIRE(unfunny_text_file != nullptr);
    CHECK(unfunny_text_file->data == UNFUNNY_STRING);

    removeFile(path1);
    removeFile(path2);
}

TEST_CASE("AssetRegistry, TextFile: Load 1 TextFile and cache") {
    const std::string path = "tmp.txt";
    const std::string name = "funny_string";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init();
    tmp::AssetRegistry& reg = g_context.asset_registry;
    tmp::ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);
    REQUIRE(reg.m_asset_cache.size() > 0);
    CHECK(reg.m_asset_cache[path] == reg.asset_dir + name + ".asset");

    removeFile(path);
}

TEST_CASE("RendererContext, AssetRegistry: Init resets cache") {
    const std::string path = "tmp.txt";
    const std::string name = "funny_string";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init();
    tmp::AssetRegistry& reg = g_context.asset_registry;
    tmp::ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);
    REQUIRE(reg.m_asset_cache.size() > 0);
    g_context.init();
    REQUIRE(reg.m_asset_cache.size() <= 0);

    removeFile(path);
}

TEST_CASE("AssetRegistry: asset serialization AND caching") {
    const std::string path = "tmp.txt";
    const std::string name = "funny_file";
    writeToFile(path, FUNNY_STRING);

    g_context.init();
    tmp::AssetRegistry& reg = g_context.asset_registry;
    tmp::ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);

    REQUIRE(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    REQUIRE(reg.m_asset_cache.size() > 0);

    auto bin_path = reg.writeAssetToBinary(handle);
    auto new_handle = reg.readAssetFromBinary(bin_path, MOLLY_ASSET_TEXT_FILE_TYPE);

    auto text_file = reg.getTextFile(new_handle);
    REQUIRE(text_file != nullptr);
    CHECK(text_file->data == FUNNY_STRING);

    removeFile(path);
    removeFile(bin_path);
}

TEST_CASE("AssetRegistry: cach save/load") {
    const std::string path = "strange.txt";
    const std::string name = "strange_file";
    writeToFile(path, STRANGE_STRING);

    g_context.init();
    tmp::AssetRegistry& reg = g_context.asset_registry;
    tmp::ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);
    removeFile(path);
 
    reg.storeCache(reg.cache_dir + "cache.bin");
    g_context.init();
    handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);
    REQUIRE(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    REQUIRE(reg.m_asset_cache.size() > 0);
    auto text_file = reg.getTextFile(handle);
    REQUIRE(text_file != nullptr);
    CHECK(text_file->data == STRANGE_STRING);

    removeFile(reg.asset_dir + name + ".asset");
}