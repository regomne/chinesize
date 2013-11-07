using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public enum SwitchTypes
    {
        Integer=2,
        String =4,
    }
    public class Switch
    {
        Dictionary<int, int> table = new Dictionary<int, int>();
        public SwitchTypes SwitchType { get; set; }
        public int DefaultCase { get; set; }
        public Dictionary<int, int> SwitchTable { get { return table; } }
    }
}
