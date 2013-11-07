using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public unsafe class Assembly
    {
        [DllImport("zlib.dll")]
        static extern void compress(byte* dest, uint* destLen, byte* src, uint srcLen);
        [DllImport("zlib.dll")]
        static extern void uncompress(byte* dest, uint* destLen, byte* src, uint srcLen);

        List<Function> func = new List<Function>();
        List<Function> fnct = new List<Function>();
        List<Switch> switches = new List<Switch>();
        List<Hll> hlls = new List<Hll>();
        List<Struct> structs = new List<Struct>();
        List<Variable> global = new List<Variable>();
        List<string> msgs = new List<string>();
        List<string> strings = new List<string>();
        List<string> fname = new List<string>();
        List<string> objg = new List<string>();
        public int Version { get; set; }
        public int GVersion { get; set; }
        public int Keyc { get; set; }
        public List<Function> Functions { get { return func; } }
        //public List<Global> Globals { get { return global; } }
        public List<string> MsgList { get { return msgs; } }
        public List<string> StringList { get { return strings; } }
        public List<string> FileNames { get { return fname; } }
        public List<Variable> GlobalVariables { get { return global; } }
        public List<Struct> Structures { get { return structs; } }
        public List<Hll> HLLs { get { return hlls; } }
        public List<Switch> SwitchTables { get { return switches; } }
        public Function EntryPoint { get; set; }
        public Function MessageFunction { get; set; }
        public Function JumpFunction { get; set; }
        public byte[] CodeBuffer { get; set; }

        public static Assembly FromAIN(string path)
        {
            Assembly asm = new Assembly();
            System.IO.FileStream fs = new System.IO.FileStream(path, System.IO.FileMode.Open);
            System.IO.BinaryReader br = new System.IO.BinaryReader(fs);
            fs.Position = 8;
            uint size = br.ReadUInt32();
            uint compressedSize = br.ReadUInt32();
            byte[] buf;
            byte[] dst = new byte[size];
            buf = br.ReadBytes((int)compressedSize);
            fixed (byte* ptr = buf)
            {
                fixed (byte* ptr2 = dst)
                {
                    uncompress(ptr2, &size, ptr, compressedSize);
                }
            }
            fs.Close();

            System.IO.MemoryStream ms = new System.IO.MemoryStream(dst);
            br = new System.IO.BinaryReader(ms);
            while (ms.Position < ms.Length)
            {
                string trunk = Encoding.ASCII.GetString(br.ReadBytes(4));
                switch (trunk)
                {
                    case "VERS":
                        asm.Version = br.ReadInt32();
                        if (asm.Version != 6)
                        {
                            throw new NotSupportedException("本SDK仅支持版本为：6的反编译");
                        }
                        break;
                    case "KEYC":
                        asm.Keyc = br.ReadInt32();
                        break;
                    case "CODE":
                        asm.CodeBuffer = br.ReadBytes(br.ReadInt32());
                        break;
                    case "FUNC":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                Function func = new Function();
                                func.Address = br.ReadInt32();
                                func.Assembly = asm;
                                func.Name = ReadString(br);
                                func.U1 = br.ReadInt32();
                                func.ReturnType = (VarTypes)br.ReadInt32();
                                func.StructID = br.ReadInt32();
                                func.ArgCount = br.ReadInt32();
                                func.TotalLocalCount = br.ReadInt32();
                                func.U4 = br.ReadInt32();
                                for (int j = 0; j < func.TotalLocalCount; j++)
                                {
                                    Variable arg = new Variable();
                                    arg.Name = ReadString(br);
                                    arg.Assembly = asm;
                                    arg.VarType = (VarTypes)br.ReadInt32();
                                    arg.StructID = br.ReadInt32();
                                    arg.Dimension = br.ReadInt32();
                                    if (j < func.ArgCount)
                                        func.Arguments.Add(arg);
                                    func.LocalVariables.Add(arg);
                                }
                                asm.Functions.Add(func);
                            }
                        }
                        break;
                    case "GLOB":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                Variable var = new Variable();
                                var.Name = ReadString(br);
                                var.Assembly = asm;
                                var.VarType = (VarTypes)br.ReadInt32();
                                var.StructID = br.ReadInt32();
                                var.Dimension = br.ReadInt32();
                                var.U3 = br.ReadInt32();
                                asm.global.Add(var);
                            }
                        }
                        break;
                    case "GSET":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                Variable var = asm.global[br.ReadInt32()];
                                VarTypes type = (VarTypes)br.ReadInt32();
                                switch (type)
                                {
                                    case VarTypes.Float:
                                        var.FloatVal = br.ReadSingle();
                                        break;
                                    case VarTypes.String:
                                        var.StringVal = ReadString(br);
                                        break;
                                    case VarTypes.Bool:
                                        var.BoolVal = br.ReadInt32() == 1;
                                        break;
                                    default:
                                        var.IntVal = br.ReadInt32();
                                        break;
                                }
                            }
                        }
                        break;
                    case "STRT":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                Struct str = new Struct();
                                str.Name = ReadString(br);
                                int func = br.ReadInt32();
                                if (func != -1)
                                    str.Constructor = asm.func[func];
                                func = br.ReadInt32();
                                if (func != -1)
                                    str.Destructor = asm.func[func];
                                str.VarCount = br.ReadInt32();
                                for (int j = 0; j < str.VarCount; j++)
                                {
                                    Variable arg = new Variable();
                                    arg.Name = ReadString(br);
                                    arg.Assembly = asm;
                                    arg.VarType = (VarTypes)br.ReadInt32();
                                    arg.StructID = br.ReadInt32();
                                    arg.Dimension = br.ReadInt32();
                                    str.Variables.Add(arg);
                                }
                                asm.structs.Add(str);
                            }
                        }
                        break;
                    case "MSG0":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                asm.msgs.Add(ReadString(br));
                            }
                        }
                        break;
                    case "MAIN":
                        asm.EntryPoint = asm.func[br.ReadInt32()];
                        break;
                    case "MSGF":
                        asm.MessageFunction = asm.func[br.ReadInt32()];
                        break;
                    case "HLL0":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                string name = ReadString(br);
                                Hll hll = new Hll();
                                hll.Name = name;
                                int count2 = br.ReadInt32();
                                for (int j = 0; j < count2; j++)
                                {
                                    Function func = new Function();
                                    func.Name = ReadString(br);
                                    func.ReturnType = (VarTypes)br.ReadInt32();
                                    func.TotalLocalCount = br.ReadInt32();
                                    func.ArgCount = func.TotalLocalCount;
                                    for (int k = 0; k < func.TotalLocalCount; k++)
                                    {
                                        Variable arg = new Variable();
                                        arg.Assembly = asm;
                                        arg.Name = ReadString(br);
                                        arg.VarType = (VarTypes)br.ReadInt32();
                                        func.Arguments.Add(arg);
                                    }
                                    hll.Functions.Add(func);
                                }
                                asm.hlls.Add(hll);
                            }
                        }
                        break;
                    case "SWI0":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                Switch swi = new Switch();
                                swi.SwitchType = (SwitchTypes)br.ReadInt32();
                                swi.DefaultCase = br.ReadInt32();
                                int count2 = br.ReadInt32();
                                for (int j = 0; j < count2; j++)
                                {
                                    swi.SwitchTable.Add(br.ReadInt32(), br.ReadInt32());
                                }
                                asm.switches.Add(swi);
                            }
                        }
                        break;
                    case "GVER":
                        asm.GVersion = br.ReadInt32();
                        break;
                    case "STR0":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                string str = ReadString(br);
                                asm.strings.Add(str);
                            }
                        }
                        break;
                    case "FNAM":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                string str = ReadString(br);
                                asm.fname.Add(str);
                            }
                        }
                        break;
                    case "OJMP":
                        {
                            int funcID = br.ReadInt32();
                            if (funcID >= 0)
                                asm.JumpFunction = asm.func[funcID];
                        }
                        break;
                    case "FNCT":
                        {
                            int len = br.ReadInt32();
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                Function func = new Function();
                                func.Name = ReadString(br);
                                func.ReturnType = (VarTypes)br.ReadInt32();
                                func.StructID = br.ReadInt32();
                                func.ArgCount = br.ReadInt32();
                                func.TotalLocalCount = br.ReadInt32();
                                for (int j = 0; j < func.TotalLocalCount; j++)
                                {
                                    Variable arg = new Variable();
                                    arg.Name = ReadString(br);
                                    arg.VarType = (VarTypes)br.ReadInt32();
                                    arg.StructID = br.ReadInt32();
                                    arg.Dimension = br.ReadInt32();
                                    func.Arguments.Add(arg);
                                }
                                asm.fnct.Add(func);
                            }
                        }
                        break;
                    case "OBJG":
                        {
                            int count = br.ReadInt32();
                            for (int i = 0; i < count; i++)
                            {
                                string str = ReadString(br);
                                asm.objg.Add(str);
                            }
                        }
                        break;
                    default:
                        throw new ArgumentException(string.Format("{0}文件中包含未知的Trunk：{1}", path, trunk));
                }
            }
            return asm;
        }

        public void Decompile()
        {
            System.IO.MemoryStream ms = new System.IO.MemoryStream(CodeBuffer);
            System.IO.BinaryReader br = new System.IO.BinaryReader(ms);
            int i = 0;
            List<Function> staticFuncs = new List<Function>();
            List<Function> lastFile = new List<Function>();
            var ordered = from func in this.func
                          orderby func.Address
                          select func;

            foreach (Function func in ordered.ToList())
            {
                if (func.Name == "NULL")
                    continue;
                ms.Position = func.Address;
                if (ms.Position == ms.Length)
                    continue;
                string filename;
                Deassemble(br, func, out filename);
                lastFile.Add(func);
                if (!string.IsNullOrEmpty(filename))
                {
                    foreach (Function j in lastFile)
                    {
                        j.FileName = filename;
                    }
                    lastFile.Clear();
                }
                DecompileFunc(func);
                if (func.Name.Contains("@"))
                {
                    Struct clsObj = GetFuncClass(func);
                    if (func != clsObj.Constructor && func != clsObj.Destructor)
                        clsObj.Functions.Add(func);
                    for (int j = 0; j < func.ArgCount; j++)
                    {
                        switch (func.LocalVariables[j].VarType)
                        {
                            case VarTypes.StrucRef:
                                this.structs[func.LocalVariables[j].StructID].Type = StructTypes.Class;
                                break;
                            case VarTypes.Struct:
                                this.structs[func.LocalVariables[j].StructID].Type = StructTypes.Struct;
                                break;
                        }
                    }
                }
                else
                    staticFuncs.Add(func);
                i++;
            }
            if (!System.IO.Directory.Exists("Output"))
                System.IO.Directory.CreateDirectory("Output");
            else
            {
                try
                {
                    System.IO.Directory.Delete("Output", true);
                    System.IO.Directory.CreateDirectory("Output");
                }
                catch { }
            }
            foreach (Struct j in this.structs)
            {
                string name = "";
                if (j.Functions.Count > 0 && j.Functions[0].FileName != null)
                    name = j.Functions[0].FileName.Substring(0, j.Functions[0].FileName.Length - 4);
                else
                    name = j.Name;
                string folder = "Output\\" + System.IO.Path.GetDirectoryName(name);
                if (!System.IO.Directory.Exists(folder))
                    System.IO.Directory.CreateDirectory(folder);
                System.IO.StreamWriter sw = new System.IO.StreamWriter("Output\\" + name + ".cs", true, Encoding.UTF8);
                sw.WriteLine(j);
                sw.Flush();
                sw.Close();
            }
            {
                foreach (Function j in staticFuncs)
                {
                    string name = "";
                    name = j.FileName != null ? j.FileName.Substring(0, j.FileName.Length - 4) : j.Name;
                    string folder = "Output\\" + System.IO.Path.GetDirectoryName(name);
                    if (!System.IO.Directory.Exists(folder))
                        System.IO.Directory.CreateDirectory(folder);
                    System.IO.StreamWriter sw = new System.IO.StreamWriter("Output\\" + name + ".cs", true, Encoding.UTF8);
                    sw.WriteLine(j.SourceCode);

                    sw.Flush();
                    sw.Close();
                }
            }
        }

        void Deassemble(System.IO.BinaryReader br, Function func,out string filename)
        {
            Dictionary<int, Instruction> addressMapping = new Dictionary<int, Instruction>();
            Dictionary<int, List<Instruction>> unresolvedAddr = new Dictionary<int, List<Instruction>>();
            int switchExit = int.MaxValue;
            filename = "";
            do
            {
                int addr = (int)br.BaseStream.Position;
                Instruction insn = Instruction.ParseInstruction(br, this, func);
                Instructions op = insn.Inst;
                addressMapping.Add(addr, insn);
                if (unresolvedAddr.ContainsKey(addr))
                {
                    int index = func.Instructions.Count;
                    foreach (Instruction i in unresolvedAddr[addr])
                    {
                        while (i.BranchTarget.Contains(addr))
                        {
                            int index2 = i.BranchTarget.IndexOf(addr);
                            i.BranchTarget[index2] = index;
                        }
                    }
                    unresolvedAddr.Remove(addr);
                    insn.IsBranchTarget = true;
                }
                for (int i = 0; i < insn.BranchTarget.Count; i++)
                {
                    if (addressMapping.ContainsKey(insn.BranchTarget[i]))
                    {
                        Instruction target = addressMapping[insn.BranchTarget[i]];
                        int index = func.Instructions.IndexOf(target);
                        target.IsBranchTarget = true;
                        target.BranchFrom = func.Instructions.Count - 1;
                        insn.BranchTarget[i] = index;
                    }
                    else
                    {
                        if (!unresolvedAddr.ContainsKey(insn.BranchTarget[i]))
                            unresolvedAddr.Add(insn.BranchTarget[i], new List<Instruction>());
                        unresolvedAddr[insn.BranchTarget[i]].Add(insn);
                    }
                }
                if (op == Instructions.Call || op == Instructions.CallMethod)
                {
                    //if (!funcs.Contains(this.func[insn.Operand1]))
                    //    funcs.Push(this.func[insn.Operand1]);
                }
                if (op == Instructions.AssertFunc)
                {
                    break;
                }
                func.Instructions.Add(insn);
                if (op == Instructions.EndOfFunc)
                {
                    insn = Instruction.ParseInstruction(br, this, func);
                    if (insn.Inst == Instructions.AssertFile)
                        filename = this.FileNames[insn.Operand1];
                    break;
                }
                if (op == Instructions.AssertFile)
                {
                    if (insn.Inst == Instructions.AssertFile)
                        filename = this.FileNames[insn.Operand1];
                    break;
                }
            } while (true);
        }

        void DecompileFunc(Function func)
        {
            CodeBlock block = new CodeBlock();
            int index = 0;
            Dictionary<Instruction, CodeBlock> blockMapping = new Dictionary<Instruction, CodeBlock>();
            string source = "";
            Stack<string> stack = new Stack<string>();
            do
            {
                CodeBlock sub = new CodeBlock();
                block.CodeBlocks.Add(sub);
                index = AnalyzeCodeBlock(sub, func.Instructions, blockMapping, index, -1);
            } while (index < func.Instructions.Count);
            source = func.ToString() + "\r\n";
            source += "{\r\n";
            string pre = "    ";
            for (int i = func.ArgCount; i < func.LocalVariables.Count; i++)
            {
                if (func.LocalVariables[i].Name.StartsWith("<dummy"))
                {
                    string name = Util.GetTypeName(func.LocalVariables[i].VarType, this, func.LocalVariables[i].StructID, func.LocalVariables[i].Dimension);
                    func.LocalVariables[i].Name = "v" + (name.Contains("[") ? name.Substring(0, name.IndexOf("[")) : name).Replace("ref ", "") + i.ToString();
                }
                source += pre + func.LocalVariables[i].ToString().Replace("ref ", "") + ";\r\n";
            }

            foreach (CodeBlock i in block.CodeBlocks)
            {
                source += DumpCodeBlock(func, stack, i, ref pre);
            }
            source += "}\r\n";
            func.SourceCode = source;
        }

        string DumpCodeBlock(Function func, Stack<string> stack, CodeBlock block, ref string pre)
        {
            string res = pre + "{\r\n";
            pre += "    ";
            Dictionary<int, List<string>> switchTargets = null;
            int startIndex = func.Instructions.IndexOf(block.Instructions[0]);
            int endIndex = block.EndIndex(func);
            int branch = 0;
            bool switchStarted = false;
            bool elseStarted = false;
            foreach (Instruction i in block.Instructions)
            {
                if (i.IsBranchTarget && (i.BranchFrom > func.Instructions.IndexOf(i)) && !block.Instructions.Contains(func.Instructions[i.BranchFrom]))
                {
                    int index = func.Instructions.IndexOf(i);
                    res += pre + "label" + index + ":\r\n";
                }
                if (i.Inst == Instructions.DummyCodeBlock)
                {
                    if (switchTargets != null)
                    {
                        CodeBlock tar = block.CodeBlocks[i.Operand1];
                        int index = func.Instructions.IndexOf(tar.Instructions[0]);
                        if (switchTargets.ContainsKey(index))
                        {
                            if (switchStarted)
                            {
                                res += pre + "break;\r\n";
                                pre = pre.Substring(4);
                            }
                            foreach (string j in switchTargets[index])
                            {
                                if (j != "default")
                                    res += string.Format("{0}case {1}:\r\n", pre, j);
                                else
                                    res += string.Format("{0}{1}:\r\n", pre, j);
                            }
                            switchStarted = true;
                        }
                        else
                            pre = pre.Substring(4);
                    }
                    if (switchTargets != null)
                        pre += "    ";
                    if (elseStarted && branch > 0)
                    {
                        res += pre + "else\r\n" + pre + "{\r\n";
                        pre += "    ";
                    }

                    int stackOld = stack.Count;
                    string cont = DumpCodeBlock(func, stack, block.CodeBlocks[i.Operand1], ref pre);
                    res += cont;
                    if (stack.Count > stackOld)
                    {
                        if (branch > 0)
                        {
                            if (!elseStarted)
                            {
                                Variable newVar = new Variable();
                                newVar.VarType = VarTypes.Unknown;
                                newVar.Name = "v" + func.LocalVariables.Count;
                                func.LocalVariables.Add(newVar);
                                res += pre + newVar.Name + " = " + stack.Pop() + ";\r\n";
                                elseStarted = true;
                                pre = pre.Substring(4);
                                res += pre + "}\r\n";
                                stack.Push(newVar.Name);
                                continue;
                            }
                            else
                            {
                                Variable newVar = func.LocalVariables[func.LocalVariables.Count - 1];
                                res += pre + newVar.Name + " = " + stack.Pop() + ";\r\n";
                                if (stack.Count == 0 || stack.Peek() != newVar.Name)
                                    stack.Push(newVar.Name);
                            }
                        }
                        else
                        {

                        }
                    }
                    else
                    {
                        if (branch > 0 && !elseStarted)
                        {
                            elseStarted = i.Operand2 == 0;
                            if (i.Operand2 == 1)
                                branch--;
                            pre = pre.Substring(4);
                            res += pre + "}\r\n";
                            continue;
                        }
                    }
                    if (switchTargets != null && !switchStarted)
                        pre = pre.Substring(4);
                    if (elseStarted && branch > 0)
                    {
                        pre = pre.Substring(4);
                        res += pre + "}\r\n";
                        branch--;
                    }
                }
                else
                {
                    switch (i.Inst)
                    {
                        case Instructions.Push:
                            {
                                if (i.Operand1 == -1)
                                    stack.Push("null");
                                else
                                    stack.Push(i.Operand1.ToString());
                            }
                            break;
                        case Instructions.Pop:
                            {
                                string content = stack.Pop();
                                if (content.StartsWith("call "))
                                    res += pre + content.Replace("call ", "") + ";\r\n";
                            }
                            break;
                        case Instructions.Ref:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.PushGP:
                            {
                                stack.Push("GlobalPage");
                            }
                            break;
                        case Instructions.PushLP:
                            {
                                stack.Push("LocalPage");
                            }
                            break;
                        case Instructions.Inv:
                        case Instructions.NegF:
                            {
                                stack.Push("-" + ProcessCondition(stack.Pop()));
                            }
                            break;
                        case Instructions.Not:
                            {
                                stack.Push("!" + ProcessCondition(stack.Pop()));
                            }
                            break;
                        case Instructions.NotBit:
                            {
                                stack.Push("~" + ProcessCondition(stack.Pop()));
                            }
                            break;
                        case Instructions.Add:
                        case Instructions.AddF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " + " + op1 + ")");
                            }
                            break;
                        case Instructions.Min:
                        case Instructions.MinF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " - " + op1 + ")");
                            }
                            break;
                        case Instructions.Mul:
                        case Instructions.MulF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " * " + op1 + ")");
                            }
                            break;
                        case Instructions.Div:
                        case Instructions.DivF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " / " + op1 + ")");
                            }
                            break;
                        case Instructions.Mod:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " % " + op1 + ")");
                            }
                            break;
                        case Instructions.And:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " & " + op1 + ")");
                            }
                            break;
                        case Instructions.Or:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " | " + op1 + ")");
                            }
                            break;
                        case Instructions.Xor:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " ^ " + op1 + ")");
                            }
                            break;
                        case Instructions.Shl:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " << " + op1 + ")");
                            }
                            break;
                        case Instructions.Shr:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " >> " + op1 + ")");
                            }
                            break;
                        case Instructions.Le:
                        case Instructions.LeF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " < " + op1 + ")");
                            }
                            break;
                        case Instructions.Ge:
                        case Instructions.GeF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " > " + op1 + ")");
                            }
                            break;
                        case Instructions.Leq:
                        case Instructions.LeqF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " <= " + op1 + ")");
                            }
                            break;
                        case Instructions.Geq:
                        case Instructions.GeqF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " >= " + op1 + ")");
                            }
                            break;
                        case Instructions.Neq:
                        case Instructions.NeqF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " != " + op1 + ")");
                            }
                            break;
                        case Instructions.Eql:
                        case Instructions.EqlF:
                            {
                                string op1 = ProcessCondition(stack.Pop());
                                string op2 = ProcessCondition(stack.Pop());
                                stack.Push("(" + op2 + " == " + op1 + ")");
                            }
                            break;
                        case Instructions.Assign:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} = {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.AddA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} += {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.MinA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} -= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.MulA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} *= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.DivA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} /= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.ModA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} %= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.AndA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} &= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.OrA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} |= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.ShlA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} <<= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.ShrA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} >>= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(string.Format("{0}", refObj));
                            }
                            break;
                        case Instructions.AssignF:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} = {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.AddFA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} += {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.MinFA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} -= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.MulFA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} *= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.DivFA:
                            {
                                string val = stack.Pop();
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} /= {2};\r\n", pre, refObj, ProcessCondition(val));
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.Dup2:
                            {
                                string v1 = stack.Pop();
                                string v2 = stack.Pop();
                                stack.Push(v2);
                                stack.Push(v1);
                                stack.Push(v2);
                                stack.Push(v1);
                            }
                            break;
                        case Instructions.Dup_At3:
                            {
                                string v3 = stack.Pop();
                                string v2 = stack.Pop();
                                string v1 = stack.Pop();
                                stack.Push(v3);
                                stack.Push(v1);
                                stack.Push(v2);
                                stack.Push(v3);
                            }
                            break;
                        case Instructions.Jmp:
                            {
                                if (i.BranchTarget[0] == startIndex)
                                {
                                    if (branch == 0)
                                    {
                                        string pre2 = pre.Substring(0, pre.Length - 4);
                                        res = pre2 + "while(true)\r\n" + res;
                                    }
                                    else
                                    {
                                        string[] token = res.Split('\n');
                                        res = "";
                                        for (int j = 0; j < token.Length - 3; j++)
                                        {
                                            res += token[j] + "\n";
                                        }
                                        pre = pre.Substring(0, pre.Length - 4);
                                        string pre2 = pre.Substring(0, pre.Length - 4);
                                        res = string.Format("{1}while{0}\n", token[token.Length - 3].TrimStart(' ').Replace("if", ""), pre2) + res;
                                        branch--;
                                    }
                                }
                                else
                                {
                                    if (switchStarted)
                                    {
                                        res += pre + "break;\r\n";
                                        pre = pre.Substring(4);
                                    }
                                    if (i.BranchTarget[0] < endIndex)
                                    {
                                        res += string.Format("{0}jmp label{1};\r\n", pre, i.BranchTarget[0]);
                                    }
                                    else
                                    {
                                        if (branch > 0 || block.Instructions.Count ==1)
                                        {
                                            res += string.Format("{0}jmp label{1};\r\n", pre, i.BranchTarget[0]);
                                            if (branch > 0)
                                            {
                                                pre = pre.Substring(4);
                                                res += pre + "}\r\n";
                                                branch--;
                                            }
                                        }
                                        else
                                        {
                                        }
                                    }
                                }
                            }
                            break;
                        case Instructions.BrFalse:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                string cond = stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name;
                                res += pre + string.Format("if({0})\r\n", ProcessCondition(cond));
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BrTrue:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                string cond = stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name;
                                res += pre + string.Format("if(!({0}))\r\n", ProcessCondition(cond));
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.Ret:
                            {
                                if (func.ReturnType != VarTypes.Void)
                                    res += pre + "return " + ProcessCondition(stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name) + ";\r\n";
                                else
                                    res += pre + "return;\r\n";
                                if (branch > 0)
                                {
                                    pre = pre.Substring(4);
                                    res += pre + "}\r\n";
                                }
                            }
                            break;
                        case Instructions.Call:
                        case Instructions.CallMethod:
                        case Instructions.CallDelegate:
                            {
                                string cmd = "";
                                Function tar = null;
                                string refe = "";
                                if (i.Inst != Instructions.CallDelegate)
                                    tar = this.func[i.Operand1];
                                else
                                {
                                    int funcID;
                                    //stack.Pop();
                                    if (int.TryParse(stack.Peek(), out funcID))
                                    {
                                        tar = this.fnct[int.Parse(stack.Pop())];
                                        refe = ProcessCondition(stack.Pop());
                                    }
                                    else
                                    {
                                        stack.Pop();
                                        refe = ProcessCondition(stack.Pop());
                                        res += string.Format("{0}{1}.UnknownDelegate();\r\n", pre, refe);
                                        break;
                                    }
                                }
                                string args = "";
                                for (int j = 0; j < tar.ArgCount; j++)
                                {
                                    switch (tar.Arguments[tar.ArgCount - j - 1].VarType)
                                    {
                                        case VarTypes.IntRef:
                                        case VarTypes.FloatRef:
                                            {
                                                stack.Push("ref " + GetRef(func, stack));
                                            }
                                            break;
                                        case VarTypes.StrucRef:
                                        case VarTypes.ArrayFloatRef:
                                        case VarTypes.ArrayIntRef:
                                        case VarTypes.ArrayStringRef:
                                        case VarTypes.ArrayStructRef:
                                        case VarTypes.StringRef:
                                            {
                                                stack.Push("ref " + ProcessCondition(stack.Pop()));
                                            }
                                            break;
                                        default:
                                            break;
                                    }

                                    if (tar.Arguments[tar.ArgCount - j - 1].VarType == VarTypes.Void)
                                        continue;
                                    args = ProcessCondition(stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name) + ", " + args;
                                }
                                if (args != "")
                                    args = args.Substring(0, args.Length - 2);
                                if (i.Inst == Instructions.CallMethod)
                                    refe = stack.Pop() + ".";
                                string name = i.Inst == Instructions.CallMethod ? tar.Name.Split('@')[1] : tar.Name;
                                if (i.Inst == Instructions.CallDelegate)
                                    name = "";
                                cmd = string.Format("{2}{0}({1})", name, args, refe);
                                if (tar.ReturnType != VarTypes.Void)
                                    stack.Push("call " + cmd);
                                else
                                    res += pre + cmd + ";\r\n";
                            }
                            break;
                        case Instructions.Inc:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}++;\r\n", pre, refObj);
                            }
                            break;
                        case Instructions.Dec:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}--;\r\n", pre, refObj);
                            }
                            break;
                        case Instructions.FTOI:
                            {
                                stack.Push("((int)(" + ProcessCondition(stack.Pop()) + "))");
                            }
                            break;
                        case Instructions.ITOF:
                            {
                                stack.Push("((float)(" + ProcessCondition(stack.Pop()) + "))");
                            }
                            break;
                        case Instructions.PushF:
                            {
                                stack.Push(i.Operand1F.ToString());
                            }
                            break;
                        case Instructions.PushS:
                            {
                                stack.Push(string.Format("\"{0}\"", this.strings[i.Operand1].Replace("\n", "\\n")));
                            }
                            break;
                        case Instructions.PopStructRef:
                        case Instructions.PopS:
                            {
                                string content = stack.Pop();
                                if (content.StartsWith("call "))
                                    res += pre + content.Replace("call ", "") + ";\r\n";
                            }
                            break;
                        case Instructions.Add_S:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());
                                stack.Push(string.Format("({0} + {1})", obj1, obj2));
                            }
                            break;
                        case Instructions.AssignS:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1} = {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.PushS_Ref:
                            {
                                stack.Push(GetRef(func, stack));
                            }
                            break;
                        case Instructions.StringNeq:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());
                                stack.Push(string.Format("({0} != {1})", obj1, obj2));
                            }
                            break;
                        case Instructions.StringEql:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());
                                stack.Push(string.Format("({0} == {1})", obj1, obj2));
                            }
                            break;
                        case Instructions.AssignStructRef:
                            {
                                stack.Pop();
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1} = {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.ArrayStructRef:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("(({0}){1})", this.structs[i.Operand1].Name, refObj));
                            }
                            break;
                        case Instructions.AllocA:
                            {
                                string dimension = ProcessCondition(stack.Pop());
                                string count = "";
                                for (int j = 0; j < int.Parse(dimension); j++)
                                    count += ProcessCondition(stack.Pop()) + ", ";
                                if (count != "")
                                    count = count.Substring(0, count.Length - 2);
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.Alloc({2});\r\n", pre, refObj, count);
                            }
                            break;
                        case Instructions.ReallocA:
                            {
                                string dimension = ProcessCondition(stack.Pop());
                                string count = "";
                                for (int j = 0; j < int.Parse(dimension); j++)
                                    count += ProcessCondition(stack.Pop()) + ", ";
                                if (count != "")
                                    count = count.Substring(0, count.Length - 2);
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.Realloc({2});\r\n", pre, refObj, count);
                            }
                            break;
                        case Instructions.FreeA:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.Free();\r\n", pre, refObj);
                            }
                            break;
                        case Instructions.ArrayLength:
                            {
                                int dimension = int.Parse(stack.Pop());
                                string refObj = GetRef(func, stack);
                                if (dimension == 1)
                                {
                                    stack.Push(string.Format("{0}.Length", refObj));
                                }
                                else
                                {
                                    stack.Push(string.Format("{0}[{1}].Length", refObj, dimension));
                                }
                            }
                            break;
                        case Instructions.CopyA:
                            {
                                string count = ProcessCondition(stack.Pop());
                                string index2 = ProcessCondition(stack.Pop());
                                string a2 = ProcessCondition(stack.Pop());
                                string index1 = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.Copy({2},{3},{4},{5});\r\n", pre, refObj, index1, a2, index2, count);
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.ArrayFill:
                            {
                                string val = ProcessCondition(stack.Pop());
                                string count = ProcessCondition(stack.Pop());
                                string index = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.Fill({1},{2},{3})", refObj, index, count, val));
                            }
                            break;
                        case Instructions.CharRef:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.AssignChar:
                            {
                                string val = ProcessCondition(stack.Pop());                                
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} = {2}", pre, refObj, val);
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.Msg:
                            {
                                res += string.Format("{0}System.Message(\"{1}\");\r\n", pre, this.msgs[i.Operand1].Replace("\n", "\\n"));
                            }
                            break;
                        case Instructions.CallHLL:
                            {
                                string cmd = "";
                                Hll tar = this.hlls[i.Operand1];
                                string args = "";
                                for (int j = 0; j < tar.Functions[i.Operand2].ArgCount; j++)
                                {
                                    switch (tar.Functions[i.Operand2].Arguments[tar.Functions[i.Operand2].ArgCount - j - 1].VarType)
                                    {
                                        case VarTypes.IntRef:
                                        case VarTypes.FloatRef:
                                            {
                                                stack.Push("ref " + GetRef(func, stack));
                                            }
                                            break;
                                        case VarTypes.StrucRef:
                                        case VarTypes.ArrayFloatRef:
                                        case VarTypes.ArrayIntRef:
                                        case VarTypes.ArrayStringRef:
                                        case VarTypes.ArrayStructRef:
                                        case VarTypes.StringRef:
                                            {
                                                stack.Push("ref " + ProcessCondition(stack.Pop()));
                                            }
                                            break;
                                        default:
                                            break;
                                    }

                                    if (tar.Functions[i.Operand2].Arguments[tar.Functions[i.Operand2].ArgCount - j - 1].VarType == VarTypes.Void)
                                        continue;
                                    args = ProcessCondition(stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name) + ", " + args;
                                }
                                /*for (int j = 0; j < tar.Functions[i.Operand2].ArgCount; j++)
                                    args = ProcessCondition(stack.Pop()) + ", " + args;*/
                                if (args != "")
                                    args = args.Substring(0, args.Length - 2);
                                cmd = string.Format("{0}.{1}({2})", tar.Name, tar.Functions[i.Operand2].Name, args);
                                if (tar.Functions[i.Operand2].ReturnType != VarTypes.Void)
                                    stack.Push("call " + cmd);
                                else
                                    res += pre + cmd + ";\r\n";
                            }
                            break;
                        case Instructions.PushSP:
                            {
                                stack.Push("this");
                            }
                            break;
                        case Instructions.RefGlobal:
                            {
                                stack.Push(string.Format("Global.{0}", this.global[i.Operand1].Name));
                            }
                            break;
                        case Instructions.RefLocal:
                            {
                                stack.Push(func.LocalVariables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.Switch:
                        case Instructions.SwitchS:
                            {
                                Switch swi = this.SwitchTables[i.Operand1];
                                List<int> keys = swi.SwitchTable.Keys.ToList();
                                switchTargets = new Dictionary<int, List<string>>();
                                for (int j = 0; j < keys.Count; j++)
                                {
                                    if (!switchTargets.ContainsKey(i.BranchTarget[j]))
                                        switchTargets.Add(i.BranchTarget[j], new List<string>());
                                    if (swi.SwitchType == SwitchTypes.Integer)
                                        switchTargets[i.BranchTarget[j]].Add(keys[j].ToString());
                                    else
                                        switchTargets[i.BranchTarget[j]].Add("\"" + this.strings[keys[j]] + "\"");
                                }
                                if (swi.DefaultCase != -1)
                                {
                                    if (!switchTargets.ContainsKey(i.BranchTarget[keys.Count]))
                                        switchTargets.Add(i.BranchTarget[keys.Count], new List<string>());
                                    switchTargets[i.BranchTarget[keys.Count]].Add("default");
                                }
                                res += pre + string.Format("switch({0})\r\n", ProcessCondition(stack.Pop()));
                                res += pre + "{\r\n";
                                pre += "    ";
                            }
                            break;
                        case Instructions.CallSys:
                            {
                                string cmd = "";
                                Function tar = Function.Syscalls[(Syscalls)i.Operand1];
                                string args = "";
                                for (int j = 0; j < tar.ArgCount; j++)
                                {
                                    switch (tar.Arguments[tar.ArgCount - j - 1].VarType)
                                    {
                                        case VarTypes.IntRef:
                                        case VarTypes.FloatRef:
                                            {
                                                stack.Push("ref " + GetRef(func, stack));
                                            }
                                            break;
                                        case VarTypes.StrucRef:
                                        case VarTypes.ArrayFloatRef:
                                        case VarTypes.ArrayIntRef:
                                        case VarTypes.ArrayStringRef:
                                        case VarTypes.ArrayStructRef:
                                        case VarTypes.StringRef:
                                            {
                                                stack.Push("ref " + ProcessCondition(stack.Pop()));
                                            }
                                            break;
                                        default:
                                            break;
                                    }

                                    if (tar.Arguments[tar.ArgCount - j - 1].VarType == VarTypes.Void)
                                        continue;
                                    args = ProcessCondition(stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name) + ", " + args;
                                }
                                if (args != "")
                                    args = args.Substring(0, args.Length - 2);
                                cmd = string.Format("System.{0}({1})", tar.Name, args);
                                if (tar.ReturnType != VarTypes.Void)
                                    stack.Push("call " + cmd);
                                else
                                    res += pre + cmd + ";\r\n";
                            }
                            break;
                        case Instructions.Swap:
                            {
                                string var2 = stack.Pop();
                                string var1 = stack.Pop();
                                stack.Push(var2);
                                stack.Push(var1);
                            }
                            break;
                        case Instructions.PushField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push("this." + clsObj.Variables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.StringLen:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.Length", refObj));
                            }
                            break;
                        case Instructions.StringLenBytes:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.ByteCount", refObj));
                            }
                            break;
                        case Instructions.AssignDelegate:
                            {
                                int type = int.Parse(stack.Pop());
                                string val = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} = ({2}){3};\r\n", pre, refObj, fnct[type].Name, val);
                                stack.Push(val);
                            }
                            break;
                        case Instructions.NewObj:
                            {
                                int num = int.Parse(stack.Pop());
                                stack.Push(string.Format("new {0}()", this.structs[num].Name));
                            }
                            break;
                        case Instructions.Delete:
                            {
                                string obj = stack.Pop();
                                res += pre + obj + "= null;\r\n";
                            }
                            break;
                        case Instructions.Clone:
                            {
                                stack.Push(string.Format("call {0}.Clone()", ProcessCondition(stack.Pop())));
                            }
                            break;
                        case Instructions.Dup:
                            {
                                string obj = stack.Pop();
                                stack.Push(obj);
                                stack.Push(obj);
                            }
                            break;
                        case Instructions.SPInc:
                            {
                                res += pre + stack.Pop() + ".spinc();\r\n";
                            }
                            break;
                        case Instructions.SPDec:
                            {
                                res += pre + stack.Pop() + ".spdec();\r\n";
                            }
                            break;
                        case Instructions.StructCreateLocal:
                            {
                                res += pre + string.Format("{0} = new {1}();\r\n", func.LocalVariables[i.Operand1].Name, this.structs[i.Operand2].Name);
                            }
                            break;
                        case Instructions.StructDelLocal:
                            {
                                res += pre + string.Format("{0} = null;\r\n", func.LocalVariables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.STOI:
                            {
                                stack.Push(string.Format("call int.Parse({0})", ProcessCondition(stack.Pop())));
                            }
                            break;
                        case Instructions.ArrayPushBack:
                            {
                                string val=ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.PushBack({2});\r\n", pre, refObj, val);
                            }
                            break;
                        case Instructions.ArrayPopBack:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.PopBack();\r\n", pre, refObj);
                            }
                            break;
                        case Instructions.StringEmpty:
                            {
                                stack.Push(string.Format("call {0}.IsEmpty", ProcessCondition(stack.Pop())));
                            }
                            break;
                        case Instructions.ArrayEmpty:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.IsEmpty", refObj));
                            }
                            break;
                        case Instructions.ArrayRemove:
                            {
                                string index = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.RemoveAt({1})", refObj, index));
                            }
                            break;
                        case Instructions.IncLocal:
                            {
                                res += pre + string.Format("{0}++;\r\n", func.LocalVariables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.DecLocal:
                            {
                                res += pre + string.Format("{0}--;\r\n", func.LocalVariables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.AssignLocal:
                            {
                                res += pre + string.Format("{0} = {1};\r\n", func.LocalVariables[i.Operand1].Name, i.Operand2);
                            }
                            break;
                        case Instructions.StringIndexOf:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());
                                stack.Push(string.Format("call {0}.IndexOf({1})", obj1, obj2));
                            }
                            break;
                        case Instructions.StringSubString:
                            {
                                string count = ProcessCondition(stack.Pop());
                                string index = ProcessCondition(stack.Pop());
                                string obj = ProcessCondition(stack.Pop());
                                stack.Push(string.Format("call {0}.Substring({1},{2})", obj, index, count));
                            }
                            break;
                        case Instructions.SortA:
                            {
                                int funcID = int.Parse(stack.Pop());
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.Sort({2});\r\n", pre, refObj, this.func[funcID].Name);
                            }
                            break;
                        case Instructions.ITOB:
                            {
                                stack.Push("(" + ProcessCondition(stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name) + " == 1)");
                            }
                            break;
                        case Instructions.StringPushBack:
                            {
                                string val = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.PushBack({2});\r\n", pre, refObj, val);
                            }
                            break;
                        case Instructions.StringPopBack:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.PopBack();\r\n", pre, refObj);
                            }
                            break;
                        case Instructions.StringFormat:
                            {
                                string type = stack.Pop();
                                string val = ProcessCondition(stack.Pop());
                                string format = ProcessCondition(stack.Pop());
                                switch (type)
                                {
                                    case "2"://int
                                    case "3"://float
                                    case "4":
                                    case "48":
                                        stack.Push(format + ", " + val);
                                        break;
                                    default:
                                        break;
                                }
                            }
                            break;
                        case Instructions.AddS:
                            {
                                string val = ProcessCondition(stack.Pop());
                                string ori = ProcessCondition(stack.Pop());
                                res += (string.Format("{0}{1} += {2};\r\n", pre, ori, val));
                                stack.Push(string.Format("{0} + {1}", ori, val));
                            }
                            break;
                        case Instructions.ObjSwap:
                            {
                                stack.Pop();
                                string obj2 = GetRef(func, stack);
                                string obj1 = GetRef(func, stack);
                                res += string.Format("{0}tmp = {1};\r\n{0}{1} = {2};\r\n{0}{2} = tmp;\r\n", pre, obj1, obj2);
                            }
                            break;
                        case Instructions.StructRef:
                            {
                            }
                            break;
                        case Instructions.StringPushBackRef:
                            {
                                string val = ProcessCondition(stack.Pop());
                                string obj = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1}.PushBack({2});\r\n", pre, obj, val);
                            }
                            break;
                        case Instructions.StringPopBackRef:
                            {
                                res += string.Format("{0}{1}.PopBack();\r\n", pre, ProcessCondition(stack.Pop()));
                            }
                            break;
                        case Instructions.ITOUI:
                            {
                                string obj = ProcessCondition(stack.Pop());
                                stack.Push(string.Format("((uint){0})", obj));
                            }
                            break;
                        case Instructions.AddUI:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());

                                res += string.Format("{0}{1} += {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.MinUI:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());

                                res += string.Format("{0}{1} -= {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.MulUI:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());

                                res += string.Format("{0}{1} *= {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.DivUI:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());

                                res += string.Format("{0}{1} /= {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.ModUI:
                            {
                                string obj2 = ProcessCondition(stack.Pop());
                                string obj1 = ProcessCondition(stack.Pop());

                                res += string.Format("{0}{1} %= {2};\r\n", pre, obj1, obj2);
                                stack.Push(obj1);
                            }
                            break;
                        case Instructions.AssignUI:
                            {
                                string val = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} = {2};\r\n", pre, refObj, val);
                                stack.Push(refObj);
                            }
                            break;
                        case Instructions.ArrayFind:
                            {
                                int funcID = int.Parse(stack.Pop());
                                string cmpObj = ProcessCondition(stack.Pop());
                                string end = ProcessCondition(stack.Pop());
                                string start = ProcessCondition(stack.Pop());
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.Find({1},{2},{3},{4})", refObj, start, end, cmpObj, this.func[funcID].Name));
                            }
                            break;
                        case Instructions.ArrayReverse:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1}.Reverse();\r\n", pre, refObj);
                            }
                            break;
                        case Instructions.AssignRef:
                            {
                                string refObj = GetRef(func, stack);
                                res += string.Format("{0}{1} = {2};\r\n", pre, ProcessCondition(stack.Pop()), refObj);
                            }
                            break;
                        case Instructions.AssignFieldLocal:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("this.{0} = {1};\r\n", clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.ArraySizeField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push("this." + clsObj.Variables[i.Operand1].Name + ".Length");
                            }
                            break;
                        case Instructions.ArraySizeGlobal:
                            {
                                stack.Push("Global." + this.global[i.Operand1].Name + ".Length");
                            }
                            break;
                        case Instructions.AssignField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("this.{0} = {1};\r\n", clsObj.Variables[i.Operand1].Name, i.Operand2);
                            }
                            break;
                        case Instructions.PushLocal2:
                            {
                                stack.Push(func.LocalVariables[i.Operand1].Name);
                                stack.Push(func.LocalVariables[i.Operand1 + 1].Name);
                            }
                            break;
                        case Instructions.MinLocal:
                            {
                                res += string.Format("{0}{1} -= {2};\r\n", pre, func.LocalVariables[i.Operand1].Name, i.Operand2);
                            }
                            break;
                        case Instructions.BltLocal:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                res += pre + string.Format("if({0} >= {1})\r\n", func.LocalVariables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BgeLocal:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                res += pre + string.Format("if({0} < {1})\r\n", func.LocalVariables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.AssignArrayLocalField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                string obj = func.LocalVariables[i.Operand1].Name;
                                string index = func.LocalVariables[i.Operand1 + 1].Name;
                                string val = "this." + clsObj.Variables[i.Operand2];
                                index = index != "<void>" ? "[" + index + "]" : "";
                                res += string.Format("{0}{1}{2} = {3};\r\n", pre, obj, index, val);
                            }
                            break;
                        case Instructions.AssignGlobalLocal:
                            {
                                res += string.Format("{0}Global.{1} = {2};\r\n", pre, this.global[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.GeField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push(string.Format("({0} > {1})", clsObj.Variables[i.Operand1].Name, i.Operand2));
                            }
                            break;
                        case Instructions.AssignFieldLocalITOB:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += string.Format("{0}this.{1} = {2} == 1;\r\n", pre, clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.AssignLocalField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += string.Format("{0}{1} = this.{2};\r\n", pre, func.LocalVariables[i.Operand1].Name, clsObj.Variables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.BneFieldLocal:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("if(this.{0} == {1})\r\n", clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BgtField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("if(this.{0} <= {1})\r\n", clsObj.Variables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BrFalseField:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("if(this.{0})\r\n", clsObj.Variables[i.Operand1].Name);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BNotEmptyField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("if(this.{0}.IsEmpty)\r\n", clsObj.Variables[i.Operand1].Name);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BneLocal:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                res += pre + string.Format("if({0} == {1})\r\n", func.LocalVariables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BeqField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("if(this.{0} != {1})\r\n", clsObj.Variables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.AssignGlobal:
                            {
                                res += string.Format("{0}Global.{1} = {2};\r\n", pre, this.global[i.Operand1].Name, i.Operand2);
                            }
                            break;
                        case Instructions.ArrayPushBackFieldLocal:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += string.Format("{0}this.{1}.PushBack({2});\r\n", pre, clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.ArrayPushBackGlobalLocal:
                            {
                                res += string.Format("{0}Global.{1}.PushBack({2});\r\n", pre, this.global[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.ArrayPushBackLocal:
                            {
                                res += string.Format("{0}{1}.PushBack({2});\r\n", pre, func.LocalVariables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.BneS:
                            {
                                string refObj = GetRef(func, stack);
                                res += pre + string.Format("if({0} == \"{1}\")\r\n", refObj, this.strings[i.Operand1].Replace("\n", "\\n"));
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.AssignSRef:
                            {
                                string refObj = GetRef(func, stack);
                                string obj = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1} = {2};\r\n", pre, obj, refObj);
                            }
                            break;
                        case Instructions.StringEmptyRef:
                            {
                                string refObj = GetRef(func, stack);
                                stack.Push(string.Format("call {0}.IsEmpty", refObj));                               
                            }
                            break;
                        case Instructions.EqlSFieldLocal:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push(string.Format("(this.{0} == {1})", clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name));
                            }
                            break;
                        case Instructions.EqlSLocal:
                            {
                                stack.Push(string.Format("({0} == \"{1}\")", func.LocalVariables[i.Operand1].Name, this.strings[i.Operand2].Replace("\n", "\\n")));
                            }
                            break;
                        case Instructions.NeqSFieldLocal:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push(string.Format("(this.{0} != {1})", clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name));                            
                            }
                            break;
                        case Instructions.NeqSLocal:
                            {
                                stack.Push(string.Format("({0} != \"{1}\")", func.LocalVariables[i.Operand1].Name, this.strings[i.Operand2].Replace("\n", "\\n")));
                            }
                            break;
                        case Instructions.PushStructRefField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push(string.Format("this.{0}.Clone()", clsObj.Variables[i.Operand1].Name));
                            }
                            break;
                        case Instructions.PushSField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push("this." + clsObj.Variables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.BgtLocal:
                            {
                                if (i.BranchTarget[0] > endIndex)
                                {
                                }
                                res += pre + string.Format("if({0} <= {1})\r\n", func.LocalVariables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.BneField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += pre + string.Format("if(this.{0} == {1})\r\n", clsObj.Variables[i.Operand1].Name, i.Operand2);
                                res += pre + "{\r\n";
                                pre += "    ";
                                branch++;
                            }
                            break;
                        case Instructions.PushSGlobal:
                            {
                                stack.Push("Global." + this.global[i.Operand1].Name);
                            }
                            break;
                        case Instructions.PushSLocal:
                            {
                                stack.Push(func.LocalVariables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.AssignSConst:
                            {
                                string target = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1} = \"{2}\";\r\n", pre, target, this.strings[i.Operand1].Replace("\n", "\\n"));
                            }
                            break;
                        case Instructions.AssignSLocal:
                            {
                                string target = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1} = \"{2}\";\r\n", pre, target, func.LocalVariables[i.Operand1].Name);
                            }
                            break;
                        case Instructions.AssignSFieldLocal:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += (string.Format("{0}this.{1} = {2};\r\n)", pre, clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name));
                            }
                            break;
                        case Instructions.AssignSLocalLocal:
                            {
                                res += string.Format("{0}{1} = {2};\r\n",pre, func.LocalVariables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.ArrayPushBackLocalString:
                            {
                                res += string.Format("{0}{1}.PushBack({2});\r\n", pre, func.LocalVariables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.AssignRefCallStack:
                            {
                                string level = ProcessCondition(stack.Pop());
                                string obj = ProcessCondition(stack.Pop());
                                res += string.Format("{0}{1} = System.GetFuncStackName({2});\r\n", pre, obj, level);
                            }
                            break;
                        case Instructions.StringEmptyLocal:
                            {
                                stack.Push(string.Format("call {0}.IsEmpty", func.LocalVariables[i.Operand1].Name));
                            }
                            break;
                        case Instructions.ArrayPushBackGlobalLocalRef:
                            {
                                res += string.Format("{0}this.{1}.PushBack({2});\r\n", pre, this.global[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.ArrayPushBackFieldLocalRef:
                            {
                                Struct clsObj = GetFuncClass(func);
                                res += string.Format("{0}this.{1}.PushBack({2});\r\n", pre, clsObj.Variables[i.Operand1].Name, func.LocalVariables[i.Operand2].Name);
                            }
                            break;
                        case Instructions.StringEmptyField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push(string.Format("call this.{0}.IsEmpty", clsObj.Variables[i.Operand1].Name));
                            }
                            break;
                        case Instructions.AssignSField:
                            {
                                Struct clsObj = GetFuncClass(func);
                                string val = ProcessCondition(stack.Pop());
                                res += string.Format("{0}this.{1} = {2};\r\n", pre, clsObj.Variables[i.Operand1].Name, val);
                            }
                            break;
                        case Instructions.NeqFieldS:
                            {
                                Struct clsObj = GetFuncClass(func);
                                stack.Push(string.Format("({0} != \"{1}\")", clsObj.Variables[i.Operand1].Name, this.strings[i.Operand2].Replace("\n", "\\n")));
                            }
                            break;
                        case Instructions.LtOrGeLocal:
                            {
                                stack.Push(string.Format("({0} < {1} || {0} >= {2})", func.LocalVariables[i.Operand1].Name, i.Operand2, i.Operand3));
                            }
                            break;
                        case Instructions.AssertFile:
                        case Instructions.AssertFunc:
                        case Instructions.EndOfFunc:
                            break;
                        default:
                            throw new NotSupportedException();

                    }
                    //   res += pre + i + "\r\n";
                }
            }
            while (branch > 0)
            {
                //pre = pre.Substring(0, pre.Length - 4);
                //res += pre + "}\r\n";
                branch--;
            }
            if (switchStarted)
            {
                res += pre + "break;\r\n";
                pre = pre.Substring(8);
                res += pre + "}\r\n";
            }
            pre = pre.Substring(0, pre.Length - 4);
            if (res.StartsWith(pre + "{"))
            {
                string[] token = res.Split('\n');
                StringBuilder sb = new StringBuilder();
                res = "";
                for (int i = 1; i < token.Length; i++)
                {
                    if (string.IsNullOrEmpty(token[i]))
                        continue;
                    sb.Append(token[i].Substring(4) + "\n");
                }
                res = sb.ToString();
            }
            else
            {
                res += pre + "}\r\n";
            }
            return res;
        }

        string GetRef(Function func, Stack<string> stack)
        {
            int index;
            if (stack.Count == 0)
                return "call stack=0";
            if (int.TryParse(stack.Peek(), out index))
            {
                stack.Pop();
                string page = ProcessCondition(stack.Pop());
                if (page.StartsWith("Global."))
                {
                    page = page.Substring(7);
                    Variable var = null;
                    foreach (Variable j in this.global)
                    {
                        if (j.Name == page || ((page.IndexOf('[') >=0) && j.Name == page.Substring(0, page.IndexOf('['))))
                        {
                            var = j;
                        }
                    }
                    if (var != null)
                    {
                        switch (var.VarType)
                        {
                            case VarTypes.Struct:
                            case VarTypes.ArrayStruct:
                                Struct struc = this.structs[var.StructID];
                                return ("Global." + page + "." + struc.Variables[index].Name);
                            default:
                                return string.Format("Global.{0}[{1}]", page, index);
                        }
                    }
                    else
                        return string.Format("Global.{0}[{1}]", page, index);
                }
                else
                {
                    switch (page)
                    {
                        case "this":
                            {
                                Struct clsObj = GetFuncClass(func);
                                return ("this." + clsObj.Variables[index].Name);
                            }
                        case "LocalPage":
                            {
                                return (func.LocalVariables[index].Name);
                            }
                        case "GlobalPage":
                            {
                                return "Global." + this.global[index].Name;
                            }
                        default:
                            {
                                Variable found = null;
                                if (!page.Contains("."))
                                {
                                    foreach (Variable j in func.LocalVariables)
                                    {
                                        if (j.Name == (page.Contains("[") ? page.Substring(0, page.IndexOf('[')) : page))
                                        {
                                            found = j;
                                            break;
                                        }
                                    }
                                }
                                if (found != null)
                                {
                                    if (found.VarType == VarTypes.ArrayStructRef || found.VarType == VarTypes.Struct || found.VarType == VarTypes.StrucRef)
                                    {
                                        Struct obj = this.structs[found.StructID];
                                        return string.Format("{0}.{1}", page, obj.Variables[index].Name);
                                    }
                                }
                                return (string.Format("{0}[{1}]", page, index));
                            }
                    }
                }
            }
            else
            {
                string indexS = ProcessCondition(stack.Pop());
                string page = ProcessCondition(stack.SafePop(func));
                switch (indexS)
                {
                    case "<void>":
                        return (page);
                    default:
                        return (string.Format("{0}[{1}]", page, indexS));
                }
            }
            throw new NotSupportedException();
        }



        Struct GetFuncClass(Function func)
        {
            string className = func.Name.Split('@')[0];
            var cls = from c in this.Structures
                      where c.Name == className
                      select c;
            Struct clsObj = cls.First();
            return clsObj;
        }

        string ProcessCondition(string cond)
        {
            if (cond.StartsWith("("))
            {
                cond = cond.Substring(1, cond.Length - 1);
                cond = cond.Substring(0, cond.Length - 1);
            }
            return cond.Replace("call ", "");
        }

        int AnalyzeCodeBlock(CodeBlock block, List<Instruction> insns, Dictionary<Instruction, CodeBlock> blockMapping, int index, int endAddress)
        {
            int i = index;
            int continueAddr = int.MaxValue;
            while (i < insns.Count)
            {
                if (i == endAddress)
                    return i;
                Instruction insn = insns[i];
                if (i == index)
                {
                    if (blockMapping.ContainsKey(insn))
                    {
                        Instruction lastInsn = blockMapping[insn].Instructions[blockMapping[insn].Instructions.Count - 1];
                        CodeBlock cur = blockMapping[insn];
                        while (lastInsn.Inst == Instructions.DummyCodeBlock)
                        {
                            cur = cur.CodeBlocks[lastInsn.Operand1];
                            lastInsn = cur.Instructions[cur.Instructions.Count - 1];
                        }
                        foreach (Instruction j in blockMapping[insn].Instructions)
                            block.Instructions.Add(j);
                        foreach (CodeBlock j in blockMapping[insn].CodeBlocks)
                            block.CodeBlocks.Add(j);
                        return insns.IndexOf(lastInsn) + 1;
                    }
                    blockMapping.Add(insn, block);
                }
                if (insn.IsBranchTarget && i != index)
                {
                    CodeBlock newBlock = new CodeBlock();
                    block.CodeBlocks.Add(newBlock);
                    Instruction dummy = new Instruction();
                    dummy.Inst = Instructions.DummyCodeBlock;
                    dummy.Operand1 = block.CodeBlocks.Count - 1;
                    block.Instructions.Add(dummy);
                    i = AnalyzeCodeBlock(newBlock, insns, blockMapping, i, -1);
                    if (newBlock.Instructions.Count == 1)
                    {
                        if (newBlock.Instructions[0].BranchTarget.Count > 0)
                        {
                        }
                    }
                    continue;
                }
                int elseTarget = insn.BranchTarget.Count > 0 ? insn.BranchTarget[0] : int.MaxValue;
                block.Instructions.Add(insn);
                i++;
                if (insn.StartNewBlock)
                {
                    if (insn.BranchTarget[0] > i)
                    {
                        if (insns[i].EndBlock)
                        {
                        }
                        int tmpI = i;
                        int tmpI2 = i;
                        CodeBlock newBlock = new CodeBlock();
                        CodeBlock newBlock2 = null;
                        Instruction dummy2 = null;
                        block.CodeBlocks.Add(newBlock);
                        Instruction dummy = new Instruction();
                        dummy.Inst = Instructions.DummyCodeBlock;
                        dummy.Operand1 = block.CodeBlocks.Count - 1;
                        block.Instructions.Add(dummy);
                        i = AnalyzeCodeBlock(newBlock, insns, blockMapping, i, elseTarget);
                        /*Instruction lastInsn = newBlock.Instructions[newBlock.Instructions.Count - 1];
                        if (lastInsn.Inst == Instructions.DummyCodeBlock)
                        {
                            lastInsn = newBlock.CodeBlocks[lastInsn.Operand1].Instructions.Last();
                        }
                        if (lastInsn.BranchTarget.Count > 0)
                        {
                            continueAddr = lastInsn.BranchTarget[0];
                        }*/
                        continueAddr = FindContinueAddress(elseTarget, newBlock);
                        if ((continueAddr != -1) && !insns[continueAddr - 1].EndBlock)
                        {
                            Instruction continueInsn = insns[continueAddr - 1];
                            continueInsn.EndBlock = true;
                            continueInsn.BranchTarget.Add(continueAddr);
                        }
                        if (continueAddr == elseTarget)
                        {
                            dummy.Operand2 = 1;
                        }

                        bool shouldReturn = false;
                        if (elseTarget < continueAddr)
                        {
                            i = elseTarget;
                            if (i != elseTarget)
                            {
                            }
                            newBlock2 = new CodeBlock();
                            block.CodeBlocks.Add(newBlock2);
                            dummy2 = new Instruction();
                            dummy2.Inst = Instructions.DummyCodeBlock;
                            dummy2.Operand1 = block.CodeBlocks.Count - 1;
                            block.Instructions.Add(dummy2);
                            i = AnalyzeCodeBlock(newBlock2, insns, blockMapping, i, continueAddr);
                            //tmpI2 = i;
                            if (i == continueAddr)
                                shouldReturn = true;
                        }
                        else
                        {
                            shouldReturn = true;
                        }
                        /*i = tmpI;
                        newBlock = new CodeBlock();
                        block.CodeBlocks.Add(newBlock);
                        dummy = new Instruction();
                        dummy.Inst = Instructions.DummyCodeBlock;
                        dummy.Operand1 = block.CodeBlocks.Count - 1;
                        block.Instructions.Add(dummy);
                        i = AnalyzeCodeBlock(newBlock, insns, blockMapping, i);

                        if (newBlock2 != null)
                        {
                            block.CodeBlocks.Add(newBlock2);
                            dummy2.Operand1 = block.CodeBlocks.Count - 1;
                            block.Instructions.Add(dummy2);
                            i = tmpI2;
                        }*/
                        if ((shouldReturn && endAddress == -1) || i >= endAddress)
                            return i;
                        else
                            continue;
                    }
                }

                if ((insn.EndBlock && (((insn.BranchTarget.Count == 0) || insn.BranchTarget[0] > endAddress) || endAddress == -1) && (insn.Inst != Instructions.Ret || (i < insns.Count && insns[i].IsBranchTarget))) || (i >= endAddress && endAddress >= 0))
                {
                    if (i > 1 && (insns[i - 2].Inst == Instructions.Switch || insns[i - 2].Inst == Instructions.SwitchS))
                        endAddress = insns[i - 1].BranchTarget[0];
                    else
                        return i;
                }
            }
            return i;
        }

        int FindContinueAddress(int endIndex, CodeBlock block)
        {
            for (int index = block.Instructions.Count - 1; index >= 0; index--)
            {
                Instruction i = block.Instructions[index];
                if (i.BranchTarget.Count > 0 && i.BranchTarget[0] >= endIndex)
                    return i.BranchTarget[0];
                if (i.Inst == Instructions.DummyCodeBlock)
                {
                    int ret = FindContinueAddress(endIndex, block.CodeBlocks[i.Operand1]);
                    if (ret >= endIndex)
                        return ret;
                }
            }
            return -1;
        }

        static string ReadString(System.IO.BinaryReader br)
        {
            byte c;
            int counter = 0;
            do
            {
                c = br.ReadByte();
                counter++;
            } while (c != '\0');
            br.BaseStream.Position -= counter;
            return Encoding.GetEncoding("shift-jis").GetString(br.ReadBytes(counter)).Trim('\0');
        }
    }
}
