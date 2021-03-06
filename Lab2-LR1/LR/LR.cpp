// LR.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<stack>
#include<set>
#include<cstring>
#include<queue>
using namespace std;

//建立字符和下标的映射
map<char, int>getNum;
char getChar[100];   
char start;//文法的起始符号
vector<string>produce;  //产生式集合 
string VNRight[100];   //该非终结符名下的所有产生式编号
string first[100];   //first集
int table[100][100];  //  分析表 值     
int SR[100][100];     //  分析表 类型
//从0-VT 终结符  从VT-num非终结符
int num = 0; 
int VT = 0;  
struct item         //项目集中的一个项目
{
	int idx;       //产生式编号
	int pos;        //小点.的位置
	string fSet;   //逗号后面的集合，A->α.Bβ ，a   b∈first(βa) 则 B->.γ , b ∈ CLOSURE(I)
};
vector< vector<item> > family;     //LR（1）项目集规范族

//链式前向星基本操作，能记录边的终点，便于构建分析表
struct node {
	int next;
	int to;
	int w;
}edge[100];
int head[100];//head[i]表示以i
int ne = 0;    //边的个数
//加边
void addEdge(int from, int to, int w){
	edge[ne].to = to;
	edge[ne].w = w;
	edge[ne].next = head[from];
	head[from] = ne++;
}

string input;//接收用户输入，需要分析的句子

//从产生式提取终结符和非终结符，利用set去重
set<char> upper;
set<char> lower;
string ps = "\t\t";//空格符
void getV() {
	for (int i = 0; i < produce.size(); i++) {
		for (int j = 0; j < produce[i].size(); j++) {
			if (produce[i][j] >= 'A' && produce[i][j] <= 'Z') {
				upper.insert(produce[i][j]);//非终结符插入
			}
			else {
				if (produce[i][j] == '@') continue;//空暂时不插入
				lower.insert(produce[i][j]);
			}
		}
	}
	//处理终结符
	getNum['#'] = 0;
	getChar[0] = '#';
	int k = 0;
	set<char>::iterator it;
	for (it = lower.begin(); it != lower.end(); it++) {
		getNum[*it] = ++k;
		getChar[k] = *it;
	}
	VT = ++k;
	getNum['@'] = VT;
	getChar[VT] = '@';

	for (it = upper.begin(); it != upper.end(); it++) {
		getNum[*it] = ++k;
		getChar[k] = *it;
	}
	num = k;
}

//初始化全局变量
void init() {
	for (int i = 0; i < 100; i++)
		head[i] = -1;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
			SR[i][j] = table[i][j] = -1;
	ne = 0;
	memset(table, -1, sizeof(table));
	
}

void grammarExpend() {
	cout << "输入起始符(Q除外): " << endl;
	cin >> start;
	string s = "Q";
	s += start;
	produce.push_back(s);//文法拓广
}
//获取非终结符名下的所有产生式编号
void getVNRight()
{
	for (int i = 0; i < produce.size(); i++)
	{
		int temp = getNum[produce[i][0]];
		VNRight[temp] += char('0' + i);
	}
}
void getData()                  
{
	cout << "输入所有产生式（空用‘@’表示,输入end结束）" << endl;
	string pro;
	while (cin >> pro && pro != "end")
	{
		string ss;
		ss += pro[0];
		for (int i = 3; i < pro.size(); i++)
		{
			//含有|则分割
			if (pro[i] == '|')
			{
				produce.push_back(ss);
				ss.clear(); ss += pro[0];
			}
			else
			{
				ss += pro[i];
			}
		}
		produce.push_back(ss);
	}

	getV();//处理终结符和非终结符
	getVNRight();//
}

bool vis[100]; //是否已经完成FIRST集合

bool unionSet(string &a, string b)  //a = a 并 b   取a,b并集赋值给a，利用了set的去重
{
	int sz = a.size();
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
	if (a.size() == sz)	return false;
	else return true;
}

