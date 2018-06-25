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

        public int vertexCount { get { return nvcGetVertexCount(self); } }
        public int indexCount { get { return nvcGetIndexCount(self); } }

        public void FillIndices(PinnedList<int> dst) { dst.ResizeDiscard(indexCount); nvcFillIndices(self, dst); }
        public void FillPoints(PinnedList<Vector3> dst) { dst.ResizeDiscard(vertexCount); nvcFillPoints(self, dst); }
        public void FillNormals(PinnedList<Vector3> dst) { dst.ResizeDiscard(vertexCount); nvcFillNormals(self, dst); }
        public void FillTangents(PinnedList<Vector4> dst) { dst.ResizeDiscard(vertexCount); nvcFillTangents(self, dst); }
        public void FillUVs(PinnedList<Vector2> dst) { dst.ResizeDiscard(vertexCount); nvcFillUVs(self, dst); }
        public void FillUVs(PinnedList<Color> dst) { dst.ResizeDiscard(vertexCount); nvcFillColors(self, dst); }

        #region internal
        [DllImport("NativeVertexCache")] static extern int nvcGetVertexCount(IntPtr self);
        [DllImport("NativeVertexCache")] static extern int nvcGetIndexCount(IntPtr self);
        [DllImport("NativeVertexCache")] static extern void nvcFillIndices(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcFillPoints(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcFillNormals(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcFillTangents(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcFillUVs(IntPtr self, IntPtr dst);
        [DllImport("NativeVertexCache")] static extern void nvcFillColors(IntPtr self, IntPtr dst);
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
