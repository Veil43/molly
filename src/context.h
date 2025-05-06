#ifndef MOLLY_CONTEXT_H
#define MOLLY_CONTEXT_H

#include "asset_registry.h"


struct RendererContext {
    AssetRegistry asset_registry;

    void init(bool load_cache = true);

    // --------- Config goes here ------------
    const char* cache_dir = "cache/";
    const char* asset_dir = "assets/";

};

extern RendererContext g_context;

#endif // MOLLY_CONTEXT_H