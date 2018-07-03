using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace NaiveVertexCache
{
    public static class Misc
    {
        public static PinnedList<byte> cstr(string s)
        {
            return new PinnedList<byte>(System.Text.Encoding.ASCII.GetBytes(s));
        }

        public static string S(IntPtr cstring)
        {
            return cstring == IntPtr.Zero ? "" : Marshal.PtrToStringAnsi(cstring);
        }

        // root can be null. in that case, search top level objects.
        public static Transform FindOrCreateObjectByPath(Transform root, string path, bool createIfNotExist, ref bool created)
        {
            var names = path.Split('/');
            Transform t = root;
            foreach (var name in names)
            {
                if (name.Length == 0) { continue; }
                t = FindOrCreateObjectByName(t, name, createIfNotExist, ref created);
                if (t == null) { break; }
            }
            return t;
        }

        public static Transform FindOrCreateObjectByName(Transform parent, string name, bool createIfNotExist, ref bool created)
        {
            Transform ret = null;
            if (parent == null)
            {
                var roots = UnityEngine.SceneManagement.SceneManager.GetActiveScene().GetRootGameObjects();
                foreach (var go in roots)
                {
                    if (go.name == name)
                    {
                        ret = go.GetComponent<Transform>();
                        break;
                    }
                }
            }
            else
            {
                ret = parent.Find(name);
            }

            if (createIfNotExist && ret == null)
            {
                var go = new GameObject();
                go.name = name;
                ret = go.GetComponent<Transform>();
                if (parent != null)
                    ret.SetParent(parent, false);
                created = true;
            }
            return ret;
        }

    }
}
