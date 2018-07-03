using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NaiveVertexCache.Alembic
{
    public enum NormalsMode
    {
        ReadFromFile,
        ComputeIfMissing,
        AlwaysCompute,
        Ignore
    };

    public enum TangentsMode
    {
        None,
        Compute,
    };

    public enum NodeType
    {
        Unknwon,
        Xform,
        Camera,
        Light,
        Mesh,
        Points,
    };

    public struct ImportOptions
    {
        // options for abci
        public NormalsMode normals_mode;
        public TangentsMode tangents_mode;
        public float scale_factor;
        public float aspect_ratio;
        public float vertex_motion_scale;
        public int split_unit;
        public Bool swap_handedness;
        public Bool swap_face_winding;
        public Bool interpolate_samples;
        public Bool turn_quad_edges;
        public Bool multithreading;

        public Bool import_point_polygon;
        public Bool import_line_polygon;
        public Bool import_triangle_polygon;

        // followings are extended options for nvcabc
        public Bool import_points;

        public static ImportOptions default_value {
            get
            {
                return new ImportOptions
                {
                    normals_mode = NormalsMode.ComputeIfMissing,
                    tangents_mode = TangentsMode.Compute,
                    scale_factor = 1.0f,
                    aspect_ratio = -1.0f,
                    vertex_motion_scale = -1.0f,
                    split_unit = 0x7fffffff,
                    swap_handedness = true,
                    swap_face_winding = false,
                    interpolate_samples = true,
                    turn_quad_edges = false,
                    multithreading = true,
                    import_point_polygon = true,
                    import_line_polygon = true,
                    import_triangle_polygon = true,
                    import_points = true,
                };
            }
        }
    };

    public struct ExportOptions
    {
        // compression settings
        public CompressionType compression_type;
        public int block_size;

        public static ExportOptions default_value
        {
            get
            {
                return new ExportOptions
                {
                    compression_type = CompressionType.Quantize,
                    block_size = 30,
                };
            }
        }
    };

    public struct XformData
    {
        float time;

        bool visibility;
        Vector3 translation;
        Quaternion rotation;
        Vector3 scale;
    };

    public struct CameraData
    {
        float time;

        Bool visibility;
        float near_clipping_plane;
        float far_clipping_plane;
        float field_of_view;
        float aspect_ratio;

        float focus_distance;
        float focal_length;
        float aperture;
    };


    public struct ImportContext
    {
        public IntPtr self;
        public static implicit operator bool(ImportContext v) { return v.self != IntPtr.Zero; }

        public static ImportContext Create() { return nvcabcCreateContext(); }

        public void Release() { nvcabcReleaseContext(self); self = IntPtr.Zero; }
        public bool Open(string path_to_abc, ref ImportOptions opt) { return nvcabcOpen(self, path_to_abc, ref opt); }

        public bool ExportNVC(string path_to_nvc, ref ExportOptions opt) { return nvcabcExportNVC(self, path_to_nvc, ref opt); }

        public int nodeCount { get { return nvcabcGetNodeCount(self); } }
        public string GetNodeName(int i) { return Misc.S(nvcabcGetNodeName(self, i)); }
        public string GetNodePath(int i) { return Misc.S(nvcabcGetNodePath(self, i)); }
        public NodeType GetNodeType(int i) { return nvcabcGetNodeType(self, i); }
        public bool FillXformSamples(int i, PinnedList<XformData> dst) { return nvcabcFillXformSamples(self, i, dst); }
        public bool FillCameraSamples(int i, PinnedList<CameraData> dst) { return nvcabcFillCameraSamples(self, i, dst); }

        #region internal
        [DllImport("AlembicToGeomCache")] static extern ImportContext nvcabcCreateContext();
        [DllImport("AlembicToGeomCache")] static extern void nvcabcReleaseContext(IntPtr self);
        [DllImport("AlembicToGeomCache")] static extern bool nvcabcOpen(IntPtr self, string path_to_abc, ref ImportOptions opt);

        [DllImport("AlembicToGeomCache")] static extern bool nvcabcExportNVC(IntPtr self, string path_to_nvc, ref ExportOptions opt);

        [DllImport("AlembicToGeomCache")] static extern int nvcabcGetNodeCount(IntPtr self);
        [DllImport("AlembicToGeomCache")] static extern IntPtr nvcabcGetNodeName(IntPtr self, int i);
        [DllImport("AlembicToGeomCache")] static extern IntPtr nvcabcGetNodePath(IntPtr self, int i);
        [DllImport("AlembicToGeomCache")] static extern NodeType nvcabcGetNodeType(IntPtr self, int i);
        [DllImport("AlembicToGeomCache")] static extern int nvcabcGetSampleCount(IntPtr self, int i);
        [DllImport("AlembicToGeomCache")] static extern bool nvcabcFillXformSamples(IntPtr self, int i, IntPtr dst);
        [DllImport("AlembicToGeomCache")] static extern bool nvcabcFillCameraSamples(IntPtr self, int i, IntPtr dst);
        #endregion
    }

}
