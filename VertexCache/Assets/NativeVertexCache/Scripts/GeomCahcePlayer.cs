using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace NaiveVertexCache
{

    public class GeomCahcePlayer : MonoBehaviour
    {
        [SerializeField] string m_path;
        [SerializeField] float m_time;
        float m_timePrev = float.MinValue;

        PinnedList<int> m_indices = new PinnedList<int>();
        PinnedList<Vector3> m_points = new PinnedList<Vector3>();
        PinnedList<Vector3> m_normals = new PinnedList<Vector3>();
        PinnedList<Vector4> m_tangents = new PinnedList<Vector4>();
        PinnedList<Vector2> m_uv0 = new PinnedList<Vector2>();
        PinnedList<Vector2> m_uv1 = new PinnedList<Vector2>();
        PinnedList<Color> m_colors = new PinnedList<Color>();

        GeomCache m_gc;
        OutputGeomCache m_ogc;

        public float time
        {
            get { return m_time; }
            set { m_time = value; }
        }


        bool Open(string path)
        {
            if (!m_gc)
                m_gc = GeomCache.Create();
            return m_gc.Open(path);
        }

        void Close()
        {
            m_gc.Release();
            m_ogc.Release();
        }

        void UpdateMesh(ref GeomMesh gm, Mesh dst)
        {
            // todo: submeshes

            if (m_ogc.FillPoints(ref gm, m_points))
                dst.SetVertices(m_points.List);
            if (m_ogc.FillNormals(ref gm, m_normals))
                dst.SetNormals(m_normals.List);
            if (m_ogc.FillTangents(ref gm, m_tangents))
                dst.SetTangents(m_tangents.List);
            if (m_ogc.FillUV0(ref gm, m_uv0))
                dst.SetUVs(0, m_uv0.List);
            if (m_ogc.FillUV1(ref gm, m_uv1))
                dst.SetUVs(1, m_uv0.List);
            if (m_ogc.FillColors(ref gm, m_colors))
                dst.SetColors(m_colors.List);
        }

        void UpdateMeshes()
        {
            if (!m_ogc)
                m_ogc = OutputGeomCache.Create();

            m_gc.Assign(m_ogc);
            // todo
        }


        #region messages
        void OnDestroy()
        {
            Close();
        }

        void Update()
        {
            if (m_time != m_timePrev)
            {
                m_gc.time = m_time;
                UpdateMeshes();
                m_timePrev = m_time;
            }
        }
        #endregion
    }

}
