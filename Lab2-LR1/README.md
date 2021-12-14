# 编译原理LR1分析器实现

## 一、基本架构
### 1.数据结构
-字符映射
~~~
vector<char> v;
map<char, int > getNum;
int vt, n;//0到vt-1是终结符  vt到n-1是非终结符
~~~
- 产生式 
~~~
struct pro
{
	char left;
	string right;
	pro() { right = ""; }
};
vector<pro> produce;
~~~
- 项目集
~~~
struct item
{
	int idx;//产生式编号
	int pos;//小点位置
	string search;//向前搜索符
	item() { pos = 0; search = ""; };
};
vector< vector<item> > family;//项目集族
~~~
-动作
~~~
struct action {
	int type;//类型
	int value;//值 -1 err 1 s  2 r  3 acc
	action() { type = -1; value = 0; };
	void clear() { type = -1; value = 0; };
}table[maxn][maxn];
~~~
-描述DFA
~~~
//链式前向星基本操作，边能记录终点便于构造分析表

struct node {
	int next;
	int to;
	int w;
	node() {};
}edge[100];
int head[100];
int ne ;    //边的个数
//表示从状态from到状态to经过的边是w，w映射到字符
void addEdge(int from, int to, int w) {
	edge[ne].to = to;
	edge[ne].w = w;
	edge[ne].next = head[from];
	head[from] = ne++;
}
~~~
---
## 二、基本步骤
1. 获取产生式
2. 建立字符映射
3. 求first集合，为向前搜索符做准备
4. - BFS构建项目集族，从I0开始不断扩展得到的临时项目集
   - 临时项目集做闭包操作
   - 判断临时项目集是否是新项目集
   - 加边建图，描述项目集之间的跳转关系
5.  构造LR1分析表
6.  判断是否是LR1文法
6.  构建分析栈分析用户输入的句子
 

---
## 三、核心方法
 - #### 求first集合（只考虑直接左递归）
~~~
//可以处理直接左递归的first集
string dfsFirst(char x, int & hasEmpty, int index)     //dfs,看VN这个非终结字符
{
	string ans = "";
	//numVT所映射的字符是@，也就是空
	if (x == '@')	hasEmpty = 1;
	if (isVT(x)) {
		string t = "";
		t += x;


		return t;//如果这个“非终结符的序号小于VT”,那么它肯定是一个终结符，直接返回
	}
	//遍历所有产生式
	for (int i = 0; i < produce.size(); i++)
	{
		//if (i == index) continue;
		//如果产生式第一个字符是这个非终结符，则从这里开始dfs
		if (produce[i].left == x)//
		{
			if (i == index || produce[i].right[0] == x) continue;
			ans += dfsFirst(produce[i].right[0] , hasEmpty, i);
		}
	}
	return  ans;
}
//dfs求first集
void getFirst()
{
	for (int i = 0; i < vt; i++)     //终结符，first集是其本身,直接加入即可。
	{
		first[i] += v[i];//
	}

	for (int j = 0; j < produce.size(); j++)    //扫描所有产生式
	{
		int k = 0;
		int hasEmpty = 0;
		do {
			hasEmpty = 0;
			
			if (k == produce[j].right.size())  //推到最后一个了，则附加空
			{
				string tmp = "@";
				unionSet(first[getNum[produce[j].left]], tmp);
				break;
			}
			unionSet(first[getNum[produce[j].left]], dfsFirst(produce[j].right[k], hasEmpty, j));
			k++;
		} while (hasEmpty);  //直到无法推出空为止
	}
}
~~~

 - #### bfs求项目集闭包
~~~
//求项目的闭包得到项目集
vector<item> getClosure(item t) {
	vector<item> tmp;
	tmp.push_back(t);

	queue<item> q;
	q.push(t);

	while (!q.empty()) {
		item fr = q.front();
		q.pop();
		int pos = fr.pos;//该项目小点位置
		if (pos == produce[fr.idx].right.size())//如果是规约项,S->A. ,
			continue;

		//如果是可移进项S->A.Ba , 
		char nxt = produce[fr.idx].right[pos];//小数点指向的符号
		if (isVT(nxt)) continue;//若小点后跟终结符则不扩展,S->A.aB ,

		vector<int> pros = getHisPro(nxt);//否则引入该非终结符名下所有产生式 S->A.Ba ,a/b
		for (int i = 0; i < pros.size(); i++) {
			item t;
			t.idx = pros[i];
			t.pos = 0;
			t.search = "";
			if(produce[fr.idx].right.size()-fr.pos == 1)  //S->A.D  ,a/b
				unionSet(t.search, fr.search);//搜索符直接并入

			else if (hasEmpty(first[getNum[produce[fr.idx].right[pos + 1]]]))// S->A.DFa,  a/b
			{
				unionSet(t.search, fr.search);//搜索符并入
				unionSet(t.search, first[getNum[produce[fr.idx].right[pos + 1]]]);
			}
			else{
				unionSet(t.search, first[getNum[produce[fr.idx].right[pos + 1]]]);
			}

			eraseEmpty(t.search);//除去空串

			if (!containItem(tmp, t)) {
				tmp.push_back(t);
				q.push(t);
			}	
		}
	}
	return tmp;
}
~~~

 - #### bfs求项目集族
