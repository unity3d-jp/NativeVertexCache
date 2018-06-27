using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace NaiveVertexCache
{

    public class GeomCahcePlayer : MonoBehaviour
    {
        PinnedList<int> m_indices = new PinnedList<int>();
        PinnedList<Vector3> m_points = new PinnedList<Vector3>();
        PinnedList<Vector3> m_normals = new PinnedList<Vector3>();
        PinnedList<Vector4> m_tangents = new PinnedList<Vector4>();
        PinnedList<Vector2> m_uv0 = new PinnedList<Vector2>();
        PinnedList<Vector4> m_colors = new PinnedList<Vector4>();

        GeomCache m_geomcache;


        public void UpdateMesh(Mesh dst)
        {
        }
    }

}