//可以处理直接左递归的first集
string dfsFirst(int VN, int & hasEmpty, int index)     //dfs,看VN这个非终结字符
{
	//numVT所映射的字符是@，也就是空
	if (VN == VT)	hasEmpty = 1;
	if (VN < VT)	return first[VN];//如果这个“非终结符的序号小于VT”,那么它肯定是一个终结符，直接返回
	string ans;
	//遍历所有产生式
	for (int i = 0; i < produce.size(); i++)
	{
		//if (i == index) continue;
		//如果产生式第一个字符是这个非终结符，则从这里开始dfs
		if (getNum[produce[i][0]] == VN)//
		{
			if (i == index || VN == getNum[produce[i][1]]) continue;
			ans += dfsFirst(getNum[produce[i][1]], hasEmpty, i);
		}
	}
	return  ans;
}
void getFirst()
{
	for (int i = 1; i <= VT; i++)     //终结符，first集是其本身,直接加入即可。
	{
		first[i] += ('0' + i);//加上‘0’将其转化为字符
	}

	for (int j = 0; j < produce.size(); j++)    //扫描所有产生式
	{
		if (produce[j][0] == produce[j][1]) continue;
		int k = 0;
		int hasEmpty = 0;
		do {
			hasEmpty = 0;
			k++;
			if (k == produce[j].size())  //推到最后一个了，则附加空
			{
				string tmp = "";
				tmp += '0' + VT;
				unionSet(first[getNum[produce[j][0]]], tmp);
				break;
			}
			unionSet(first[getNum[produce[j][0]]], dfsFirst(getNum[produce[j][k]], hasEmpty, j));
		} while (hasEmpty);  //直到无法推出空为止
	}
}
//处理A->.@，使这种产生式变成 A->.
void dealEmpty() {
	for (int i = 0; i < produce.size(); i++) {
		if (produce[i][1] == '@') {
			char left = produce[i][0];
			produce[i] = "";
			produce[i] += left;
		}
	}
}


bool itemEqual(item a, item b)
{
	//两个项目所有元素全部相等才算相等
	if (a.fSet == b.fSet && a.pos == b.pos && a.idx == b.idx) return 1;
	return 0;
}


bool itemContain(item a, vector<item> b)      //判断项目是否在项目集中
{
	for (int i = 0; i < b.size(); i++)
	{
		if (itemEqual(a, b[i]))return 1;
	}
	return 0;
}


vector<item>  unionSet(vector<item>a, vector<item>b)  //合并项目集 a,b到a
{
	for (int i = 0; i < b.size(); i++)
	{
		if (itemContain(b[i], a))continue;
		else
			a.push_back(b[i]);
	}
	return a;
}


bool itemSetEqual(vector<item> a, vector<item> b)  //两个项目集是否相等
{
	if (a.size() != b.size())return 0;
	for (int i = 0; i < a.size(); i++)
	{
		if (!itemContain(a[i], b))return 0;
	}
	return 1;
}

int itemSetContain(vector<item>a , vector<vector<item> >b)  //查找项目集，若有，则返回在项目集族中的编号
{
	for (int i = 0; i < b.size(); i++)
	{
		if (itemSetEqual(a, b[i]))	return i;
	}
	return -1;
}

bool containEmpty(string s) {
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == VT) return 1;
	}
	return 0;
}

