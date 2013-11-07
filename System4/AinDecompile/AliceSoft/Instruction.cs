using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public enum Instructions
    {
        Push,
        Pop,
        Ref,
        PushGP = 4,
        PushLP = 5,
        Inv,
        Not,
        NotBit,
        Add,
        Min,
        Mul,
        Div,
        Mod,
        And,
        Or,
        Xor,
        Shl,
        Shr,
        Le = 19,
        Ge,
        Leq,
        Geq,
        Neq,
        Eql = 24,
        Assign = 25,
        AddA,
        MinA,
        MulA,
        DivA,
        ModA,
        AndA,
        OrA,
        XorA,
        ShlA,
        ShrA,
        AssignF = 36,
        AddFA,
        MinFA,
        MulFA = 39,
        DivFA,
        Dup2 = 41,
        Dup_At3,
        Jmp = 44,
        BrFalse,
        BrTrue,
        Ret = 47,
        Call = 48,
        Inc,
        Dec,
        FTOI = 51,
        ITOF,
        NegF,
        AddF,
        MinF,
        MulF,
        DivF = 57,
        GeF,
        LeF,
        GeqF,
        LeqF,
        NeqF,
        EqlF,
        PushF = 64,
        PushS = 65,
        PopS,
        Add_S,
        AssignS = 68,
        PushS_Ref = 70,
        StringNeq = 72,
        StringEql = 73,
        PopStructRef = 77,
        AssignStructRef = 78,
        ArrayStructRef = 79,
        AllocA = 81,
        ReallocA = 82,
        FreeA,
        ArrayLength,
        CopyA = 85,
        ArrayFill,
        CharRef,
        AssignChar,
        Msg,
        CallHLL = 90,
        PushSP = 91,
        CallMethod = 92,
        RefGlobal,
        RefLocal,
        Switch,
        SwitchS,
        AssertFunc = 97,
        AssertFile,
        CallSys = 99,
        Swap = 102,
        PushField = 103,
        StringLen = 104,
        StringLenBytes,
        CallDelegate = 107,
        AssignDelegate = 110,
        NewObj = 118,
        Delete = 119,
        Clone = 121,
        Dup = 122,
        SPInc = 124,
        SPDec = 125,
        EndOfFunc = 126,
        StructCreateLocal = 129,
        StructDelLocal = 130,
        STOI,
        ArrayPushBack = 132,
        ArrayPopBack,
        StringEmpty = 134,
        ArrayEmpty = 135,
        ArrayRemove,
        IncLocal = 138,
        DecLocal,
        AssignLocal = 140,
        ITOB,
        StringIndexOf,
        StringSubString = 143,
        SortA = 144,
        StringPushBack,
        StringPopBack,
        StringFormat = 148,
        AddS,
        ObjSwap,
        StructRef = 152,
        StringPushBackRef = 154,
        StringPopBackRef,
        ITOUI,
        AddUI,
        MinUI,
        MulUI,
        DivUI,
        ModUI,
        AssignUI=162,
        ArrayFind = 175,
        ArrayReverse,
        AssignRef = 177,
        AssignFieldLocal = 178,
        ArraySizeGlobal = 179,
        ArraySizeField = 180,
        AssignField = 181,
        PushLocal2 = 182,
        MinLocal,
        BltLocal = 184,
        BgeLocal = 185,
        AssignArrayLocalField,
        AssignGlobalLocal = 188,
        GeField = 189,
        AssignFieldLocalITOB,
        AssignLocalField = 191,
        BneFieldLocal = 192,
        BgtField,
        BrFalseField = 199,
        BNotEmptyField,
        BgtLocal = 201,
        BneField,
        BneLocal = 204,
        BeqField,
        AssignGlobal,
        ArrayPushBackFieldLocal = 208,
        ArrayPushBackGlobalLocal,
        ArrayPushBackLocal = 210,
        BneS,
        AssignSRef = 212,
        StringEmptyRef = 214,
        EqlSFieldLocal = 215,
        EqlSLocal = 216,
        NeqSFieldLocal,
        NeqSLocal = 218,
        PushStructRefField = 219,
        PushSField = 220,
        PushSGlobal = 223,
        PushSLocal = 224,
        AssignSLocalLocal,
        ArrayPushBackLocalString = 226,
        AssignRefCallStack = 227,
        AssignSConst = 228,
        AssignSLocal = 229,
        AssignSFieldLocal = 230,
        StringEmptyLocal,
        ArrayPushBackGlobalLocalRef,
        ArrayPushBackFieldLocalRef,
        StringEmptyField,
        AssignSField = 236,
        NeqFieldS = 238,
        LtOrGeLocal=240,
        DummyCodeBlock = 10000000,
    }

    public enum Syscalls
    {
        Exit,
        GlobalSave,
        GlobalLoad,
        LockPeek,
        UnlockPeek,
        Reset,
        Output,
        MsgBox,
        ResumeSave,
        ResumeLoad,
        ExistFile,
        OpenWeb,
        GetSaveFolderName,
        GetTime,
        GetGameName,
        Error,
        ExistSaveFile,
        IsDebugMode,
        MsgBoxOkCancel,
        GetFuncStackName,
        Peek,
        Sleep,
        ResumeWriteComment,
        ResumeReadComment,
        GroupSave,
        GroupLoad,
        DeleteSaveFile,
        ExistFunc,
        CopySaveFile,
    }
    public class Instruction
    {
        List<int> branches = new List<int>();
        public Instructions Inst { get; set; }
        public int Operand1 { get; set; }
        public int Operand2 { get; set; }
        public int Operand3 { get; set; }
        public float Operand1F { get; set; }
        public Assembly Assembly { get; set; }
        public Function Function { get; set; }
        public bool IsBranchTarget { get; set; }
        public int BranchFrom { get; set; }
        public List<int> BranchTarget { get { return branches; } }
        public bool EndBlock { get; set; }
        public bool StartNewBlock { get; set; }

        public static Instruction ParseInstruction(System.IO.BinaryReader br,Assembly asm,Function func)
        {
            Instructions op = (Instructions)br.ReadInt16();
            Instruction insn = new Instruction();
            insn.Assembly = asm;
            insn.Function = func;
            insn.Inst = op;
            switch (op)
            {
                case Instructions.BneLocal:
                case Instructions.BgeLocal:
                case Instructions.BltLocal:
                case Instructions.BgtLocal:
                case Instructions.BneField:
                case Instructions.BeqField:
                case Instructions.BneFieldLocal:
                case Instructions.BgtField:
                    insn.Operand1 = br.ReadInt32();
                    insn.Operand2 = br.ReadInt32();
                    insn.Operand3 = br.ReadInt32();
                    insn.BranchTarget.Add(insn.Operand3);
                    insn.StartNewBlock = true;
                    break;
                case Instructions.Jmp:
                    insn.Operand1 = br.ReadInt32();
                    insn.BranchTarget.Add(insn.Operand1);
                    insn.EndBlock = true;
                    break;
                case Instructions.BrFalse:
                case Instructions.BrTrue:                    
                    insn.Operand1 = br.ReadInt32();
                    insn.BranchTarget.Add(insn.Operand1);
                    insn.StartNewBlock = true;
                    break;
                case Instructions.Switch:
                case Instructions.SwitchS:
                    insn.Operand1 = br.ReadInt32();
                    Switch swi = asm.SwitchTables[insn.Operand1];
                    foreach (int i in swi.SwitchTable.Values)
                        insn.BranchTarget.Add(i);
                    if (swi.DefaultCase != -1)
                        insn.BranchTarget.Add(swi.DefaultCase);
                    break;
                case Instructions.Push:
                case Instructions.CallMethod:
                case Instructions.Call:
                case Instructions.RefGlobal:
                case Instructions.RefLocal:
                case Instructions.PushS:
                case Instructions.AssertFunc:
                case Instructions.AssertFile:
                case Instructions.CallSys:
                case Instructions.EndOfFunc:
                case Instructions.StructDelLocal:
                case Instructions.PushField:
                case Instructions.IncLocal:
                case Instructions.DecLocal:
                case Instructions.ArraySizeField:
                case Instructions.ArraySizeGlobal:
                case Instructions.PushLocal2:
                case Instructions.PushSLocal:
                case Instructions.AssignSConst:
                case Instructions.PushSField:
                case Instructions.ArrayStructRef:
                case Instructions.PushSGlobal:
                case Instructions.AssignSLocal:
                case Instructions.StringEmptyLocal:
                case Instructions.StringEmptyField:
                case Instructions.StructRef:
                case Instructions.AssignSField:
                case Instructions.Msg:
                    insn.Operand1 = br.ReadInt32();
                    break;
                case Instructions.PushF:
                    insn.Operand1F = br.ReadSingle();
                    break;
                case Instructions.BrFalseField:
                case Instructions.BNotEmptyField:
                case Instructions.BneS:
                    insn.Operand1 = br.ReadInt32();
                    insn.Operand2 = br.ReadInt32();
                    insn.BranchTarget.Add(insn.Operand2);
                    insn.StartNewBlock = true;
                    break;
                case Instructions.StructCreateLocal:
                case Instructions.AssignLocal:
                case Instructions.AssignField:
                case Instructions.CallHLL:
                case Instructions.AssignFieldLocal:
                case Instructions.AssignGlobalLocal:
                case Instructions.AssignLocalField:
                case Instructions.ArrayPushBackLocal:
                case Instructions.AssignGlobal:
                case Instructions.GeField:
                case Instructions.EqlSLocal:
                case Instructions.NeqSLocal:
                case Instructions.EqlSFieldLocal:
                case Instructions.AssignSFieldLocal:
                case Instructions.PushStructRefField:
                case Instructions.ArrayPushBackFieldLocal:
                case Instructions.AssignArrayLocalField:
                case Instructions.ArrayPushBackLocalString:
                case Instructions.ArrayPushBackGlobalLocalRef:
                case Instructions.ArrayPushBackFieldLocalRef:
                case Instructions.AssignFieldLocalITOB:
                case Instructions.ArrayPushBackGlobalLocal:
                case Instructions.MinLocal:
                case Instructions.NeqFieldS:
                case Instructions.NeqSFieldLocal:
                case Instructions.AssignSLocalLocal:
                    insn.Operand1 = br.ReadInt32();
                    insn.Operand2 = br.ReadInt32();
                    break;
                case Instructions.LtOrGeLocal:
                    insn.Operand1 = br.ReadInt32();
                    insn.Operand2 = br.ReadInt32();
                    insn.Operand3 = br.ReadInt32();
                    break;
                case Instructions.Ret:
                    insn.EndBlock = true;
                    break;
                case Instructions.PushSP:
                case Instructions.Le:
                default:
                    break;
            }
            return insn;
        }

        public override string ToString()
        {
            switch (Inst)
            {
                case Instructions.Push:
                    return "push " + Operand1;
                case Instructions.Jmp:
                    return "jmp label" + BranchTarget[0];
                case Instructions.BrFalse :
                    return "brfalse label" + BranchTarget[0];
                case Instructions.BrTrue:
                    return "brtrue label" + BranchTarget[0];
                case Instructions.Switch:
                case Instructions.SwitchS:
                    return "switch " + Operand1;
                case Instructions.ArrayStructRef:
                    return "arraystructref " + Assembly.Structures[Operand1].Name;
                case Instructions.Ret:
                    return "ret";
                case Instructions.PushField:
                case Instructions.AssignField:
                case Instructions.BrFalseField:
                case Instructions.AssignFieldLocal:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        if (Inst == Instructions.AssignFieldLocal)
                            return string.Format("assignfield_local {0},{1}", clsObj.Variables[Operand1].Name, Function.LocalVariables[Operand2].Name);
                        if (Inst == Instructions.BrFalseField)
                            return string.Format("brfalse_field {0},label{1}", clsObj.Variables[Operand1].Name, branches[0]);
                        else 
                            return Inst == Instructions.PushField ? "pushfield " + clsObj.Variables[Operand1].Name : string.Format("assignfield {0},{1}", clsObj.Variables[Operand1].Name, Operand2);
                    }
                case Instructions.RefGlobal:
                    return "refglobal " + Assembly.GlobalVariables[Operand1].Name;
                case Instructions.RefLocal:
                    return "reflocal " + Function.LocalVariables[Operand1].Name;
                case Instructions.PushSLocal:
                    return "pushs_local " + Function.LocalVariables[Operand1].Name;
                case Instructions.PushLocal2:
                    return "pushlocal_2 " + Function.LocalVariables[Operand1].Name + "," + Function.LocalVariables[Operand1 + 1].Name;
                case Instructions.AssignLocal:
                    return string.Format("assignlocal {0},{1}", Function.LocalVariables[Operand1].Name, Operand2);
                case Instructions.ArrayPushBackLocal:
                    return string.Format("arraypushback_local {0},{1}", Function.LocalVariables[Operand1].Name,  Function.LocalVariables[Operand2].Name);
                case Instructions.AssignGlobalLocal:
                    return string.Format("assignglobal_locl {0},{1}", Assembly.GlobalVariables[Operand1].Name, Function.LocalVariables[Operand2].Name);
                case Instructions.CallMethod:
                case Instructions.Call:
                    return "call " + Assembly.Functions[Operand1].Name;
                case Instructions.PushF:
                    return "pushf " + Operand1F;
                case Instructions.PushS:
                    return "pushs \"" + Assembly.StringList[Operand1] + "\"";
                case Instructions.CallSys:
                    return "callsys System." + ((Syscalls)Operand1);
                case Instructions.StructCreateLocal:
                    return string.Format("structcreate {0},{1}", Function.LocalVariables[Operand1].Name, Assembly.Structures[Operand2].Name);
                case Instructions.StructDelLocal:
                    return "structdel_local " + Function.LocalVariables[Operand1].Name;
                case Instructions.IncLocal:
                    return "inclocal " + Function.LocalVariables[Operand1].Name;
                case Instructions.EqlSLocal:
                    return "eqls_local " + Function.LocalVariables[Operand1].Name + "," + Assembly.StringList[Operand2];                
                case  Instructions.CallHLL:
                    return string.Format("callhll {0}.{1}", Assembly.HLLs[Operand1].Name, Assembly.HLLs[Operand1].Functions[Operand2].Name);
                case Instructions.ArraySizeField:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return string.Format("arraysize_field {0}", clsObj.Variables[Operand1].Name);
                    }
                case Instructions.BNotEmptyField:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return string.Format("bn_empty_field {0}", clsObj.Variables[Operand1].Name, branches[0]);
                    }
                case Instructions.AssignLocalField:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return string.Format("assignlocal_field {0},{1}", Function.LocalVariables[Operand1].Name, clsObj.Variables[Operand2].Name);
                    }
                
                case Instructions.ArraySizeGlobal:
                    {
                        return string.Format("arraysize_global {0}", Assembly.GlobalVariables[Operand1].Name);
                    }
                case Instructions.BneLocal:
                    return string.Format("bnelocal {0},{1},label{2}", Function.LocalVariables[Operand1].Name, Operand2, branches[0]);
                case Instructions.BltLocal:
                    return string.Format("bltlocal {0},{1},label{2}", Function.LocalVariables[Operand1].Name, Operand2, branches[0]);
                case Instructions.BgeLocal:
                    return string.Format("bgelocal {0},{1},label{2}", Function.LocalVariables[Operand1].Name, Operand2, branches[0]);
                case Instructions.BgtLocal:
                    return string.Format("bgtlocal {0},{1},label{2}", Function.LocalVariables[Operand1].Name, Operand2, branches[0]);
                case Instructions.PushSField:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return "pushs_field " + clsObj.Variables[Operand1].Name;
                    }
                case Instructions.EqlSFieldLocal:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return "eqls_field_local " + clsObj.Variables[Operand1].Name + "," + Function.LocalVariables[Operand2].Name;
                    }
                case Instructions.AssignSFieldLocal:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return "assigns_field_local " + clsObj.Variables[Operand1].Name + "," + Function.LocalVariables[Operand2].Name;
                    }
                case Instructions.GeField:
                    {
                        string className = Function.Name.Split('@')[0];
                        var cls = from c in Assembly.Structures
                                  where c.Name == className
                                  select c;
                        Struct clsObj = cls.First();
                        return "ge_field " + clsObj.Variables[Operand1].Name + "," + Operand2;
                    }
                
                case Instructions.AssignSConst:
                    return string.Format("assignS {0}", Assembly.StringList[Operand1]);
                case Instructions.AssertFunc:
                    return "assertfunc " + Assembly.Functions[Operand1];
                default:
                    return Inst.ToString().ToLower();
            }
        }

    }
}
