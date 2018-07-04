#if UNITY_2017_1_OR_NEWER

using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using UnityEditor;
using UnityEngine;
using UnityEditor.Experimental.AssetImporters;
using Object = UnityEngine.Object;

namespace NaiveVertexCache.Alembic
{
    public class AlembicAssetModificationProcessor : UnityEditor.AssetModificationProcessor
    {
        public static AssetDeleteResult OnWillDeleteAsset(string assetPath, RemoveAssetOptions rao)
        {
            if (Path.GetExtension(assetPath.ToLower()) != ".abc")
                return AssetDeleteResult.DidNotDelete;

            try
            {
                var streamingAssetPath = AlembicImporter.MakeShortAssetPath(assetPath);
                var fullStreamingAssetPath = Application.streamingAssetsPath + streamingAssetPath;
                File.SetAttributes(fullStreamingAssetPath, FileAttributes.Normal);
                File.Delete(fullStreamingAssetPath);
                File.SetAttributes(fullStreamingAssetPath + ".meta", FileAttributes.Normal);
                File.Delete(fullStreamingAssetPath + ".meta");
            }
            catch (System.Exception e)
            {
                Debug.LogWarning(e);
            }

            return AssetDeleteResult.DidNotDelete;
        }

        public static AssetMoveResult OnWillMoveAsset(string from, string to)
        {
            if (Path.GetExtension(from.ToLower()) != ".abc")
                return AssetMoveResult.DidNotMove;

            try
            {
                var streamDstPath = AlembicImporter.MakeShortAssetPath(to);
                var streamSrcPath = AlembicImporter.MakeShortAssetPath(from);
                var dstPath = Application.streamingAssetsPath + streamDstPath;
                var srcPath = Application.streamingAssetsPath + streamSrcPath;

                var directoryPath = Path.GetDirectoryName(dstPath);
                if (File.Exists(dstPath))
                {
                    File.SetAttributes(dstPath + ".meta", FileAttributes.Normal);
                    File.Delete(dstPath);
                }
                else if (!Directory.Exists(directoryPath))
                {
                    Directory.CreateDirectory(directoryPath);
                }
                if (File.Exists(dstPath))
                    File.SetAttributes(dstPath, FileAttributes.Normal);
                File.Move(srcPath, dstPath);
                if (File.Exists(dstPath + ".meta"))
                {
                    File.SetAttributes(dstPath + ".meta", FileAttributes.Normal);
                    File.Move(srcPath + ".meta", dstPath + ".meta");
                }

                AssetDatabase.Refresh(ImportAssetOptions.Default);
            }
            catch (System.Exception e)
            {
                Debug.LogWarning(e);
            }

            return AssetMoveResult.DidNotMove;
        }

    }

    [ScriptedImporter(1, "abc", -100)]
    public class AlembicImporter : ScriptedImporter
    {
        [SerializeField] public AlembicImportSettings settings = new AlembicImportSettings();

        public static string MakeShortAssetPath(string assetPath)
        {
            return Regex.Replace(assetPath, "^Assets", "");
        }

        public override void OnImportAsset(AssetImportContext ctx)
        {
            // todo
        }
    }

}

#endif
