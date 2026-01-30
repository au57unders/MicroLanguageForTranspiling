using System.Runtime.InteropServices;

public static class MLTBridge
{
    const string DllPath = "Backend.dll";

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern IntPtr TranspileLine(string input, string lang);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern IntPtr TranspileFile(string inputPath, string outputPath, string lang);
}