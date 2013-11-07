using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public class Function
    {
        List<Variable> args = new List<Variable>();
        List<Variable> locals = new List<Variable>();
        List<Instruction> insts = new List<Instruction>();
        public int Address { get; set; }
        public string Name { get; set; }
        public int TotalLocalCount { get; set; }
        public VarTypes ReturnType { get; set; }
        public int U1 { get; set; }
        public int StructID { get; set; }
        public int ArgCount { get; set; }
        public int U4 { get; set; }
        public List<Variable> Arguments { get { return args; } }
        public List<Variable> LocalVariables { get { return locals; } }
        public Assembly Assembly { get; set; }
        public List<Instruction> Instructions { get { return insts; } }
        public string FileName { get; set; }
        public string SourceCode { get; set; }
        static Dictionary<Syscalls, Function> syscalls;

        public static Dictionary<Syscalls, Function> Syscalls
        {
            get
            {
                if (syscalls == null)
                {
                    syscalls = new Dictionary<Syscalls, Function>();
                    Function func = new Function();
                    func.Name = "Exit";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.Int);
                    func.ReturnType = VarTypes.Void;
                    syscalls.Add(Decompiler.Assembly.Syscalls.Exit, func);
                    func = new Function();
                    func.Name = "GlobalSave";
                    func.ArgCount = 2;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GlobalSave, func);
                    func = new Function();
                    func.Name = "GlobalLoad";
                    func.ArgCount = 2;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GlobalLoad, func);
                    func = new Function();
                    func.Name = "ResumeLoad";
                    func.ArgCount = 2;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Void;
                    syscalls.Add(Decompiler.Assembly.Syscalls.ResumeLoad, func);
                    func = new Function();
                    func.Name = "Error";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.String;
                    syscalls.Add(Decompiler.Assembly.Syscalls.Error, func);
                    func = new Function();
                    func.Name = "Output";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.String;
                    syscalls.Add(Decompiler.Assembly.Syscalls.Output, func);
                    func = new Function();
                    func.Name = "Sleep";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.Int);
                    func.ReturnType = VarTypes.Void;
                    syscalls.Add(Decompiler.Assembly.Syscalls.Sleep, func);
                    func = new Function();
                    func.Name = "Peek";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.Void;
                    syscalls.Add(Decompiler.Assembly.Syscalls.Peek, func);
                    func = new Function();
                    func.Name = "LockPeek";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.LockPeek, func);
                    func = new Function();
                    func.Name = "UnlockPeek";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.UnlockPeek, func);
                    func = new Function();
                    func.Name = "Reset";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.Void;
                    syscalls.Add(Decompiler.Assembly.Syscalls.Reset, func);
                    func = new Function();
                    func.Name = "GetFuncStackName";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.Int);
                    func.ReturnType = VarTypes.String;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GetFuncStackName, func);
                    func = new Function();
                    func.Name = "GetTime";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GetTime, func);
                    func = new Function();
                    func.Name = "CopySaveFile";
                    func.ArgCount = 2;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.CopySaveFile, func);
                    func = new Function();
                    func.Name = "GroupSave";
                    func.ArgCount = 5;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GroupSave, func);
                    func = new Function();
                    func.Name = "GetSaveFolderName";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.String;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GetSaveFolderName, func);
                    func = new Function();
                    func.Name = "ExistSaveFile";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.ExistSaveFile, func);
                    func = new Function();
                    func.Name = "ResumeSave";
                    func.ArgCount = 3;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.IntRef);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.ResumeSave, func);
                    func = new Function();
                    func.Name = "MsgBox";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.String;
                    syscalls.Add(Decompiler.Assembly.Syscalls.MsgBox, func);
                    func = new Function();
                    func.Name = "MsgBoxOkCancel";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.MsgBoxOkCancel, func);
                    func = new Function();
                    func.Name = "OpenWeb";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Void;
                    syscalls.Add(Decompiler.Assembly.Syscalls.OpenWeb, func);
                    func.Name = "GroupLoad";
                    func.ArgCount = 5;
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.GroupLoad, func);
                    func.Name = "DeleteSaveFile";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.DeleteSaveFile, func);
                    func = new Function();
                    func.Name = "IsDebugMode";
                    func.ArgCount = 0;
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.IsDebugMode, func);
                    func = new Function();
                    func.Name = "ExistFunc";
                    func.ArgCount = 1;
                    func.AddArg(VarTypes.String);
                    func.ReturnType = VarTypes.Int;
                    syscalls.Add(Decompiler.Assembly.Syscalls.ExistFunc, func);
                }
                return syscalls;
            }
        }

        public override string ToString()
        {
            string arg = "";
            string fname = Util.GetTypeName(ReturnType, Assembly, StructID, 1);
            string n = Name;
            if (n.Contains("@"))
                n = n.Substring(n.IndexOf("@") + 1);
            foreach (Variable i in args)
            {
                string name = Util.GetTypeName(i.VarType, Assembly, i.StructID, i.Dimension);
                
                arg += string.Format("{0} {1},", name, i.Name);
            }
            if (arg.Length == 0)
                arg += ",";
            return string.Format("{0} {1} ({2})", fname, n, arg.Substring(0, arg.Length - 1));
        }
    }
}
