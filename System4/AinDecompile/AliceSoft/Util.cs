using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public static class Util
    {
        public static string GetDimension(this int dimension)
        {
            string res = "";
            for (int i = 0; i < dimension - 1; i++)
            {
                res += ",";
            }
            return "[" + res + "]";
        }
        public static string GetTypeName(this VarTypes type, Assembly asm, int StructID, int dimension)
        {
            string name = "";
            try
            {
                switch (type)
                {
                    case VarTypes.StrucRef:
                        name = "ref " + asm.Structures[StructID].Name;
                        break;
                    case VarTypes.Struct:
                        name = asm.Structures[StructID].Name;
                        break;
                    case VarTypes.ArrayStruct:
                        name = asm.Structures[StructID].Name + dimension.GetDimension();
                        break;
                    case VarTypes.IntRef:
                        name = "ref int";
                        break;
                    case VarTypes.FloatRef:
                        name = "ref float";
                        break;
                    case VarTypes.StringRef:
                        name = "ref string";
                        break;
                    case VarTypes.ArrayIntRef:
                        name = "ref int" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayFloatRef:
                        name = "ref float" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayStringRef:
                        name = "ref string" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayStructRef:
                        name = "ref " + asm.Structures[StructID].Name + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayInt:
                        name = "int" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayFloat:
                        name = "float" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayString:
                        name = "string" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayBool:
                        name = "bool" + dimension.GetDimension();
                        break;
                    case VarTypes.ArrayDelegate:
                        name = "delegate" + dimension.GetDimension();
                        break;
                    default:
                        name = type.ToString().ToLower();
                        break;
                }
            }
            catch
            {
                name = type.ToString().ToLower();
            }
            return name;
        }

        public static string SafePop(this Stack<string> stack, Function func)
        {
            return stack.Count > 0 ? stack.Pop() : func.LocalVariables[func.LocalVariables.Count - 1].Name;
        }

        public static void AddArg(this Function func, VarTypes type)
        {
            Variable var = new Variable();
            var.VarType = type;
            func.Arguments.Add(var);
        }
    }
}