vector<item> getClosure(item t)           //对项目 T作闭包
{
	vector<item> temp; //新建一个项目集
	temp.push_back(t);//首项目入队
	queue<item> q;                         //bfs
	q.push(t);
	while (!q.empty())
	{
		item cur = q.front();
		q.pop();
		if (cur.pos == produce[cur.idx].size())   //归约项不必再导出产生式
			continue;
		int flw = getNum[produce[cur.idx][cur.pos]];       //获取小点.之后的符号
		if (flw <= VT)   continue;                  //若是终结符则不必再找

		for (int i = 0; i < VNRight[flw].size(); i++)         //否则引入该产生式名下的所有产生式编号
		{
			item c;
			c.pos = 1;  //起始小点位置都是1                             //
			c.idx = VNRight[flw][i] - '0';   //所属产生式编号
			if (produce[cur.idx].size() - cur.pos == 1)   //  A->BC.D,a/b first(a/b) ={ a,b}
				c.fSet += cur.fSet;
			else                           //A->B.CFb,a/b
			{
				int nxt = getNum[produce[cur.idx][cur.pos + 1]];
				c.fSet += first[nxt];//需要考虑first[nxt]是否含有空
				if (containEmpty(first[nxt]))	c.fSet += cur.fSet;
			}
			if (!itemContain(c, temp))           //新的项目加入项目集合。
			{
				q.push(c);
				temp.push_back(c);
			}
		}
	}
	return temp;
}
void getFamily()             //获得项目集族
{
	vector<item>temp;
	item t;
	t.idx = 0; 
	t.pos = 1;
	t.fSet += '0';    //初始的项目集：  ，#
	temp = getClosure(t);
	queue<vector<item> >q;        //bfs
	q.push(temp);

	family.push_back(temp);
	while (!q.empty())
	{
		vector<item> cur = q.front();
		q.pop();
		for (int i = 1; i <= num; i++)     //除了空@外所有符号
		{
			if (i == VT)continue;      //'#'
			vector<item> temp;
			for (int j = 0; j < cur.size(); j++)     //该项目集中的所有项目
			{
				if (cur[j].pos == produce[cur[j].idx].size())continue;  //规约项目无法再引入
				int nxt = getNum[produce[cur[j].idx][cur[j].pos]];  //不是规约项目可引入新的项目集
				if (nxt == i)                //如果点后面的符号就是i,那么就把它所代表的产生式加入新建项目集
				{
					item tempt;
					tempt.fSet = cur[j].fSet;
					tempt.pos = cur[j].pos + 1;
					tempt.idx = cur[j].idx;
					temp = unionSet(temp, getClosure(tempt));//作闭包后加入临时项目集
				}
			}
			if (temp.size() == 0)	continue;             //该符号无法引入新的项目集

			int numCur = itemSetContain(cur, family);   //当前节点标号
			int numNew = itemSetContain(temp, family);  //新目标标号

			if (numNew == -1)                    //不存在即是新的项目集
			{
				family.push_back(temp);//新项目集插入
				q.push(temp);//新项目集入队继续扩展其他项目集
				addEdge(numCur, family.size() - 1, i);   //添加边，权为读入的符号
			}
			else                             //存在则是老的项目集
			{
				addEdge(numCur, numNew, i);
			}
		}
	}
}


void printFamily()              //打印项目集族
{
	for (int i = 0; i < family.size(); i++)
	{
		cout << "I" << i << ":" << endl;
		for (int j = 0; j < family[i].size(); j++)
		{
			cout << produce[family[i][j].idx] << " " << family[i][j].pos << " " << family[i][j].fSet << endl;
		}
		cout << endl;
	}
	for (int i = 0; i < family.size(); i++)
	{
		for (int j = head[i]; j != -1; j = edge[j].next)
		{
			cout << "  " << getChar[edge[j].w] << endl;
			cout << i << "--->" << edge[j].to << endl;
		}
	}
}


//每个项目集（状态机当前状态）遇到一个符号只有一种动作，如果除了一种动作之外还能有其他动作，那么就存在冲突

bool getTable()            //获得分析表table[i][j]=w:状态i-->j,读入符号W。
{
	for (int i = 0; i < family.size(); i++)          //遍历图
	{
		for (int j = head[i]; j != -1; j = edge[j].next)
		{
			if (table[i][edge[j].w] != -1)  return false;           //不是确定的路径，肯定不是LR(1)文法
			table[i][edge[j].w] = edge[j].to;	//存储表格值
			SR[i][edge[j].w] = -1;             //s1
		}
	}
	for (int i = 0; i < family.size(); i++)             //遍历项目集规范族中所有项目集
	{
		for (int j = 0; j < family[i].size(); j++)   //item  
		{
			if (family[i][j].pos == produce[family[i][j].idx].size())  //归约项
			{
				for (int k = 0; k < family[i][j].fSet.size(); k++)//规约项中所有向前搜索符
				{
					if (table[i][family[i][j].fSet[k] - '0'] != -1)return 0;           //不是确定的路径，肯定不是LR(1)文法
					if (family[i][j].fSet[k] == '0'&&family[i][j].idx == 0) //规约态下的第一个产生式Q->S. 遇到#是接受态
						table[i][(family[i][j].fSet)[k] - '0'] = -3;          //acc
					else
					{
						table[i][(family[i][j].fSet)[k] - '0'] = family[i][j].idx;
						SR[i][(family[i][j].fSet)[k] - '0'] = -2;            //r1
					}
				}
			}
		}
	}
	return 1;
}
void printTable()
{
	string placeholder1 , placeholder2 = "";
	for (int i = 0; i < VT / 2; i++)	placeholder1 += "\t";
	for(int i=0; i<(num - VT)/2; i++)	placeholder2 += "\t";
	cout << "LR(1)分析表：" << endl;
	cout << "状态   " <<placeholder1<<"actoin " << placeholder1 << placeholder2<<"Goto       \n"<< endl;
	for (int j = 0; j <= num; j++)
	{
		if (j == VT||getChar[j] == 'Q') continue; //空不需要
		cout << "\t" << getChar[j];
	}
	cout << endl;
	for (int i = 0; i < family.size(); i++)
	{
		cout << i << "\t";
		for (int j = 0; j <= num; j++)
		{
			if (j == VT || getChar[j] == 'Q') continue;
			if (table[i][j] == -3)  cout << "acc" << "\t";       //接受
			else if (table[i][j] == -1)cout << "\t";        //error
			else if (SR[i][j] == -1) {
				if (j <= VT)
					cout << "s";
				cout<< table[i][j] << "\t";  //移进
			}
			else if (SR[i][j] == -2)cout << "r" << table[i][j] << "\t";  //归约
		}
		cout << endl;
	}
}

