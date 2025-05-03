#ifndef MOLLY_CONTEXT_H
#define MOLLY_CONTEXT_H

#include "asset_registry.h"

struct RendererContext {
private:
    AssetRegistry private_asset_registry;
public:
    const AssetRegistry& asset_registry = private_asset_registry;

    // --------- Config goes here ------------
};

extern RendererContext g_context;

#endif // MOLLY_CONTEXT_H