using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NaiveVertexCache.Alembic
{
    public enum AspectRatioMode
    {
        CurrentResolution,
        DefaultResolution,
        CameraAperture
    };

    [Serializable]
    public class AlembicImportSettings
    {
        [SerializeField] public NormalsMode normals = NormalsMode.ComputeIfMissing;
        [SerializeField] public TangentsMode tangents = TangentsMode.Compute;
        [SerializeField] public AspectRatioMode cameraAspectRatio = AspectRatioMode.CameraAperture;

        [SerializeField] public float scaleFactor = 0.01f;
        [SerializeField] public bool swapHandedness = true;
        [SerializeField] public bool flipFaces = false;
        [SerializeField] public bool interpolateSamples = true;

        [SerializeField] public bool importPointPolygon = true;
        [SerializeField] public bool importLinePolygon = true;
        [SerializeField] public bool importTrianglePolygon = true;

        [SerializeField] public bool importVisibility = true;
        [SerializeField] public bool importCameras = true;
        [SerializeField] public bool importMeshes = true;
        [SerializeField] public bool importPoints = true;


        [SerializeField] public CompressionType compressionType = CompressionType.Quantize;
        [SerializeField] public int blockSize = 30;

        public AlembicImportOptions GetAlembicImportOptions()
        {
            return new AlembicImportOptions {
                normals_mode = normals,
                tangents_mode = tangents,
                scale_factor = scaleFactor,
                swap_handedness = swapHandedness,
                swap_face_winding = flipFaces,
                interpolate_samples = interpolateSamples,

                import_point_polygon = importPointPolygon,
                import_line_polygon = importLinePolygon,
                import_triangle_polygon = importTrianglePolygon,
                import_points = importPoints,
            };
        }

        public NvcExportOptions GetNvcExportOptions()
        {
            return new NvcExportOptions {
                compression_type = compressionType,
                block_size = blockSize,
            };
        }
    }

}
