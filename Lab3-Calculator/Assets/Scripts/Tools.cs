using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
namespace Calc
{
	class Tools
	{
		public static bool IsNum(char x)
		{
			if (x >= '0' && x <= '9') return true;
			else return false;
		}

        //1-2-3
        //给定浮点数的左边界和表达式，返回浮点数的右边界
        public static int GetNextNum(int start, string exp)
		{
            bool flag = true;
			if (start >= exp.Length) return 0;
			int len = 0;
			while (IsNum(exp[start])|| exp[start] == '.'|| flag)
			{
                flag = false;
				start++;
				len++;
				if (start >= exp.Length) break;
			}
			return len;
		}

		public static void InitExp(ref string exp)
		{
			//消除空格
			exp = exp.Replace(" ", "");
			//lg改成l
			exp = exp.Replace("lg", "l");
			//√改成@
			exp = exp.Replace("√", "@");
			exp = exp.Replace("×", "*");
			exp = exp.Replace("÷", "/");
            
            
            
		}
	}
}
