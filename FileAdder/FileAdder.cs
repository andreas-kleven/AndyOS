using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileAdder
{
    class FileAdder
    {
        static void Main(string[] args)
        {
            if (args.Length < 4)
                return;

            string mapPath = args[0];
            string kernelPath = args[1];
            string filename = args[2];
            string varname = args[3];

            CheckFile(mapPath);
            CheckFile(kernelPath);
            CheckFile(filename);

            string[] lines = File.ReadAllLines(mapPath);

            foreach(string line in lines)
            {
                if (line.Trim().StartsWith("0003:"))
                {
                    if (line.Contains($"?{varname}@@"))
                    {
                        var split = line.Split(new[] { '"' }).SelectMany((s, i) =>
                        {
                            if (i % 2 == 1) return new[] { s.Replace(" ", "") };
                            return s.Split(new[] { ' ', ';' }, StringSplitOptions.RemoveEmptyEntries);
                        }).ToList();

                        UInt32 addr = UInt32.Parse(split[2], System.Globalization.NumberStyles.HexNumber);
                        addr -= 0x100000;
                        Console.WriteLine($"Writing file to '{varname}' at address 0x{addr}...");

                        if(WriteToFile(filename, kernelPath, addr))
                        {
                            Console.WriteLine("Success!");
                        }
                        else
                        {
                            Console.WriteLine("Could not write to file");
                        }
                    }
                }
            }
        }

        static bool WriteToFile(string readPath, string writePath, UInt32 position)
        {
            try
            {
                byte[] data = File.ReadAllBytes(readPath);

                using (Stream stream = new FileStream(writePath, FileMode.Open))
                {
                    stream.Seek(position, SeekOrigin.Begin);
                    stream.Write(BitConverter.GetBytes(data.Length), 0, 4);
                    stream.Write(data, 0, data.Length);
                }

                return true;
            }
            catch
            {
                return false;
            }
        }

        static void CheckFile(string path)
        {
            if (!File.Exists(path))
            {
                Console.WriteLine($"File '{path}' does not exist!");
                Environment.Exit(0);
            }
        }
    }
}
