// LL1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<string>
#include<map>
#include<vector>
#include<stack>
#include<set>
#include<cstring>
#include<cmath>
#include<algorithm>
using namespace std;

map<char, int>getNum; //字符与下标的映射
char getChar[100];   //下标与字符的映射，根据下标获得对应字符
//既可以通过下标来获得字符，也可以通过字符获取它的下标位置
int num = 0;	//符号总数量
int numVT = 0;     //终结符数量，0是#，下标1—numVT是终结符，下标numVT+1 ~ num-1是非终结符
vector<string>produce;//产生式集合
int table[100][100];      //预测分析表 table[i][j]表示从i->j选用了哪一个推导式，返回下标
string first[200];//first集合，存的是下标
string follow[200];//follow集合，存的是下标
char start;//起始点字符
int vis[200][200];//每个非终结符到终结符选用的推导式唯一确定，也就是select集合不能相交
bool LL1flag = 1;//判定是否是LL1文法
bool containLeft = 0;//标记是否含有左递归
bool containConFactor;//判断产生式是否含有公因子
string sentence;//用户输入的句子，用来测试
bool mark[200];//标记哪些非终结符有左递归
bool era[200];//需要擦除的旧产生式的序号
char pool[10] = { 'V','Z','X','U','W','O','I','L','Y','J' };//字符池，供改造产生式使用
int poolCnt = 0;//字符池已经取走的字符数
//从字符池里取出一个T' 供改造产生式使用
char getPoolChar() {
	if (poolCnt >= 9) {
		cout << "字符池已空，异常退出。";
		exit(0);
	}
	return pool[poolCnt++];
}

//初始化
void init() {
	memset(era, 0, sizeof era);
	memset(mark, 0, sizeof mark);
	memset(table, -1, sizeof table);
	memset(vis, -1, sizeof vis);
	getNum['#'] = 0;
	getChar[0] = '#';
	LL1flag = 1;
	containLeft = 0;
	for (int i = 0; i < 200; i++) {
		first[i] = "";
		follow[i] = "";
	}
}
//读入数据
void getData()
{
	init();//初始化
	cout << "请输入所有的终结符：" << endl;

	char x;//待读入的字符
	do
	{
		cin >> x;
		getNum[x] = ++num;
		getChar[num] = x;
	} while (cin.peek() != '\n');//只要不是换行符就接着读入字符

	//cout <<"终结符数量 = "<<num << endl;

	//# a b @
	numVT = ++num;//最后一个是@字符，人为添上一个空@

	getNum['@'] = numVT;        //@代表空
	getChar[num] = '@';

	cout << "请输入所有非终结符：(请不要输入字符池里的字符（'V','Z','X','U','W','O','I','L','Y','J'）)" << endl;
	//读入非终结符，
	do
	{
		cin >> x;
		getNum[x] = ++num;
		getChar[num] = x;
	} while (cin.peek() != '\n');

	cout << "输入产生式集合，默认第一个字符为起始点，空用‘@’表示‘end’结束:" << endl;
	string pro;//读入产生式
	int cc = 0;
	while (cin >> pro && pro != "end")
	{	
		if (cc == 0)	start = pro[0];
		cc++;
		string ss;
		ss += pro[0];//pro[0]是产生式左部
		//除去->之后是从3开始
		for (int i = 3; i < pro.size(); i++)
		{
			//如果碰到|说明要拆分成新的产生式
			if (pro[i] == '|')
			{
				if (ss[0] == ss[1]) {
					mark[getNum[ss[0]]] = 1;//标记该非终结符左递归
					containLeft = 1;
				}
				produce.push_back(ss);
				cout << "新压入的产生式为：" << ss << endl;
				ss.clear(); 
				ss += pro[0];
			}
			else
			{

				ss += pro[i];
			}
		}
		if (ss[0] == ss[1]) {
			mark[getNum[ss[0]]] = 1;//标记该非终结符左递归
			containLeft = 1;
		}

		produce.push_back(ss);//把产生式压入产生式集合,删除了->，S->aA 就是SaA
		cout << "新压入的产生式为：" << ss << endl;
	}
}

