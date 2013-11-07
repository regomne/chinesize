using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public enum VarTypes
    {
        Void = 0,
        Int = 10,
        Float,
        String,
        Struct,
        ArrayInt = 14,
        ArrayFloat,
        ArrayString,
        ArrayStruct = 17,
        IntRef = 18,
        FloatRef,
        StringRef,
        StrucRef = 21,
        ArrayIntRef = 22,
        ArrayFloatRef,
        ArrayStringRef,
        ArrayStructRef = 25,
        Delegate = 27,
        ArrayDelegate = 30,
        Bool = 47,
        ArrayBool = 50,
        UInt = 55,
        Unknown = 100000,
    }
    public class Variable
    {
        public string Name { get; set; }
        public VarTypes VarType { get; set; }
        public int StructID { get; set; }
        public int Dimension { get; set; }
        public int U3 { get; set; }
        public int IntVal { get; set; }
        public float FloatVal { get; set; }
        public bool BoolVal { get; set; }
        public string StringVal { get; set; }
        public Assembly Assembly { get; set; }
        public override string ToString()
        {
            return string.Format("{0} {1}", Util.GetTypeName(VarType, Assembly, StructID, Dimension), Name);
        }
    }
}
