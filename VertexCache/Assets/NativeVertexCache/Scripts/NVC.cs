using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
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

    public struct GeomCacheDesc
    {
        public IntPtr semantic; // char*
        public DataFormat format;
    };

    public struct GeomCacheData
    {
        public IntPtr indices;     // void*
        public IntPtr indexCount;  // size_t
        public IntPtr vertices;    // void**
        public IntPtr vertexCount; // size_t
    };


    // interface to export geometry from Unity to cache
    public class InputGeomCache
    {
        public IntPtr self;
        public static implicit operator bool(InputGeomCache v) { return v.self != IntPtr.Zero; }

        public static InputGeomCache Create(PinnedList<GeomCacheDesc> descs) { return nvcIGCCreate(descs); }

        public void Release() { nvcIGCRelease(self); }
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
        public void Release() { nvcOGCRelease(self); }

        public int vertexCount { get { return nvcOGCGetVertexCount(self); } }
        public int indexCount { get { return nvcOGCGetIndexCount(self); } }

        public void FillIndices(PinnedList<int> dst) { dst.ResizeDiscard(indexCount); nvcOGCGetIndices(self, dst); }
        public void FillPoints(PinnedList<Vector3> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetPoints(self, dst); }
        public void FillNormals(PinnedList<Vector3> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetNormals(self, dst); }
        public void FillTangents(PinnedList<Vector4> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetTangents(self, dst); }
        public void FillUVs(PinnedList<Vector2> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetUVs(self, dst); }
        public void FillUVs(PinnedList<Color> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetColors(self, dst); }

        #region internal
        [DllImport("NativeVertexCache")] static extern OutputGeomCache nvcOGCCreate();
        [DllImport("NativeVertexCache")] static extern void nvcOGCRelease(IntPtr self);

        [DllImport("NativeVertexCache")] static extern int nvcOGCGetVertexCount(IntPtr self);
        [DllImport("NativeVertexCache")] static extern int nvcOGCGetIndexCount(IntPtr self);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetIndices(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetPoints(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetNormals(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetTangents(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetUVs(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcOGCGetColors(IntPtr self, IntPtr dst);
        #endregion
    }


    public class GeomCache
    {
        public IntPtr self;
        public static implicit operator bool(GeomCache v) { return v.self != IntPtr.Zero; }

        public static GeomCache Create() { return nvcGCCreate(); }
        public void Release() { nvcGCRelease(self); }

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

}
