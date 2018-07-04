#include "Plugin/PrecompiledHeader.h"
#include "./AlembicToGeomCache.h"

nvcabcAPI nvc::InputGeomCache* nvcabcAlembicToInputGeomCache(const char *path_to_abc, const nvcabc::ImportOptions& options)
{
    nvcabc::ImportContext ic;
    if (!ic.open(path_to_abc, options)) {
        return false;
    }
    ic.gatherTimes();
    ic.gatherMeshes();
    auto ret = ic.gatherSamples();
    return ret;
}


nvcabcAPI nvcabc::ImportContext* nvcabcCreateContext()
{
    return new nvcabc::ImportContext();
}
nvcabcAPI void nvcabcReleaseContext(nvcabc::ImportContext *self)
{
    delete self;
}
nvcabcAPI int nvcabcOpen(nvcabc::ImportContext *self, const char *path_to_abc, const nvcabc::ImportOptions* options)
{
    if (self) {
        return self->open(path_to_abc, *options);
    }
    return false;
}

// convert and export to file
nvcabcAPI int nvcabcExportNVC(nvcabc::ImportContext *self, const char *path_to_nvc, const nvcabc::ExportOptions* options)
{
    if (self) {
        //// Import -> output .nvc
        //nvcabc::ImportOptions opt;

        //const auto abcIgc = nvcabcAlembicToInputGeomCache(abcFilename, opt);
        //assert(abcIgc);

        //nvc::FileStream fs{ nvcFilename, FileStream::OpenModes::Random_ReadWrite };
        //nvc::Compressor nc{};
        //nc.compress(*abcIgc, &fs);

        //nvcIGCRelease(abcIgc);
    }
    return false;
}

nvcabcAPI int nvcabcGetNodeCount(nvcabc::ImportContext *self)
{
    if (self) {
        return  (int)self->m_abc_nodes.size();
    }
    return 0;
}
nvcabcAPI const char* nvcabcGetNodeName(nvcabc::ImportContext *self, int i)
{
    if (self) {
        return aiObjectGetName(self->m_abc_nodes[i]);
    }
    return "";
}
nvcabcAPI const char* nvcabcGetNodePath(nvcabc::ImportContext *self, int i)
{
    if (self) {
        return aiObjectGetFullName(self->m_abc_nodes[i]);
    }
    return "";
}
nvcabcAPI nvcabc::NodeType nvcabcGetNodeType(nvcabc::ImportContext *self, int i)
{
    if (self) {
        auto obj = self->m_abc_nodes[i];
        if (aiObjectAsXform(obj)) { return nvcabc::NodeType::Xform; }
        if (aiObjectAsCamera(obj)) { return nvcabc::NodeType::Camera; }
        if (aiObjectAsPolyMesh(obj)) { return nvcabc::NodeType::Mesh; }
        if (aiObjectAsPoints(obj)) { return nvcabc::NodeType::Points; }
    }
    return nvcabc::NodeType::Unknwon;
}
nvcabcAPI int nvcabcGetSampleCount(nvcabc::ImportContext *self, int /*i*/)
{
    if (self) {
        (int)self->m_timesamples.size();
    }
    return 0;
}
nvcabcAPI int nvcabcFillXformSamples(nvcabc::ImportContext *self, int i, nvcabc::XformData *dst)
{
    if (self) {
        auto obj = self->m_abc_nodes[i];
        // todo
    }
    return false;
}
nvcabcAPI int nvcabcFillCameraSamples(nvcabc::ImportContext *self, int i, nvcabc::CameraData *dst)
{
    if (self) {
        auto obj = self->m_abc_nodes[i];
        // todo
    }
    return false;
}
