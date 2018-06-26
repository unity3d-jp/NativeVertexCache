using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using UnityEngine;

namespace NaiveVertexCache
{
    class OutputGeomCache
    {
        public IntPtr self;
        public static implicit operator bool(OutputGeomCache v) { return v.self != IntPtr.Zero; }

        public int vertexCount { get { return nvcOGCGetVertexCount(self); } }
        public int indexCount { get { return nvcOGCGetIndexCount(self); } }

        public void FillIndices(PinnedList<int> dst) { dst.ResizeDiscard(indexCount); nvcOGCGetIndices(self, dst); }
        public void FillPoints(PinnedList<Vector3> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetPoints(self, dst); }
        public void FillNormals(PinnedList<Vector3> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetNormals(self, dst); }
        public void FillTangents(PinnedList<Vector4> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetTangents(self, dst); }
        public void FillUVs(PinnedList<Vector2> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetUVs(self, dst); }
        public void FillUVs(PinnedList<Color> dst) { dst.ResizeDiscard(vertexCount); nvcOGCGetColors(self, dst); }

        #region internal
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


    class GeomCache
    {
        public IntPtr self;
        public static implicit operator bool(GeomCache v) { return v.self != IntPtr.Zero; }

        public float time { set { nvcSeek(self, value); } }
        public OutputGeomCache currentCache { get { return nvcGetCurrentCache(self); } }


        #region internal
        [DllImport("NativeVertexCache")] static extern OutputGeomCache nvcSeek(IntPtr self, float time);
        [DllImport("NativeVertexCache")] static extern OutputGeomCache nvcGetCurrentCache(IntPtr self);
        #endregion

    }

}
