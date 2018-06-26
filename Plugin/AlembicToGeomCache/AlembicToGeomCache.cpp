#include "Plugin/PrecompiledHeader.h"
#include "AlembicToGeomCache.h"
#include "../../External/abci/abci.h"

namespace nvc {

static void ProcessAbcObject(aiObject *obj)
{
    if (!obj)
        return;

    aiPolyMesh *poly = aiObjectAsPolyMesh(obj);
    if (poly) {
        // todo
    }

    // process children
    int num_children = aiObjectGetNumChildren(obj);
    for (int ci = 0; ci < num_children; ++ci) {
        ProcessAbcObject(aiObjectGetChild(obj, ci));
    }
}

bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& options)
{
    aiContext* ctx = aiContextCreate(1);
    if (!aiContextLoad(ctx, path_to_abc)) {
        aiContextDestroy(ctx);
        return false;
    }

    ProcessAbcObject(aiContextGetTopObject(ctx));

    aiContextDestroy(ctx);
    return true;
}

} // namespace nvc
