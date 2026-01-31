using System;
using System.IO;
using System.Runtime.InteropServices;

class Program
{
    static void Main(string[] args)
    {
        if (args.Length < 2)
        {
            Console.WriteLine("Usage: mlt <file.mlt> <lang>");
            return;
        }

        string inputFile = args[0];
        string targetLang = args[1].ToLower(); // 'csharp', 'cpp', etc.

        string ext = targetLang switch
        {
            "csharp" => ".cs",
            "cs" => ".cs",
            "cpp" => ".cpp",
            "c" => ".c",
            "go" => ".go",
            "golang" => ".go",
            "rust" => ".rs",
            "rs" => ".rs",
            _ => ".txt"
        };

        string outputFile = Path.ChangeExtension(inputFile, ext);

        Console.WriteLine($"--- MLT Transpiler ---");
        Console.WriteLine($"Target: {targetLang}");

        try
        {
            IntPtr resultPtr = MLTBridge.TranspileFile(inputFile, outputFile, targetLang);

            string result = Marshal.PtrToStringAnsi(resultPtr);
            if (!string.IsNullOrEmpty(result))
            {
                Console.WriteLine($"Status: {result}");
            }

            Console.WriteLine($"Done! Saved to {outputFile}");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
        }
    }
}