~~~
- //求项目集族
void getFamily() {
	//初始I0 入项目集族
	vector<item> st;
	item Q;
	Q.idx = 0;
	Q.pos = 0;
	Q.search += '#'; //#入第一个item
	st = getClosure(Q);
	queue< vector<item> > q;
	q.push(st);
	family.push_back(st);
	while (!q.empty()) {
		vector<item> fr = q.front();
		q.pop();
		//对每一个item进行分析  引入新的项目集
		//枚举边
		for (int i = 2; i < n; i++) { //0 - @  1 - #    枚举边
			vector<item> next;
			for (int j = 0; j < fr.size(); j++) { //枚举每一个item

				if (fr[j].pos == produce[fr[j].idx].right.size()) {//如果是规约项，就不引入新的项目集
					continue;
				}
				else {//如果是移进项，查看移进符号是否等于当前枚举的边
					int pos = fr[j].pos;
					if (produce[fr[j].idx].right[pos] == v[i]) {
						//把所有小点指向这个边（符号）的item全部加入到新的项目集中
						item tmp;
						tmp.idx = fr[j].idx;
						tmp.pos = fr[j].pos + 1;
						tmp.search = fr[j].search;
						if (!containItem(next, tmp)) {
							next = unionItemSet(next, getClosure(tmp));
						}
					}
				}
			}
			if (next.size() == 0) continue;//空的不引进
			//新项目集在项目集族中的位置  若没有，则是新的，需要插入建边，若有则是老的，建边
			int newPos = findItemSet(family, next);
			int pre = findItemSet(family, fr);
			//cout << newPos << " && " << pre << endl;
			if (newPos == -1) { //如果是新的项目集
				family.push_back(next);
				q.push(next);
				addEdge(pre, family.size()-1, i);
			}
			else { //老项目
				addEdge(pre, newPos, i);
			}
		}
	}
}
~~~



 - #### 获取分析表

~~~void getTable() {

	//链式前向星图描述的是移进关系，不考虑向前搜索符号，把这件事情推给规约时考虑 两者不能有一点冲突
	for (int i = 0; i < family.size(); i++) {
		for (int j = head[i]; j != -1; j = edge[j].next) {
			if (table[i][edge[j].w].type == -1) { // table[i][j] 从状态i到字符j的动作  是唯一的

				table[i][edge[j].w].value = edge[j].to;
				table[i][edge[j].w].type = 1;//-1 错误  1移进   2规约  3 acc
			}
			else {
				LR1 = false;
				return;
			}
		}
	}
	//接下来找 规约项
	for (int i = 0; i < family.size(); i++) {//对每一个项目集合
		for (int j = 0; j < family[i].size(); j++) { //对于项目集合中每一个item
			
			item cur = family[i][j];
			if (cur.pos == produce[cur.idx].right.size()) { //是规约态
				if (cur.idx == 0) { //如果是S'->S. , #
					if (table[i][1].type != -1) {
						LR1 = false;
						return;
					}
					table[i][1].type = 3;
				}
				else {
					for (int k = 0; k < cur.search.size(); k++) {   //按照其向前搜索符号进行规约
						int x = getNum[cur.search[k]];
						if (table[i][x].type != -1) {
							LR1 = false;
							return;
						}
						table[i][x].type = 2; //规约项
						table[i][x].value = cur.idx; //按照第idx个产生式规约
					}
				}
				
			}
		}
	}
}
~~~

 - #### 分析句子
~~~
//分析过程
bool analysis() {
	cout << "please input a sentence : " << endl;
	cin >> input ;
	string g = "";
	for (int i = 0; i < 25; i++) g += " ";
	cout << "步骤\t" << "状态栈" <<g<< "符号栈" << g << "输入串" << g << "动作" << g << endl;

	for(int i = 0; i < input.size(); i++) {
		if (!in(input[i])) {
			cout << "含有除文法之外的字符，错误！" << endl;
			return 0;
		}
	}
	input += "#";
	stack<int> state;
	stack<char> str;
	state.push(0);
	str.push('#');
	int cnt = 0;//步骤；
	int i = 0;//字符串的指针

	while (true) {
		cnt++;
		int curState = state.top();
		int curChar = getNum[input[i]];
		int type = table[curState][curChar].type;
		cout << cnt << "\t";
		string st = printStack(state);

		string sr = printStack(str);

		string left = getLeft(input, i);
		
		if (type == -1) { //err
			cout <<  st <<  sr <<  left << "Error!";
			return 0;
		}
		else if (type == 3) { //acc
			cout <<  st <<  sr <<  left << "Accepted!";
			return 1;
		}
		else if (type == 1) { //移进操作
			cout <<  st <<  sr <<  left << "s" << table[curState][curChar].value;
			state.push(table[curState][curChar].value);
			str.push(input[i]);
			i++;
		}
		else if(type == 2) //规约操作
		{
			int id = table[curState][curChar].value;
			int len = produce[id].right.length();
			while (len--) { //规约几个就弹出几个，符号栈与状态栈共进退
				str.pop();
				state.pop();
			}
			str.push(produce[id].left);
			int go = table[state.top()][getNum[str.top()]].value;
			state.push(go);
			cout <<  st <<  sr <<  left << "r" << id<<"   goto" << go ;
		}
		cout << endl;
	}
}
~~~
---
## 四、注意事项
1. 文法拓广，防止多起点
2. 空串在项目集中不要显示 A->.ε  直接A->.否则会造成归约错误
3. 构造分析表时，谨记从一个状态遇到一个字符只可能有一种动作，如果这个项前后被赋予不同的值，必然不是LR1文法，避免对冲突事项求交集。


---
### 五、运行结果
- 测试数据
```
(1) P158练习 15

S
S->a|^|(T)
T->S,T|S
end
(a,^)

(2) P156练习 1
A
A->aAd|aAb|@
end


(3) P147教材例子
S
S->BB
B->aB
B->b
end
aabb
```