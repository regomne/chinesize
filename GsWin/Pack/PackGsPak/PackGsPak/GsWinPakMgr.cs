using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;


namespace PackGsPak
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct ScwHeader
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x10)]
        public string Magic;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x20)]
        public string Description;
        public ushort MinorVer;
        public ushort MajorVer;
        public uint IndexLen;
        public uint Flags;
        public uint IndexEntries;
        public uint DataOffset;
        public uint IndexOffset;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct ScwIndexEntry
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Name;
        public uint Offset;
        public uint Length;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        public byte[] Pad;
    }

    class GsWinPakMgr
    {
        public bool LzssUncompress(byte[] dest, int destlen, byte[] src, int srclen)
        {
            byte[] win = new byte[0x1000];
            int iwin = 0xfee;
            int isrc = 0;
            int idest = 0;
            byte data;
            while (isrc < srclen)
            {
                ushort code = (ushort)(src[isrc++] | 0xff00);
                for (; (code & 0x100) != 0; code >>= 1)
                {
                    if ((code & 1) == 1)
                    {
                        if (idest >= destlen)
                            break;
                        dest[idest++] = win[iwin++] = src[isrc++];
                        iwin &= 0xfff;
                    }
                    else
                    {
                        if (isrc + 1 >= srclen)
                            break;
                        uint pos = src[isrc++];
                        uint count = src[isrc++];
                        pos |= (count & 0xf0) << 4;
                        count = (count & 0xf) + 3;
                        for (int i = 0; i < count; i++)
                        {
                            data = win[(pos + i) & 0xfff];
                            if (idest >= destlen)
                                break;
                            dest[idest++] = win[iwin++] = data;
                            iwin &= 0xfff;
                        }
                    }
                }
            }
            return true;
        }

        public byte[] LzssPack(byte[] src, uint srclen, ref uint newlen)
        {
            uint isrc = 0;
            uint idest = 0;
            byte[] dest = new byte[srclen * 10 / 8];
            while (isrc < srclen - 8)
            {
                dest[idest++] = 0xff;
                for (int i = 0; i < 8; i++)
                {
                    dest[idest++] = src[isrc++];
                }
            }
            if (isrc != srclen)
            {
                byte len = (byte)(srclen - isrc);
                //dest[idest++] = (byte)((1 << (byte)(len + 1)) - 1);
                dest[idest++] = 0xff;
                for (int i = 0; i < len; i++)
                {
                    dest[idest++] = src[isrc++];
                }
            }
            newlen = idest;
            return dest;
        }

        public void DecryptBlock(byte[] stm, int length=-1)
        {
            if (length == -1)
                length = stm.Length;
            for (int i = 0; i < length; i++)
            {
                stm[i] ^= (byte)(i & 0xff);
            }
        }

        public int CalcKey(string name)
        {
            int key=0;
            foreach (sbyte b in name)
            {
                key = key * 0x25 + ((int)b | 0x20);
            }
            return key;
        }

        public void DecryptBlock2(byte[] stm, int key, int length=-1)
        {
            if (length == -1)
                length = stm.Length;
            MemoryStream s = new MemoryStream(stm);
            MemoryStream news = new MemoryStream(length);
            BinaryReader br = new BinaryReader(s);
            BinaryWriter bw = new BinaryWriter(news);

            for (int i = 0; i < length / 4; i++)
            {
                uint a = br.ReadUInt32();
                bw.Write((uint)(a ^ key));
            }
            for (int i = 0; i < (length & 0x3); i++)
            {
                bw.Write(br.ReadByte());
            }
            s.Seek(0, SeekOrigin.Begin);
            news.Seek(0, SeekOrigin.Begin);
            news.WriteTo(s);
        }

        public bool Pack(string DirName, string PackageName, string NewPakName = null)
        {
            //if (NewPakName == null)
            //    NewPakName = PackageName.Replace(".pak", "2.pak");

            FileInfo pakFile = new FileInfo(PackageName);
            FileStream stmori = pakFile.Open(FileMode.Open, FileAccess.ReadWrite);
            BinaryReader brori = new BinaryReader(stmori);
            BinaryWriter bwori = new BinaryWriter(stmori);

            ScwHeader scwHdr = (ScwHeader)StructConverter.BytesToStruct(brori.ReadBytes(
                Marshal.SizeOf(typeof(ScwHeader))), typeof(ScwHeader));

            if (scwHdr.Magic != "DataPack5" || scwHdr.MajorVer != 5)
                throw new FormatException("File");

            stmori.Seek(scwHdr.IndexOffset, SeekOrigin.Begin);
            byte[] comprIdx = brori.ReadBytes((int)scwHdr.IndexLen);
            byte[] uncomprIdx = new byte[scwHdr.IndexEntries * Marshal.SizeOf(typeof(ScwIndexEntry))];
            DecryptBlock(comprIdx);
            LzssUncompress(uncomprIdx, (int)scwHdr.IndexEntries * Marshal.SizeOf(typeof(ScwIndexEntry)),
                comprIdx, (int)scwHdr.IndexLen);
            MemoryStream stmidx = new MemoryStream(uncomprIdx);
            BinaryReader bridx = new BinaryReader(stmidx);
            List<ScwIndexEntry> entries = new List<ScwIndexEntry>((int)scwHdr.IndexEntries);
            //ArrayList entries2 = new ArrayList((int)scwHdr.IndexEntries);
            for (int i = 0; i < scwHdr.IndexEntries; i++)
            {
                entries.Add((ScwIndexEntry)StructConverter.BytesToStruct(
                    bridx.ReadBytes(Marshal.SizeOf(typeof(ScwIndexEntry))), typeof(ScwIndexEntry)));
            }

            Directory.SetCurrentDirectory(DirName);
            long dataend = scwHdr.IndexOffset - scwHdr.DataOffset;
            for (int i = 0; i < scwHdr.IndexEntries; i++)
            {
                ScwIndexEntry entry = entries[i];
                if (Path.GetFileName(PackageName).StartsWith("Graphic", StringComparison.CurrentCultureIgnoreCase))
                {
                    if (File.Exists(entry.Name+".png"))
                    {
                        FileInfo newf = new FileInfo(entry.Name+".png");
                        byte[] stmnew = File.ReadAllBytes(entry.Name + ".png");
                        DecryptBlock2(stmnew, CalcKey(entry.Name));
                        if (newf.Length <= entry.Length)
                        {
                            stmori.Seek(entry.Offset + scwHdr.DataOffset, SeekOrigin.Begin);
                            bwori.Write(stmnew);
                            entry.Length = (uint)newf.Length;
                        }
                        else
                        {
                            stmori.Seek(dataend + scwHdr.DataOffset, SeekOrigin.Begin);
                            bwori.Write(stmnew);
                            entry.Offset = (uint)dataend;
                            dataend += newf.Length;
                            dataend = dataend % 4 == 0 ? dataend : dataend + (4 - dataend % 4);
                            entry.Length = (uint)newf.Length;
                        }
                        entries[i] = entry;
                    }
                }
                else if (Path.GetFileName(PackageName).StartsWith("Scr", StringComparison.CurrentCultureIgnoreCase))
                {
                    if (File.Exists(entry.Name))
                    {
                        FileInfo newf = new FileInfo(entry.Name);
                        if (newf.Length <= entry.Length)
                        {
                            stmori.Seek(entry.Offset + scwHdr.DataOffset, SeekOrigin.Begin);
                            bwori.Write(File.ReadAllBytes(entry.Name));
                            entry.Length = (uint)newf.Length;
                        }
                        else
                        {
                            stmori.Seek(dataend + scwHdr.DataOffset, SeekOrigin.Begin);
                            bwori.Write(File.ReadAllBytes(entry.Name));
                            entry.Offset = (uint)dataend;
                            dataend += newf.Length;
                            dataend = dataend % 4 == 0 ? dataend : dataend + (4 - dataend % 4);
                            entry.Length = (uint)newf.Length;
                        }
                        entries[i] = entry;
                    }
                }
            }

            stmidx.Seek(0, SeekOrigin.Begin);
            for (int i = 0; i < scwHdr.IndexEntries; i++)
            {
                byte[] bentry = StructConverter.StructToBytes(entries[i]);
                stmidx.Write(bentry, 0, Marshal.SizeOf(typeof(ScwIndexEntry)));
            }
            uint newlen = 0;
            comprIdx = LzssPack(uncomprIdx, (uint)uncomprIdx.Length, ref newlen);
            DecryptBlock(comprIdx,(int)newlen);
            stmori.Seek(dataend + scwHdr.DataOffset, SeekOrigin.Begin);
            bwori.Write(comprIdx, 0, (int)newlen);
            long endpos = stmori.Position;

            scwHdr.IndexOffset = (uint)dataend+scwHdr.DataOffset;
            scwHdr.IndexLen = (uint)newlen;
            stmori.Seek(0, SeekOrigin.Begin);
            bwori.Write(StructConverter.StructToBytes(scwHdr));

            stmori.SetLength(endpos);
            stmori.Close();

            return true;
        }

        public bool Unpack(string DirName, string PackageName)
        {
            FileInfo pakFile = new FileInfo(PackageName);
            FileStream stmori = pakFile.Open(FileMode.Open, FileAccess.Read);
            BinaryReader brori = new BinaryReader(stmori);

            ScwHeader scwHdr = (ScwHeader)StructConverter.BytesToStruct(brori.ReadBytes(
                Marshal.SizeOf(typeof(ScwHeader))), typeof(ScwHeader));

            if (scwHdr.Magic != "DataPack5" || scwHdr.MajorVer != 5)
                throw new FormatException("File");

            stmori.Seek(scwHdr.IndexOffset, SeekOrigin.Begin);
            byte[] comprIdx = brori.ReadBytes((int)scwHdr.IndexLen);
            byte[] uncomprIdx = new byte[scwHdr.IndexEntries * Marshal.SizeOf(typeof(ScwIndexEntry))];
            DecryptBlock(comprIdx);
            LzssUncompress(uncomprIdx, (int)scwHdr.IndexEntries * Marshal.SizeOf(typeof(ScwIndexEntry)),
                comprIdx, (int)scwHdr.IndexLen);
            MemoryStream stmidx = new MemoryStream(uncomprIdx);
            BinaryReader bridx = new BinaryReader(stmidx);
            List<ScwIndexEntry> entries = new List<ScwIndexEntry>((int)scwHdr.IndexEntries);
            //ArrayList entries2 = new ArrayList((int)scwHdr.IndexEntries);
            for (int i = 0; i < scwHdr.IndexEntries; i++)
            {
                entries.Add((ScwIndexEntry)StructConverter.BytesToStruct(
                    bridx.ReadBytes(Marshal.SizeOf(typeof(ScwIndexEntry))), typeof(ScwIndexEntry)));
            }

            Directory.SetCurrentDirectory(DirName);
            if (Path.GetFileName(PackageName).StartsWith("Graphic", StringComparison.CurrentCultureIgnoreCase))
            {
                for (int i = 0; i < scwHdr.IndexEntries; i++)
                {
                    ScwIndexEntry entry = entries[i];
                    FileInfo newf = new FileInfo(entry.Name + ".png");
                    Stream stmnew = newf.Open(FileMode.Create, FileAccess.Write);

                    stmori.Seek(entry.Offset + scwHdr.DataOffset, SeekOrigin.Begin);
                    byte[] newbuff = brori.ReadBytes((int)entry.Length);
                    DecryptBlock2(newbuff, CalcKey(entry.Name));
                    stmnew.Write(newbuff, 0, (int)entry.Length);
                    stmnew.Close();
                }
            }
            else if (Path.GetFileName(PackageName).StartsWith("Scr", StringComparison.CurrentCultureIgnoreCase))
            {
                for (int i = 0; i < scwHdr.IndexEntries; i++)
                {
                    ScwIndexEntry entry = entries[i];
                    FileInfo newf = new FileInfo(entry.Name);
                    Stream stmnew = newf.Open(FileMode.Create, FileAccess.Write);
                    stmori.Seek(entry.Offset + scwHdr.DataOffset, SeekOrigin.Begin);
                    stmnew.Write(brori.ReadBytes((int)entry.Length), 0, (int)entry.Length);
                    stmnew.Close();
                }
            }
            else
            {
                throw new FormatException("Not supported prefix!");
            }

            stmori.Close();
            return true;
        }
    }
}