void updateProduce() {
	//删除旧的产生式
	vector<string>prod;
	for (int i = 0; i < produce.size(); i++) {
		if (!era[i]) {
			prod.push_back(produce[i]);
		}
	}
	produce.clear();
	for (int i = 0; i < prod.size(); i++) {
		produce.push_back(prod[i]);

	}

	
}
void dealLeft() {
	int sz = produce.size();
	int tnum = num;
	for (int i = numVT+1; i <= tnum; i++) {//遍历每一个非终结符
		if (!mark[i]) continue;

		string ss = "";
		
		for (int j = 0; j < sz; j++) {//T->TB TTB
			if (produce[j][0] == getChar[i] && produce[j][0] == produce[j][1]) {
				for (int k = 1; k < produce[j].size(); k++) {
					ss += produce[j][k];
				}
				ss += '|';
				//produce.erase[j];
				era[j] = 1;
			}
			
		}
		for (int j = 0; j < sz; j++) {//T->a  Ta
			if (produce[j][0] == getChar[i] && produce[j][0] != produce[j][1]) {
				for (int k = 1; k < produce[j].size(); k++) {
					ss += produce[j][k];
				}
				ss += '|';
				//produce.erase[j];
				era[j] = 1;
			}
		}
		//   T-> 【TB1 | TB2 | a1 | b2 |】

		vector<string> split;
		int p = 0;
		string tmp = "";
		while (p<ss.size()) {
			
			if (ss[p] != '|')
				tmp += ss[p];
			else {
				split.push_back(tmp);
				tmp = "";
			}
			p++;
		}

		char X = getPoolChar();//分配的非终结符X
		getNum[X] = ++num;
		getChar[num] = X;

		for (int q = 0; q < split.size(); q++) {
			string re1 = "", re2 = "";
			re1 += getChar[i];
			re2 += X;
			if (split[q][0] != getChar[i]) {
				if(split[q] != "@")
					re1 += split[q];
				re1 += X;
				produce.push_back(re1);
			}
			else {
				for (int n = 1; n < split[q].size(); n++) {
					re2 += split[q][n];
				}
				re2 += X;
				produce.push_back(re2);
			}
		}
		//别忘了要补一个T'->@
		string r = "";
		r += X;
		r += '@';
		produce.push_back(r);
		  
	}

	updateProduce();
}
//两个字符串求并集
void unionSet(string &a, string b)  //a = a 并 b   取a,b并集赋值给a，利用了set的去重
{
	set<char>se;
	for (int i = 0; i < a.size(); i++)
		se.insert(a[i]);

	for (int i = 0; i < b.size(); i++)
		se.insert(b[i]);

	string ans;
	set<char>::iterator it;
	for (it = se.begin(); it != se.end(); it++)
		ans += *it;
	a = ans;
}

string dfsFirst(int VN, int & hasEmpty)     //dfs,看VN这个非终结字符
{
	//numVT所映射的字符是@，也就是空
	if (VN == numVT)	hasEmpty = 1;
	if (VN < numVT)	return first[VN];//如果这个“非终结符的序号小于numVT”,那么它肯定是一个终结符，直接返回
	string ans;
	//遍历所有产生式
	for (int i = 0; i < produce.size(); i++)
	{
		//如果产生式第一个字符是这个非终结符，则从这里开始dfs，
		if (getNum[produce[i][0]] == VN)//
			ans += dfsFirst(getNum[produce[i][1]], hasEmpty);
	}
	return  ans;
}

void getFirst()
{
	for (int i = 1; i <= numVT; i++)     //终结符，first集是其本身,直接加入即可。
	{
		first[i] += ('0' + i);//加上‘0’将其转化为字符
	}

	for (int j = 0; j < produce.size(); j++)    //扫描所有产生式
	{
		int k = 0; 
		int hasEmpty = 0;        
		do {
			hasEmpty = 0;
			k++;
			if (k == produce[j].size())  //推到最后一个了，则附加空字
			{
				first[getNum[produce[j][0]]] += ('0' + numVT);
				break;
			}
			//合并之
			unionSet(first[getNum[produce[j][0]]], dfsFirst(getNum[produce[j][k]], hasEmpty));
		}while (hasEmpty);  //到无法推出空字为止
	}
}

void printFirst()
{
	cout << "First集:" << endl;
	for (int i = 1; i <= num; i++)
	{
		cout << "First [" << getChar[i] << "]: ";
		for (int j = 0; j < first[i].size(); j++)
			cout << getChar[first[i][j] - '0'] << " ";
		cout << endl;
	}
	cout << endl;
}

