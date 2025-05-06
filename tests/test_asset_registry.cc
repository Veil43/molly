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
#include "textfile.h"
#include "test_utils.h"

TEST_CASE("RendererContext::init") {
    // Paths are set correctly
    g_context.init(false);
    const AssetRegistry& reg = g_context.asset_registry;
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

TEST_CASE("AssetRegistry::TextFile::Load: load 1 text file and VERIFY::HANDLE") {
    const std::string path = "funny_text_file.txt";
    const std::string bin_name = "funny_file";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);

    // verify 
    REQUIRE(reg.m_text_files.size() == 1);
    CHECK(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(handle.generation() == 0);
    CHECK(handle.index() == 0);
    
    removeFile(path);
    removeFile(reg.asset_dir + bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load/Get: load 1 text file and VERIFY::CONTENT") {
    const std::string path = "strange_text_file.txt";
    const std::string bin_name = "strange_file";
    writeToFile(path, STRANGE_STRING);
    
    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);

    REQUIRE(reg.m_text_files.size() == 1);
    CHECK(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(handle.generation() == 0);
    CHECK(handle.index() == 0);
    
    auto text_file_ptr = reg.getTextFile(handle);
    REQUIRE(text_file_ptr != nullptr);
    CHECK(text_file_ptr->name == bin_name);
    CHECK(text_file_ptr->content == STRANGE_STRING);

    removeFile(path);
    removeFile(reg.asset_dir + bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load: load 2 text files and VERIFY::HANDLES") {
    const std::string funny_path = "funny_text_file.txt";
    const std::string unfunny_path = "unfunny_text_file.txt";
    const std::string funny_bin_name = "funny_file";
    const std::string unfunny_bin_name = "unfunny_file";
    writeToFile(funny_path, FUNNY_STRING);
    writeToFile(unfunny_path, UNFUNNY_STRING);

    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle funny_handle = reg.loadAsset(funny_path, MOLLY_ASSET_TEXT_FILE_TYPE, funny_bin_name);

    REQUIRE(reg.m_text_files.size() == 1);
    CHECK(funny_handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(funny_handle.generation() == 0);
    CHECK(funny_handle.index() == 0);

    ARegHandle unfunny_handle = reg.loadAsset(unfunny_path, MOLLY_ASSET_TEXT_FILE_TYPE, unfunny_bin_name);
    REQUIRE(reg.m_text_files.size() == 2);
    CHECK(unfunny_handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(unfunny_handle.generation() == 0);
    CHECK(unfunny_handle.index() == 1);

    removeFile(funny_path);
    removeFile(reg.asset_dir + funny_bin_name + ".asset");
    removeFile(unfunny_path);
    removeFile(reg.asset_dir + unfunny_bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load/Get: load 2 text files and VERIFY::CONTENT") {
    const std::string funny_path = "funny_text_file.txt";
    const std::string unfunny_path = "unfunny_text_file.txt";
    const std::string funny_bin_name = "funny_file";
    const std::string unfunny_bin_name = "unfunny_file";
    writeToFile(funny_path, FUNNY_STRING);
    writeToFile(unfunny_path, UNFUNNY_STRING);

    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle funny_handle = reg.loadAsset(funny_path, MOLLY_ASSET_TEXT_FILE_TYPE, funny_bin_name);

    REQUIRE(reg.m_text_files.size() == 1);
    CHECK(funny_handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(funny_handle.generation() == 0);
    CHECK(funny_handle.index() == 0);

    ARegHandle unfunny_handle = reg.loadAsset(unfunny_path, MOLLY_ASSET_TEXT_FILE_TYPE, unfunny_bin_name);
    REQUIRE(reg.m_text_files.size() == 2);
    CHECK(unfunny_handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(unfunny_handle.generation() == 0);
    CHECK(unfunny_handle.index() == 1);

    auto funny_file_ptr = reg.getTextFile(funny_handle);
    REQUIRE(funny_file_ptr != nullptr);
    CHECK(funny_file_ptr->name == funny_bin_name);
    CHECK(funny_file_ptr->content == FUNNY_STRING);
    
    auto unfunny_file_ptr = reg.getTextFile(unfunny_handle);
    REQUIRE(unfunny_file_ptr != nullptr);
    CHECK(unfunny_file_ptr->name == unfunny_bin_name);
    CHECK(unfunny_file_ptr->content == UNFUNNY_STRING);

    removeFile(funny_path);
    removeFile(reg.asset_dir + funny_bin_name + ".asset");
    removeFile(unfunny_path);
    removeFile(reg.asset_dir + unfunny_bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load: load 1 text file TWICE and VERIFY::HANDLES") {
    const std::string path = "funny_text_file.txt";
    const std::string bin_name = "funny_file";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);

    REQUIRE(reg.m_text_files.size() == 1);
    CHECK(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(handle.generation() == 0);
    CHECK(handle.index() == 0);
    
    handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);
    REQUIRE(reg.m_text_files.size() == 2);
    CHECK(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(handle.generation() == 0);
    CHECK(handle.index() == 1);

    removeFile(path);
    removeFile(reg.asset_dir + bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load: load 1 text file TWICE and VERIFY::CONTENT") {
    const std::string path = "funny_text_file.txt";
    const std::string bin_name = "funny_file";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);

    REQUIRE(reg.m_text_files.size() == 1);
    CHECK(handle.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(handle.generation() == 0);
    CHECK(handle.index() == 0);
    
    ARegHandle handle2 = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);
    REQUIRE(reg.m_text_files.size() == 2);
    CHECK(handle2.type() == MOLLY_ASSET_TEXT_FILE_TYPE);
    CHECK(handle2.generation() == 0);
    CHECK(handle2.index() == 1);

    auto file_ptr1 = reg.getTextFile(handle);
    REQUIRE(file_ptr1 != nullptr);
    CHECK(file_ptr1->name == bin_name);
    CHECK(file_ptr1->content == FUNNY_STRING);

    auto file_ptr2 = reg.getTextFile(handle2);
    REQUIRE(file_ptr2 != nullptr);
    CHECK(file_ptr2->name == bin_name);
    CHECK(file_ptr2->content == FUNNY_STRING);

    removeFile(path);
    removeFile(reg.asset_dir + bin_name + ".asset");
}

// TODO: Add more TexFile test cases

TEST_CASE("AssetRegistry::TextFile::Load::Cache: load 1 text file and VERIFY::CACHE") {
    const std::string path = "unfunny_text_file.txt";
    const std::string bin_name = "unfunny_file";
    writeToFile(path, UNFUNNY_STRING);
    
    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, bin_name);
    REQUIRE(reg.m_asset_cache.size() > 0);
    CHECK(reg.m_asset_cache[path].first == reg.asset_dir + bin_name + ".asset");

    removeFile(path);
    removeFile(reg.asset_dir + bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load::Cache: load 2 text files and VERIFY::CACHE") {
    const std::string unfunny_path = "unfunny_text_file.txt";
    const std::string strange_path = "strange_text_file.txt";
    const std::string unfunny_bin_name = "unfunny_file";
    const std::string strange_bin_name = "strange_file";
    writeToFile(unfunny_path, UNFUNNY_STRING);
    writeToFile(strange_path, STRANGE_STRING);

    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    REQUIRE(reg.m_asset_cache.size() == 0);
    ARegHandle unfunny_handle = reg.loadAsset(unfunny_path, MOLLY_ASSET_TEXT_FILE_TYPE, unfunny_bin_name);
    REQUIRE(reg.m_asset_cache.size() == 1);
    CHECK(reg.m_asset_cache[unfunny_path].first == reg.asset_dir + unfunny_bin_name + ".asset");

    ARegHandle strange_handle = reg.loadAsset(strange_path, MOLLY_ASSET_TEXT_FILE_TYPE, strange_bin_name);
    REQUIRE(reg.m_asset_cache.size() == 2);
    CHECK(reg.m_asset_cache[strange_path].first == reg.asset_dir + strange_bin_name + ".asset");

    removeFile(unfunny_path);
    removeFile(strange_path);
    removeFile(reg.asset_dir + unfunny_bin_name + ".asset");
    removeFile(reg.asset_dir + strange_bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load::Cache: load 1 text file TWICE and VERIFY::CACHE") {
    const std::string strange_path = "strange_text_file.txt";
    const std::string strange_bin_name = "strange_file";
    writeToFile(strange_path, STRANGE_STRING);

    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    REQUIRE(reg.m_asset_cache.size() == 0);
    ARegHandle handle1 = reg.loadAsset(strange_path, MOLLY_ASSET_TEXT_FILE_TYPE, strange_bin_name);
    REQUIRE(reg.m_asset_cache.size() == 1);
    CHECK(reg.m_asset_cache[strange_path].first == reg.asset_dir + strange_bin_name + ".asset");

    removeFile(strange_path); // second time that I load it MUST come from the cache (HIT but file not there)
    ARegHandle handle2 = reg.loadAsset(strange_path, MOLLY_ASSET_TEXT_FILE_TYPE, strange_bin_name);
    REQUIRE(reg.m_asset_cache.size() == 0);

    // Make sure the cache for this path has nothing
    REQUIRE(reg.m_asset_cache.find(strange_path) == reg.m_asset_cache.end());

    removeFile(reg.asset_dir + strange_bin_name + ".asset");
}

TEST_CASE("AssetRegistry::TextFile::Load::Cache: load 1 text file, delete original, relaunch without save, VERIFY::CACHE") {
    const std::string path = "tmp.txt";
    const std::string name = "tmp_file";
    writeToFile(path, FUNNY_STRING);
    
    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);
    REQUIRE(reg.m_asset_cache.size() == 1);    
    
    removeFile(path);
    g_context.init(false);
    REQUIRE(reg.m_asset_cache.size() == 0);
    handle = reg.loadAsset(path, MOLLY_ASSET_TEXT_FILE_TYPE, name);
    REQUIRE(handle.value == 0);
}

TEST_CASE("AssetRegistry::TextFile::Cache::Store: store and load cache of 3 files") {
    const std::string fpath = "ffile.txt";
    const std::string upath = "ufile.txt";
    const std::string spath = "sfile.txt";
    writeToFile(fpath, FUNNY_STRING);
    writeToFile(upath, UNFUNNY_STRING);
    writeToFile(spath, STRANGE_STRING);

    g_context.init(false);
    AssetRegistry& reg = g_context.asset_registry;
    ARegHandle fhandle = reg.loadAsset(fpath, MOLLY_ASSET_TEXT_FILE_TYPE, "funny");
    ARegHandle uhandle = reg.loadAsset(upath, MOLLY_ASSET_TEXT_FILE_TYPE, "unfunny");
    ARegHandle shandle = reg.loadAsset(spath, MOLLY_ASSET_TEXT_FILE_TYPE, "strange");

    auto fptr = reg.getTextFile(fhandle);
    auto uptr = reg.getTextFile(uhandle);
    auto sptr = reg.getTextFile(shandle);
    REQUIRE(fptr != nullptr);
    REQUIRE(uptr != nullptr);
    REQUIRE(sptr != nullptr);
    CHECK(fptr->content == FUNNY_STRING);
    CHECK(uptr->content == UNFUNNY_STRING);
    CHECK(sptr->content == STRANGE_STRING);

    // removeFile(fpath);
    // removeFile(upath);
    // removeFile(spath);
    reg.storeCache(reg.cache_dir + "cache.bin");

    g_context.init(true);

    fhandle = reg.loadAsset(fpath, MOLLY_ASSET_TEXT_FILE_TYPE, "funny");
    uhandle = reg.loadAsset(upath, MOLLY_ASSET_TEXT_FILE_TYPE, "unfunny");
    shandle = reg.loadAsset(spath, MOLLY_ASSET_TEXT_FILE_TYPE, "strange");

    // Second fetch
    fptr = reg.getTextFile(fhandle);
    uptr = reg.getTextFile(uhandle);
    sptr = reg.getTextFile(shandle);
    REQUIRE(fptr != nullptr);
    REQUIRE(uptr != nullptr);
    REQUIRE(sptr != nullptr);
    CHECK(fptr->content == FUNNY_STRING);
    CHECK(uptr->content == UNFUNNY_STRING);
    CHECK(sptr->content == STRANGE_STRING);

    removeFile(reg.cache_dir + "cache.bin");
    removeFile(reg.asset_dir + "funny.asset");
    removeFile(reg.asset_dir + "unfunny.asset");
    removeFile(reg.asset_dir + "strange.asset");
}