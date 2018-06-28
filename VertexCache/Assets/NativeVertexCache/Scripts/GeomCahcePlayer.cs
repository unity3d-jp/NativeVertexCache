using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace NaiveVertexCache
{
    //[ExecuteInEditMode]
    public class GeomCahcePlayer : MonoBehaviour
    {
        [SerializeField] string m_path;
        [SerializeField] float m_time;
        float m_timePrev = float.MinValue;

        PinnedList<int>     m_indices = new PinnedList<int>();
        PinnedList<Vector3> m_points = new PinnedList<Vector3>();
        PinnedList<Vector3> m_normals = new PinnedList<Vector3>();
        PinnedList<Vector4> m_tangents = new PinnedList<Vector4>();
        PinnedList<Vector2> m_uv0 = new PinnedList<Vector2>();
        PinnedList<Vector2> m_uv1 = new PinnedList<Vector2>();
        PinnedList<Color>   m_colors = new PinnedList<Color>();

        GeomCache m_gc;
        OutputGeomCache m_ogc;

        public float time
        {
            get { return m_time; }
            set { m_time = value; }
        }


        bool OpenNVC()
        {
            if (!m_gc && m_path != null && m_path.Length > 0)
            {
                m_gc = GeomCache.Create();
                if (!m_gc.Open(m_path))
                {
                    CloseNVC();
                    return false;
                }
            }
            return true;
        }

        void CloseNVC()
        {
            m_gc.Release();
            m_ogc.Release();
        }

        void UpdateMesh(ref GeomMesh gm, Mesh dst)
        {
            dst.Clear();

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

            int si = 0;
            var subm = new GeomSubmesh();
            for (int smi = 0; smi < gm.submeshCount; ++smi)
            {
                m_ogc.GetSubmesh(gm.submeshOffset + smi, ref subm);
                if(subm.topology == Topology.Triangles)
                {
                    m_ogc.FillIndices(ref subm, m_indices);
                    dst.SetTriangles(m_indices.List, si++);
                }
            }
        }

        Mesh FindOrAddMesh(string name)
        {
            var trans = GetComponent<Transform>();
            var child = trans.Find(name);
            if (child == null)
            {
                var go = new GameObject();
                go.name = name;
                child = go.GetComponent<Transform>();
                child.SetParent(trans);
            }

            var cgo = child.gameObject;

            var mf = cgo.GetComponent<MeshFilter>();
            if (mf == null)
                mf = cgo.AddComponent<MeshFilter>();

            var mesh = mf.sharedMesh;
            if (mesh == null)
            {
                mesh = new Mesh();
                mesh.name = cgo.name;
                mf.sharedMesh = mesh;
            }

            var mr = cgo.GetComponent<MeshRenderer>();
            if (mr == null)
                mr = cgo.AddComponent<MeshRenderer>();
            if (mr.sharedMaterial == null)
                mr.sharedMaterial = new Material(Shader.Find("Standard"));

            return mesh;
        }

        void UpdateMeshes()
        {
            if (!m_ogc)
                m_ogc = OutputGeomCache.Create();

            if (m_gc.Assign(m_ogc))
            {
                var gm = new GeomMesh();
                int meshCount = m_ogc.meshCount;
                for (int mi = 0; mi < meshCount; ++mi)
                {
                    m_ogc.GetMesh(mi, ref gm);
                    UpdateMesh(ref gm, FindOrAddMesh(mi.ToString()));
                }
            }
        }


        #region messages
        void OnDestroy()
        {
            CloseNVC();

            m_indices.Dispose();
            m_points.Dispose();
            m_normals.Dispose();
            m_tangents.Dispose();
            m_uv0.Dispose();
            m_uv1.Dispose();
            m_colors.Dispose();
        }

        void Update()
        {
            if (m_time != m_timePrev)
            {
                if (OpenNVC())
                {
                    m_gc.time = m_time;
                    UpdateMeshes();
                    m_timePrev = m_time;
                }
            }
        }
        #endregion
    }

}
