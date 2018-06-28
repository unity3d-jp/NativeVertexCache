using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NaiveVertexCache
{
    public enum DataFormat
    {
        Unknown,
        Int,
        Int2,
        Int3,
        Int4,
        Float,
        Float2,
        Float3,
        Float4,
        Half,
        Half2,
        Half3,
        Half4,
        SNorm16,
        SNorm16x2,
        SNorm16x3,
        SNorm16x4,
        UNorm16,
        UNorm16x2,
        UNorm16x3,
        UNorm16x4,
    }

    enum Topology
    {
        Points,
        Lines,
        Triangles,
        Quads,
    }


    public struct GeomCacheDesc
    {
        public IntPtr semantic; // char*
        public DataFormat format;
    };

    public struct GeomSubmesh
    {
        public int indexOffset, indexCount;
        Topology topology;
    };

    public struct GeomMesh
    {
        public int vertexOffset, vertexCount;
        public int submeshOffset, submeshCount;
    };

    public struct GeomCacheData
    {
        public IntPtr indices;      // int*
        public IntPtr indexCount;   // size_t
        public IntPtr vertices;     // void**
        public IntPtr vertexCount;  // size_t
        public IntPtr meshes;       // GeomMesh*
        public IntPtr meshCount;    // size_t
        public IntPtr submeshes;    // GeomSubmesh*
        public IntPtr submeshCount; // size_t
    };


    // interface to export geometry from Unity to cache
    public class InputGeomCache
    {
        public IntPtr self;
        public static implicit operator bool(InputGeomCache v) { return v.self != IntPtr.Zero; }

        public static InputGeomCache Create(PinnedList<GeomCacheDesc> descs) { return nvcIGCCreate(descs); }

        public void Release() { nvcIGCRelease(self); self = IntPtr.Zero; }
        public void AddData(float time, PinnedList<GeomCacheData> data) { nvcIGCAddData(self, time, data); }

        #region internal
        [DllImport("NativeVertexCache")] static extern InputGeomCache nvcIGCCreate(IntPtr descs);
        [DllImport("NativeVertexCache")] static extern void nvcIGCRelease(IntPtr self);
        [DllImport("NativeVertexCache")] static extern void nvcIGCAddData(IntPtr self, float time, IntPtr data);
        #endregion
    }

    // interface to import geometry from cache to Unity
    public class OutputGeomCache
    {
        public IntPtr self;
        public static implicit operator bool(OutputGeomCache v) { return v.self != IntPtr.Zero; }

        public static OutputGeomCache Create() { return nvcOGCCreate(); }
        public void Release() { nvcOGCRelease(self); self = IntPtr.Zero; }

        public int meshCount { get { return nvcOGCGetMeshCount(self); } }
        public int submeshCount { get { return nvcOGCGetSubmeshCount(self); } }

        public void GetMesh(int index, ref GeomMesh dst) { nvcOGCGetMesh(self, index, ref dst); }
        public void GetSubmesh(int index, ref GeomSubmesh dst) { nvcOGCGetSubmesh(self, index, ref dst); }

        public bool FillIndices(ref GeomSubmesh subm, PinnedList<int> dst)
        {
            dst.ResizeDiscard(subm.indexCount);
            return nvcOGCCopyIndices(self, ref subm, dst);
        }
        public bool FillPoints(ref GeomMesh gm, PinnedList<Vector3> dst)
        {
            dst.ResizeDiscard(gm.vertexCount);
            return nvcOGCCopyPoints(self, ref gm, dst);
        }
        public bool FillNormals(ref GeomMesh gm, PinnedList<Vector3> dst)
        {
            dst.ResizeDiscard(gm.vertexCount);
            return nvcOGCCopyNormals(self, ref gm, dst);
        }
        public bool FillTangents(ref GeomMesh gm, PinnedList<Vector4> dst)
        {
            dst.ResizeDiscard(gm.vertexCount);
            return nvcOGCCopyTangents(self, ref gm, dst);
        }
        public bool FillUV0(ref GeomMesh gm, PinnedList<Vector2> dst)
        {
            dst.ResizeDiscard(gm.vertexCount);
            return nvcOGCCopyUV0(self, ref gm, dst);
        }
        public bool FillUV1(ref GeomMesh gm, PinnedList<Vector2> dst)
        {
            dst.ResizeDiscard(gm.vertexCount);
            return nvcOGCCopyUV1(self, ref gm, dst);
        }
        public bool FillColors(ref GeomMesh gm, PinnedList<Color> dst)
        {
            dst.ResizeDiscard(gm.vertexCount);
            return nvcOGCCopyColors(self, ref gm, dst);
        }

        #region internal
        [DllImport("NativeVertexCache")] static extern OutputGeomCache nvcOGCCreate();
        [DllImport("NativeVertexCache")] static extern void nvcOGCRelease(IntPtr self);

        [DllImport("NativeVertexCache")] static extern int nvcOGCGetMeshCount(IntPtr self);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetMesh(IntPtr self, int index, ref GeomMesh dst);
        [DllImport("NativeVertexCache")] static extern int nvcOGCGetSubmeshCount(IntPtr self);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetSubmesh(IntPtr self, int index, ref GeomSubmesh dst);

        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyIndices(IntPtr self, ref GeomSubmesh subm, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyPoints(IntPtr self, ref GeomMesh gm, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyNormals(IntPtr self, ref GeomMesh gm, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyTangents(IntPtr self, ref GeomMesh gm, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyUV0(IntPtr self, ref GeomMesh gm, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyUV1(IntPtr self, ref GeomMesh gm, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern bool nvcOGCCopyColors(IntPtr self, ref GeomMesh gm, IntPtr dst);
        #endregion
    }


    public class GeomCache
    {
        public IntPtr self;
        public static implicit operator bool(GeomCache v) { return v.self != IntPtr.Zero; }

        public static GeomCache Create() { return nvcGCCreate(); }
        public void Release() { nvcGCRelease(self); self = IntPtr.Zero; }

        public bool Open(string path) { return nvcGCOpen(self, path); }
        public void Close() { nvcGCClose(self); }

        public float time { set { nvcGCSetCurrentTime(self, value); } }
        public void Assign(OutputGeomCache ogc) { nvcGCGetCurrentCache(self, ogc); }


        #region internal
        [DllImport("NativeVertexCache")] static extern GeomCache nvcGCCreate();
        [DllImport("NativeVertexCache")] static extern void nvcGCRelease(IntPtr self);
        [DllImport("NativeVertexCache")] static extern bool nvcGCOpen(IntPtr self, string path);
        [DllImport("NativeVertexCache")] static extern void nvcGCClose(IntPtr self);
        [DllImport("NativeVertexCache")] static extern void nvcGCSetCurrentTime(IntPtr self, float time);
        [DllImport("NativeVertexCache")] static extern void nvcGCGetCurrentCache(IntPtr self, OutputGeomCache ogc);
        #endregion
    }


    public static class Misc
    {
        public static PinnedList<byte> cstr(string s)
        {
            return new PinnedList<byte>(System.Text.Encoding.ASCII.GetBytes(s));
        }
    }
}