void getFollow()
{
	unionSet(follow[getNum[start]], "0");  //起始点follow集合添加‘#’‘#'对应的下标是0；

	for (int i = 0; i < produce.size(); i++)       //扫描所有产生式
	{
		for (int j = 1; j < produce[i].size(); j++)   //每个非终结符的follow集
		{
			if (getNum[produce[i][j]] <= numVT) continue;  //终结字符VT无follow集

			int k = j;
			int hasEmpty;
			do
			{
				hasEmpty = 0;
				k++;
				if (k == produce[i].size())   //T->...D的结构，T的follow集 要加入D的follow集，
				{
					unionSet(follow[getNum[produce[i][j]]], follow[getNum[produce[i][0]]]);
					break;
				}
				//只要这个非终结符的first集合包含空
				//那么就要继续找这个非终结符的下一个符号的first集合，直到遍历完这个产生式的所有字符
				//注意follow集合不含空
				unionSet(follow[getNum[produce[i][j]]] , dfsFirst(getNum[produce[i][k]], hasEmpty));
			}while (hasEmpty);
		}
	}

	//第一遍扩充之后第二遍才是完整的follow集
	for (int i = 0; i < produce.size(); i++)       
	{
		for (int j = 1; j < produce[i].size(); j++)   
		{
			if (getNum[produce[i][j]] <= numVT) continue;  

			int k = j;
			int hasEmpty;
			do
			{
				hasEmpty = 0;
				k++;
				if (k == produce[i].size()) 
				{
					unionSet(follow[getNum[produce[i][j]]], follow[getNum[produce[i][0]]]);
					break;
				}
				unionSet(follow[getNum[produce[i][j]]], dfsFirst(getNum[produce[i][k]], hasEmpty));
			} while (hasEmpty);
		}
	}
}

void getPredictTable()          //获得预测分析表
{
	for (int i = 0; i < produce.size(); i++)   //扫描所有产生式
	{
		if (produce[i][1] == '@')     //直接推出空的，把产生式左部的follow集合加入
		{
			//B→εselect(B→ε) = Follow(B)={ c,d,e }
			string f = follow[getNum[produce[i][0]]];
			for (int k = 0; k < f.size(); k++)
			{
				if (vis[getNum[produce[i][0]] ][f[k] - '0'] != -1 && vis[getNum[produce[i][0]]][f[k] - '0'] != i){
					LL1flag = 0;
					return;
				}
				//为非终结符和终结符建立映射关系，映射关键字是产生式的下标
				table[getNum[produce[i][0]]][f[k] - '0'] = i;

				vis[getNum[produce[i][0]]][f[k] - '0'] = i;
			}
			//没必要再往下走
			continue;
		}

		string temps = first[getNum[produce[i][1]]];

		for (int j = 0; j < temps.size(); j++)               //A->BT考察B的first集first（B） = {a,b,c,d}
		{
			if (temps[j] != ('0' + numVT))//如果不是空就加进来
			{
				if (vis[getNum[produce[i][0]]][temps[j] - '0']!=-1 && vis[getNum[produce[i][0]]][temps[j] - '0'] != i) {
					LL1flag = 0;
					return;
				}
				table[getNum[produce[i][0]]][temps[j] - '0'] = i;

				vis[getNum[produce[i][0]]][temps[j] - '0'] = i;
			}
			else //有空字的，考察follow集
			{
				/*********************************/
				//string flw = follow[getNum[produce[i][1]]];
				string flw = follow[getNum[produce[i][0]]];

				for (int k = 0; k < flw.size(); k++)
				{
					if (vis[getNum[produce[i][0]]][flw[k] - '0'] != -1 && vis[getNum[produce[i][0]]][flw[k] - '0'] != i) {
						LL1flag = 0;
						return;
					}
					table[getNum[produce[i][0]]][flw[k] - '0'] = i;
					vis[getNum[produce[i][0]]][flw[k] - '0'] = i;
				}
			}
		}
	}
}
string getProce(int i)  //由对应下标获得对应产生式。
{
	if (i < 0)return " ";    //无该产生式
	string ans;
	ans += produce[i][0];
	ans += "->";
	//ans+=(proce[i][0]+"->");  注意这样不行！思之即可。
	for (int j = 1; j < produce[i].size(); j++)
		ans += produce[i][j];
	return ans;
}
void printTable()
{
	cout << "预测分析表：" << endl;
	for (int i = 0; i < numVT; i++)
		cout << '\t' << getChar[i];
	cout << endl;
	for (int i = numVT + 1; i <= num; i++)
	{
		cout << getChar[i];
		for (int j = 0; j < numVT; j++)
		{
			cout << '\t' << getProce(table[i][j]);
		}
		cout << endl;
	}
	cout << endl;
}
void printFollow()
{
	cout << "follow集：" << endl;
	for (int i = numVT + 1; i <= num; i++)
	{
		cout << "follow [" << getChar[i] << "]: ";
		for (int j = 0; j < follow[i].size(); j++)
			cout << getChar[follow[i][j] - '0'] << " ";
		cout << endl;
	}
	cout << endl;
}

