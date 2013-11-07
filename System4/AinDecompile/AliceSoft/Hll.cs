using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public class Hll
    {
        List<Function> funcs = new List<Function>();
        public string Name { get; set; }
        public List<Function> Functions { get { return funcs; } }
    }
}
