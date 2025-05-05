#ifndef MOLLY_CONTEXT_H
#define MOLLY_CONTEXT_H

#include "asset_registry.h"


struct RendererContext {
    tmp::AssetRegistry asset_registry;

    void init();

    // --------- Config goes here ------------
    const char* cache_dir = "cache/";
    const char* asset_dir = "assets/";

};

extern RendererContext g_context;

#endif // MOLLY_CONTEXT_H