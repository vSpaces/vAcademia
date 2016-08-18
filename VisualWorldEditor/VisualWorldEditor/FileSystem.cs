using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace VisualWorldEditor
{
    class FileSystem
    {
        public FileSystem()
        {
            
        }

        public bool CopyFile(String srcFilePath, String destFilePath) // копирование файла
        {
            try
            {
                destFilePath = destFilePath.Replace("\\\\", "\\").Replace("\\", "/");
                String destDirPath = destFilePath.Substring(0, destFilePath.LastIndexOf("/"));
                if(!Directory.Exists(destDirPath))
                {
                    Directory.CreateDirectory(destDirPath);
                }

                File.Copy(srcFilePath, destFilePath, true);

                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine("FileSystem exception: " + e.Message);
            }

            return false;
        }

        public bool CopyDir(String srcPath, String destPath) // копирование каталога
        {
            try
            {
                String[] files = Directory.GetFiles(srcPath, "*", SearchOption.AllDirectories); // список файлов в каталоге

                foreach(String file in files)
                {
                    String destFilePath = destPath + file.Substring(srcPath.Length);

                    CopyFile(file, destFilePath);
                }

                return true;

            }
            catch (Exception e)
            {
                Console.WriteLine("FileSystem exception: " + e.Message);
            }
            
            return false;
        }

        public bool ReplaceDir(String srcPath, String destPath) // полна€ замена каталога
        {
            try
            {
                if (Directory.Exists(destPath))
                {
                    if(!RemoveDir(destPath))
                    {
                        return false;
                    }
                }

                return CopyDir(srcPath, destPath);
            }
            catch (Exception e)
            {
                Console.WriteLine("FileSystem exception: " + e.Message);
                return false;
            }
        }

        public bool RemoveDir(String targetPath)
        {
            try
            {
                Directory.Delete(targetPath, true);
            }
            catch(Exception e)
            {
                Console.WriteLine("FileSystem exception: " + e.Message);
                return false;
            }

            return true;
        }

    }
}
