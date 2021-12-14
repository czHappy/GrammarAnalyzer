using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Calc
{
	public class Calculator : MonoBehaviour
	{
		public InputField textField;
		//输入
		public void InputToTextField(string c)
		{
			textField.text = textField.text + c;
		}
		//CE
		public void CE()
		{
			textField.text = "";
		}
		//Delete
		public void Delete()
		{
			textField.text = textField.text.Remove(textField.text.Length - 1, 1);
		}
		//equals
		public void Equals()
		{
			Calc cal = new Calc();
			Debug.Log(textField.text);
			double result = cal.Calculate(textField.text);
			textField.text = result.ToString();
		}
	}
}
