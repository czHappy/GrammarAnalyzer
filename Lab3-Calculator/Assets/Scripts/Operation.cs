using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Calc
{
	class Operation
	{

		public static double Add(double a, double b)
		{
			return a + b;
		}

		public static double Multi(double a, double b)
		{
			return a * b;
		}

		public static double Sub(double a, double b)
		{
			return a - b;
		}

		public static double Divide(double a, double b)
		{
			if (b == 0) return double.NaN;
			else return a / b;
		}

		public static double Pow(double a, double b)
		{
			if (double.IsInfinity((Math.Pow(a, b)))) return double.NaN;
			return Math.Pow(a, b);
		}

		public static double Sqrt(double a)
		{
			if (a >= 0) return Math.Sqrt(a);
			else return double.NaN;
		}

		public static double Mod(double a, double b)
		{
			if (b == 0) return double.NaN;
			int c = (int)(a / b);
			return a - c * b;
		}

		public static double Log(double a)
		{
			if (a <= 0) return double.NaN;
			else return Math.Log10(a);
		}
	}
}
