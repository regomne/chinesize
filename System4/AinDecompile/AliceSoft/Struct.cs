using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public enum StructTypes
    {
        Struct,
        Class,
    }
    public class Struct
    {
        List<Variable> vars = new List<Variable>();
        List<Function> funcs = new List<Function>();
        public string Name { get; set; }
        public StructTypes Type { get; set; }
        public Function Constructor { get; set; }
        public Function Destructor { get; set; }
        public List<Function> Functions { get { return funcs; } }
        public int VarCount { get; set; }

        public List<Variable> Variables { get { return vars; } }

        public override string ToString()
        {
            string vars = "";
            foreach (Variable i in Variables)
            {
                vars += string.Format("    {0} {1};\r\n", Util.GetTypeName(i.VarType, i.Assembly, i.StructID, i.Dimension).Replace("ref ", ""), i.Name);
            }
            string funcs = "";
            if (Constructor != null)
                funcs += Constructor.SourceCode.Replace("void 0 (", Name + "(") + "\r\n";
            if (Destructor != null)
                funcs += Destructor.SourceCode.Replace("void 1 (", "~" + Name + "(") + "\r\n";
            foreach (Function i in this.funcs)
                funcs += i.SourceCode + "\r\n";
            return string.Format("{2} {0}\r\n[[[\r\n{1}\r\n{3}]]]", Name, vars, Type.ToString().ToLower(), funcs).Replace("[[[", "{").Replace("]]]", "}");
        }
    }
}
