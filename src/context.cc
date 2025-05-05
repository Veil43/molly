#include "context.h"

// Behold! The God Object!
RendererContext g_context;

void RendererContext::init() {
    g_context.asset_registry = {};
    g_context.asset_registry.init(this->cache_dir, this->asset_dir);
}