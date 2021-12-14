using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
namespace Calc
{
	class Calc
	{
		//优先矩阵
		private int[,] priority = new int[10, 10] {
			{ 1 , 1 ,  0  , 0 ,  0  , 0  , 0  , 0 ,  0 ,  1 },
			{ 1 , 1 ,  0  , 0 ,  0  , 0  , 0  , 0 ,  0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  0  , 0  , 0  , 1 ,  0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  0  , 0  , 0  , 1 ,  0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  0  , 1  , 0  , 1 ,  0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  0  , 0  , 0  , 1 ,  0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  0  , 1  , 0  , 1,   0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  0  , 0  , 0  , 1 ,  0 ,  1 },
			{ 0 , 0 ,  0  , 0 ,  0  , 0  , 0  , 0 ,  0 ,  1 },
			{ 1 , 1 ,  1  , 1 ,  1  , 1  , 1  , 1 ,  1 ,  1 }

		};
		//运算符 l代表log
		private char[] op = new char[10] { '+', '-', '*', '/', '@', '^', 'l', '%', '(', ')' };
		//运算符与编号映射
		private Dictionary<char, int> opDic;

		public Calc()
		{
			opDic = new Dictionary<char, int>();
			for (int i = 0; i < 10; i++)
			{
				opDic.Add(op[i], i);
			}
		}

		private void ChooseOp(char c, ref Stack<double> num)
		{
			switch (c)
			{
				case '+':
					double x = num.Pop();
					double y = num.Pop();
					num.Push(Operation.Add(x, y));
					break;
				case '-':
					x = num.Pop();
					y = num.Pop();
					num.Push(Operation.Sub(y, x));
					break;
				case '*':
					x = num.Pop();
					y = num.Pop();
					num.Push(Operation.Multi(x, y));
					break;
				case '/':
					x = num.Pop();
					y = num.Pop();
					num.Push(Operation.Divide(y, x));
					break;
				case '@':
					x = num.Pop();
					num.Push(Operation.Sqrt(x));
					break;
				case '^':
					x = num.Pop();
					y = num.Pop();
					num.Push(Operation.Pow(y, x));
					break;
				case 'l':
					x = num.Pop();
					num.Push(Operation.Log(x));
					break;
				case '%':
					x = num.Pop();
					y = num.Pop();
					num.Push(Operation.Mod(y, x));
					break;
			}
		}
		public double Calculate(string exp)
		{
			double ans = 0;
			Tools.InitExp(ref exp);
            Debug.Log("exp = " + exp);
			Stack<double> num = new Stack<double>();
			Stack<char> op = new Stack<char>();
			 num.Push(0);
			for (int i = 0; i < exp.Length; i++)
			{
				if (Tools.IsNum(exp[i]) || (exp[i] == '-' && i>=1 && !Tools.IsNum(exp[i-1])))
				{
					int len = Tools.GetNextNum(i, exp);
                    Debug.Log("len = " + len);
                    // Console.WriteLine(exp.Substring(i, len));
                    string re = exp.Substring(i, len);
                    Debug.Log("re = " + re);
                    double x;
                    //if (re[0] == '-')
                    //{
                    //    re = re.Replace("-", "");
                    //    x = -double.Parse(re);
                    //}
                    //else
                    //{
                    //    x = double.Parse(re);
                    //}
                    x = Double.Parse(re);
                    //Console.WriteLine("x = " + x);
                  
					num.Push(x);
					i += len - 1;
				}
				else
				{
					//左右括号 栈空 各个运算符所需要的操作数 等条件
					//如果是运算符则看运算符栈顶情况
					if (op.Count == 0)
					{
						op.Push(exp[i]);
					}
					//如果是右括号则匹配到左括号为止，右括号不入栈
					else if (exp[i] == ')')
					{
						while (op.Peek() != '(')
						{
							char c = op.Pop();
							//运算
							ChooseOp(c, ref num);
						}
						op.Pop();//左括号弹出
					}
					//1.85+24*6*(12.3/2+4)+sqrt5
					else//如果不是右括号
					{
						char top = op.Peek();
						int re = priority[opDic[top], opDic[exp[i]]];
						if (re == 0)//栈顶运算符优先级低，则当前运算符入栈
						{
							op.Push(exp[i]);
						}
						else//栈顶运算符优先级高，则进行运算，最后把当前运算符入栈
						{
							while (priority[opDic[top], opDic[exp[i]]] != 0)
							{
								if (op.Count == 0) break;//运算符栈空则跳出
								char c = op.Pop();
								if (op.Count > 0)
									top = op.Peek();//更新栈顶运算符
								ChooseOp(c, ref num);
							}
							op.Push(exp[i]);//最后当前运算符需要压栈
						}

					}
				}
			}

			//最后处理运算符栈里的剩余运算符
			while (op.Count != 0)
			{
				char c = op.Pop();
				ChooseOp(c, ref num);
			}

			ans = num.Pop();
			return ans;
		}


	}
}