bool analyzeStack()       //分析给定句子的合法性，并打印判定过程。
{
	//分析栈
	stack<char>sta;
	sta.push('#'); //首先push#进去垫底
	sta.push(start);//起始点进栈
	cout << "start = " << start << endl;
	int i = 0;

	while (!sta.empty())
	{
		int cur = sta.top();
		sta.pop();
		if (cur == sentence[i] )       //是终结符，消去，查看下一个字符
		{
			i++;
		}
		else  if (cur == '#' && i == sentence.size()) //只有这一种可能可以成功，那就是栈空，句子遍历完结尾，结束
		{
			return 1;
		}
		else  if (table[getNum[cur]][getNum[sentence[i]]] != -1) //查表
		{

			int k = table[getNum[cur]][getNum[sentence[i]]];

			cout << produce[k][0] << "->";

			//打印选用的产生式
			for (int j = 1; j < produce[k].size(); j++)
				cout << produce[k][j];

			cout << endl;

			for (int j = produce[k].size() - 1; j > 0; j--)  //逆序入栈
			{
				if (produce[k][j] != '@')//空不需要入栈
					sta.push(produce[k][j]);
			}
		}
		else      //失败！
		{
			return 0;
		}
	}
	return 1;
}

bool hasConF(string a , string b) {
	bool has = 0;
	int sz = min(a.size(), b.size());
	for (int i = 1; i < sz; i++) {
		if (a[i] == b[i])	return 1;
	}
	return 0;
}
bool containConF() {
	
	for (int i = 0; i < produce.size(); i++) {
		for (int j = i+1; j < produce.size(); j++) {
			if (produce[i][0] == produce[j][0]) {
				if (hasConF(produce[i], produce[j]))	return true;
			}
		}
	}
	return false;
}
int getConF(string a, string b) {
	//从第1个开始不是第0个开始
	int sz = min(a.size(), b.size());
	for (int i = 1; i < sz; i++) {
		if (a[i] != b[i])	return i;
	}
	return sz;
}
void printProduce() {
	for (int i = 0; i < produce.size(); i++) {
		string t = "";
		t += produce[i][0];
		t += "->";
		for (int j = 1; j < produce[i].size(); j++) {
			t += produce[i][j];
		}
		cout << t << endl;
	}
	
}
void dealConFactor() {
	memset(era, 0, sizeof era);
	//int sz = produce.size(); 
	for (int i = 0; i < produce.size(); i++) {//这个size是不断更新的
		for (int j = i+1; j < produce.size(); j++) {
			if (produce[i][0] == produce[j][0]) {//是同一个非终结符推导出来的产生式
				if (hasConF(produce[i], produce[j]) && !era[i] &&!era[j] ) {
					era[i] = era[j] = 1;//标记这两个产生式应该被擦除
					int border = getConF(produce[i], produce[j]);//公因式边界
					char X = getPoolChar();//分配一个新的非终结符
					getNum[X] = ++num;//建立索引
					getChar[num] = X;
					//将有三个新的产生式进入产生式集合
					string tmp1 = "",tmp2 = "";
					tmp1 += X , tmp2 += X;
					for (int k = border; k < produce[i].size(); k++) {
						tmp1 += produce[i][k];
					}
					for (int k = border; k < produce[j].size(); k++) {
						tmp2 += produce[j][k];
					}
					if (tmp1.size() == 1)	tmp1 += '@';
					if (tmp2.size() == 1)	tmp2 += '@';
					produce.push_back(tmp1);
					produce.push_back(tmp2);
					string tmp3 = "";
					tmp3 += produce[i][0];
					//注意从1开始，产生式左部非终结符不算
					for (int k = 1; k < border; k++) {
						tmp3 += produce[i][k];
					}
					tmp3 += X;
					produce.push_back(tmp3);
				}
			}
		}
	}
	updateProduce();//擦除旧的产生式
}

int main()
{
	getData();

	if (containLeft) {
		cout << "该文法包含左递归，正在进行左递归消除..." << endl;
		dealLeft();
		//打印最终的产生式
		cout << "消除左递归后的新产生式集合：" << endl;
		printProduce();
	}
	containConFactor = containConF();
	if (containConFactor) {
		cout << "该文法某些产生式包含公因子，正在提取公因子..." << endl;
		dealConFactor();
		cout << "提取公因子后的新产生式集合：" << endl;
		printProduce();
	}
	getFirst();
	getFollow();
	//获取预测分析表的同时可以知道是否是LL1文法
	getPredictTable();
	cout << "num = " << num << " numVT = " << numVT << endl;
	

	if (!LL1flag) {
		cout << "\n该文法不是LL1文法！经过消除左递归和提取公因子仍然不能使其变成LL(1)文法\n" << endl;
		return 0;
	}
	else cout << "\n该文法是LL1文法！\n" << endl;
	printFirst();
	printFollow();
	printTable();
	cout << "请输入句子：" << endl;
	cin >> sentence;
	if (analyzeStack())
		cout << "Accepted! 推导过程如上。" << endl;
	else   cout << "Error!" << endl;

	return 0;
}
