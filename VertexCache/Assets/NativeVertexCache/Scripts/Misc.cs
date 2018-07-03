using System;
using System.Runtime.InteropServices;

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
    }
}
