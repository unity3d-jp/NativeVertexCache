using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NaiveVertexCache.Alembic
{
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
    }

}