void  printCurState(int count, stack<int>state, stack<int>wd, int i)
{
	
	cout << count << ps;
	stack<int>temp;
	while (!state.empty())
	{
		temp.push(state.top());
		state.pop();
	}
	while (!temp.empty())
	{
		cout << temp.top();
		temp.pop();
	}
	cout << ps;
	while (!wd.empty())
	{
		temp.push(wd.top());
		wd.pop();
	}
	while (!temp.empty())
	{
		cout << getChar[temp.top()];
		temp.pop();
	}
	cout <<ps;
	for (int j = i; j < input.size(); j++)
		cout << input[j];
	cout <<ps;
}

bool analyze()
{
	cout << "       " << input << "的分析过程：" << endl;
	cout << "步骤" +ps<< "状态栈" + ps << "符号栈" + ps << "输入串" + ps << "动作\n" << endl;
	stack<int>state;   //俩个栈：状态栈和符号栈
	stack<int>wd;

	int count = 0;		//步骤计数器
	state.push(0);     //初始化状态栈，0入栈
	wd.push(0);        //初始化字符栈，'#'入栈
	int i = 0;//输入串读到的位置
	while(true)       //
	{
		int cur = state.top();//当前状态
		if (table[cur][getNum[input[i]]] == -1)    // 空白，报错误
			return 0;
		if (table[cur][getNum[input[i]]] == -3)  //接受态
		{
			printCurState(count++, state, wd, i);
			cout << " Accepted" << endl;
			return 1;
		}
		if (SR[cur][getNum[input[i]]] == -1)       //移进项
		{
			printCurState(count++, state, wd, i);//打印该步骤的栈信息以及剩余输入串
			int nextAction = table[cur][getNum[input[i]]];//
			
			if (getNum[input[i]] <= VT) //如果是非终结符则是s1,否则是goto
				cout << "s";
			cout << nextAction << endl;

			wd.push(getNum[input[i]]);//移进输入串的第i位，
			state.push(nextAction);//
			i++;
		}
		else if (SR[cur][getNum[input[i]]] == -2)         //归约
		{
			printCurState(count++, state, wd, i);//打印该步骤的栈信息以及剩余输入串
			int idxP = table[cur][getNum[input[i]]];   //用该产生式归约
			int len = produce[idxP].size() - 1;
			for (int j = 0; j < len; j++)                 //弹栈，该产生式右部有多少个字符就弹出几个字符，空不算
			{
				wd.pop(); //状态栈与符号栈共进退
				state.pop();
			}
			wd.push(getNum[produce[idxP][0]]);    //新入栈规约符
			cout << "r" << table[cur][getNum[input[i]]] << ",\t"<<"goto "<<table[state.top()][getNum[ produce[idxP][0] ] ]<<endl ;

			int cur = state.top();//GOTO到新状态
			state.push(table[cur][getNum[produce[idxP][0]]]);
		}
	}
	return 1;
}
int main()
{

	init();//初始化
	grammarExpend();//文法拓广
	getData();//获取产生式
	getFirst();//求first集合
	dealEmpty();//处理空串
	getFamily();//获取项目集族
	printFamily();//打印项目集族
	if (!getTable())//获取分析表并判断是否是LR（1）文法
	{
		cout << "该文法不是LR(1)文法" << endl;
		return 0;
	}
	printTable();//打印分析表
	cout << "请输入待分析的句子：" << endl;//分析句子
	cin >> input;
	input += '#';
	if (!analyze()) cout << "error!" << endl;
	return 0;
}
