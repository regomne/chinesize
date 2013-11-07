using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Decompiler.Assembly
{
    public class CodeBlock
    {
        List<Instruction> insns = new List<Instruction>();
        List<CodeBlock> blocks = new List<CodeBlock>();

        public List<Instruction> Instructions { get { return insns; } }
        public List<CodeBlock> CodeBlocks { get { return blocks; } }

        public int EndIndex(Function func)
        {
            Instruction last = Instructions[Instructions.Count - 1];
            CodeBlock block = this;
            while (last.Inst == Decompiler.Assembly.Instructions.DummyCodeBlock)
            {
                block = block.CodeBlocks[last.Operand1];
                last = block.Instructions[block.Instructions.Count - 1];
            }
            return func.Instructions.IndexOf(last);
        }
    }
}
