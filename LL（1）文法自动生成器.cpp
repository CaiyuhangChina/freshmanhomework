#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <vector>
#include <stack>
#include <queue>
using namespace std;
struct symbol;
struct non_terminal_symbol;
struct terminal_symbol;
struct production;
int num_terminal;
int cnt;
int num_non_terminal;
stack <int> symbol_stack;
queue<int> input_queue;
non_terminal_symbol *start;
vector <non_terminal_symbol> non_terminal;
vector <terminal_symbol> terminal;
vector <production> product;
int **table;
int find(vector<non_terminal_symbol> symbol_vector, string name);
int find(vector<terminal_symbol> symbol_vector, string name);
int find(vector<non_terminal_symbol> symbol_vector, non_terminal_symbol a);
int find(vector<terminal_symbol> symbol_vector, terminal_symbol a);
int find(vector<int>, int);
int find(vector<production>, production);
void init();
void match(string);
void error();
symbol &get(int);
void cal();//计算FIRST集与FOLLOW集
void print(int);
void draw();
void make_table();
void match_sentence();
void input_sentence();
void show_process();
void show_production(int n);
void show_state();
template <typename T>
T** new_Array2D(int row, int col)
{
	int size = sizeof(T);
	int point_size = sizeof(T*);
	//先申请内存，其中sizeof(T*) * row表示存放row个行指针  
	T **arr = (T **)malloc(point_size * row + size * row * col);
	if (arr != NULL)
	{
		T *head = (T*)((int)arr + point_size * row);
		for (int i = 0; i < row; ++i)
		{
			arr[i] = (T*)((int)head + i * col * size);
			for (int j = 0; j < col; ++j)
				new (&arr[i][j]) T;
		}
	}
	return (T**)arr;
}
struct symbol
{
	string name;
	bool nullable = 0;
	set<int> FIRST;
	set<int> FOLLOW;
	symbol(string name):name(name){}
	symbol() {}
};
struct non_terminal_symbol :symbol
{

	non_terminal_symbol()
	{

		cout << "输入非终结符的文法符号\n";
		cin >> name;
		cout << "若" << name << "是可空的，输入1，否则输入0\n";
		cin >> nullable;
	}
	bool operator ==(non_terminal_symbol b)
	{
		return (this->FIRST == b.FIRST) && (this->FOLLOW == b.FOLLOW) && (this->name == b.name) && (this->nullable == b.nullable);
	}
};

struct terminal_symbol :symbol
{
	static int num;
	regex r;
	terminal_symbol()
	{
		num++;
		cout << "输入终结符的文法符号" << endl;
		cin >> name;
		cout << "输入对应的正规表达式" << endl;
		string pattern;
		cin >> pattern;
		r = regex(pattern);
		FIRST.insert(-num);
	}
	terminal_symbol(string name, string rage) :symbol(name)
	{
		r = regex(rage);
	}
	bool operator ==(terminal_symbol b)
	{
		return (this->name == b.name);
	}
};
int terminal_symbol::num = 0;
terminal_symbol End("#","#");//#
struct production
{
	set<int> FIRST;
	set<int> FOLLOW;
	set<int> SELECT;
	bool nullable;//需要更新
	int left;
	vector<int> right;//不使用0，大于0表示非终结符的下标+1，小于0表示终结符的下标+1的相反数,相当于一个指针
	void update();
	production()
	{
		string sleft;
		string sright;
		cout << "输入产生式的左部" << endl;
		cin >> sleft;
		left = find(non_terminal, sleft);
		cout << "输入产生式的右部" << endl;
		while (cin >> sright)
		{
			if (sright == "$" || sright == ";")
			{
				break;
			}
			int temp = find(non_terminal, sright);
			if (temp != 0)
			{
				right.push_back(temp);
				continue;
			}
			else
			{
				temp = find(terminal, sright);
				if (temp != 0)
					right.push_back(temp);
				else cout<<"输入了错误的文法符号\n";
			}
		}
	}
	void show()
	{
		print(left);
		cout << "->";
		for (const auto c : right)
		{
			if (c != 0)
			{
				print(c);
				cout << ' ';

			}
		}
	}
	bool operator ==(production b)
	{
		return (b.left == this->left) && (b.right == this->right);
	}
};
int find(vector<non_terminal_symbol> symbol_vector, string name)
{
	for (auto i = symbol_vector.begin(); i != symbol_vector.end(); i++)
	{
		if (i->name == name)
		{
			return i - symbol_vector.begin() + 1;
		}
	}
	return 0;
}
int find(vector<terminal_symbol> symbol_vector, string name)
{
	for (auto i = symbol_vector.begin(); i != symbol_vector.end(); i++)
	{
		if (i->name == name)
		{
			return -(i - symbol_vector.begin() + 1);
		}
	}
	return 0;
}
int find(vector<non_terminal_symbol> symbol_vector, non_terminal_symbol a)
{
	for (int i = 0; i < symbol_vector.size(); i++)
		if (a == symbol_vector[i])
			return i+1;
	return 0;
}
int find(vector<terminal_symbol> symbol_vector, terminal_symbol a)
{
	for (int i = 0; i < symbol_vector.size(); i++)
		if (a == symbol_vector[i])
			return -i-1;
	return 0;
}
int find(vector<int> vec, int n)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (n == vec[i])
			return i;
}
	return -1;
}
int find(vector<production> vec, production pro)
{
	for (int i = 0; i < vec.size(); i++)
		if (vec[i] == pro)
			return i;
	return -1;
}
void init()
{
	cout << "输入非终结符个数\n";
	cin >> num_non_terminal;
	for (int i = 0; i < num_non_terminal; i++)
	{
		non_terminal.push_back(*new non_terminal_symbol);
	}
	cout << "输入终结符的个数\n";
	cin >> num_terminal;
	for (int i = 0; i < num_terminal; i++)
	{
		terminal.push_back(*new terminal_symbol);
	}
	cout << "选择初始符号\n";
	string name;
	cin >> name;
	/*for (auto c : non_terminal)
	{
		if (name == c.name)
		{
			start = &c;
			break;
		}
	}*/
	for (int i = 0; i < non_terminal.size(); i++)
	{
		if (name == non_terminal[i].name)
		{
			start = &non_terminal[i];
			break;
		}
	}
	cout << "输入产生式的个数" << endl;
	int num;
	cin >> num;
	for (int i = 0; i < num; i++)
	{
		product.push_back(*new production);
	}
	for (auto c : product)
	{
		if (!c.right.empty())
			if (c.right[0] == c.left)
			{
				cout << "含有左递归，不是LL（1）文法。程序退出\n";
				system("pause");
				exit(0);
			}
	}
	cal();
	table = new_Array2D<int>(num_non_terminal, num_terminal + 1);
	for (int i = 0; i < num_non_terminal; i++)
		for (int j = 0; j < num_terminal+1; j++)
			table[i][j] = -1;
	for (auto & c : product)
	{
		c.update();
	}
	bool LL = 1;
	for (int i = 0; i < num; i++)
	{
		for (int j = i+1; j < num; j++)
		{
			if (product[i].left == product[j].left)
			{
				auto a = product[i].SELECT;
				auto b = product[j].SELECT;
				auto c = a.size();
				a.insert(b.begin(), b.end());
				if (a.size() < c + b.size())
				{
					cout << "FIRST(";
					product[i].show();
					cout << "）与FIRST（";
					product[j].show();
					cout << ")的交集非空。所以该文法不是LL（1）文法。程序退出\n";
;					system("pause");
					exit(0);
				}
		}
		}
	}
	for (int i = 0; i < num; i++)
	{
		for (int j = i + 1; j < num; j++)
		{
			if ((product[i].right.size() != 0) && (product[j].right.size() != 0))
			{
				if(product[i].right[0]==product[j].right[0])
				{
					product[i].show();
					cout << "和";
					product[j].show();
					cout << "存在回溯，所以该文法不是LL（1）文法\n";
					LL = 0;
				}
			}
		}
	}
	if (!LL)
	{
		cout << "程序退出\n";
		system("pause");
		exit(0);
	}
	make_table();
}
void match(string m)
{
	for (const auto c : terminal)
	{
		if (regex_match(m, c.r))
		{
			input_queue.push(find(terminal, c));
			return;
		}
}
	cout << m;
}
void error()
{
	static int i = 1;
	cout << "erroe\n"<<i++;
}
symbol & get(int n)
{
	if (n > 0)
		return non_terminal[n - 1];
	if (n < 0)
		return terminal[-n - 1];
	else return End;
	// TODO: 在此处插入 return 语句
}
void print(int n)
{
	if (n > 0)
		cout << non_terminal[n - 1].name;
	else if (n < 0)
		cout << terminal[-n - 1].name;
	else cout<<"#";
}
void draw()
{
	cout << "\n-2表示产生空字\n-1表示出现错误\n";
	int count = 0;
	for (auto c : product)
	{
		cout << count++ << ":";
		c.show();
		cout << endl;
	}
	cout << "LL(1)分析表" << endl;
	cout << '\t';
	for (auto c : terminal)
		cout << c.name << '\t';
	cout << "#";
	cout << endl;
	for(int i=0;i<num_non_terminal;i++)
	{
		cout << non_terminal[i].name << '\t';
	for (int j = 0; j < num_terminal + 1; j++)
	{
		cout << table[i][j];
		cout << '\t';
	}
	cout << endl;
}
}
void make_table()
{
//	for (const auto c : product)
//	{
//		for (const auto cc : get(c.left).FIRST)
//		{
//			for (const auto r : c.right)
//			{
//				if (find(c.right, r) + 1)
//				{
//					if (table[c.left-1][-r - 1] == -1)
//						table[c.left-1][-r - 1] = find(product, c);
//					else error();
//					break;
//				}
//			}
//			if (get(cc).nullable)
//			{
//				for (const auto r : get(cc).FOLLOW)
//					if (table[c.left-1][-r - 1] == -1)
//						table[c.left-1][-r - 1] = -2;
//					else error();
//			}
//		}
//}
	for (const auto pro : product)
	{
		for (const int arr : pro.FIRST)
		{
			if (table[pro.left - 1][-arr - 1] == -1)
				table[pro.left - 1][-arr - 1] = find(product, pro);
			
		}
		if (pro.nullable == 1)
		{
			for (const int arr : pro.FOLLOW)
			{
				if (arr < 0)
				{
					if (table[pro.left - 1][-arr - 1] == -1)
						table[pro.left - 1][-arr - 1] = -2;
					
				}
				else if (arr == 0)
				{
					table[pro.left - 1][num_terminal] = -2;
				}
			}
		}
	}
}
void show_production(int n)
{if(n>=0)
	product[n].show();
else if (n == -1);
else if (n == -2);
}
void show_state()
{
	stack<int> t;
	stack<int> ts = symbol_stack;
	while (!ts.empty())
	{
		t.push(ts.top());
		ts.pop();
	}
	while (!t.empty())
	{
		print(t.top());
		t.pop();
	}
	cout << '\t';
	queue<int> tq = input_queue;
	print(tq.front());
	cout<< '\t';
	tq.pop();
	while (!tq.empty())
	{
		print(tq.front());
		tq.pop();
	}
	cout << endl;
 }
void match_sentence()
{
	symbol_stack.push(0);
	symbol_stack.push(find(non_terminal, *start));
	while (1) {
		show_state();
		int i = symbol_stack.top();
		int j = input_queue.front();
		if (i==j)
		{
			if (0==i)
			{
				cout << "分析成功\n";
				break;
			}
			else
			{
				symbol_stack.pop();
				input_queue.pop();
			}
		}
		else
		{
			if (i >= 0)
			{
				if (j != 0)
				{
					int temp = table[i - 1][-j - 1];
					if (temp == -1)
					{
						cout << "出现错误，输入的句子与文法不匹配\n";
						goto L1;
					}
					else
					{
						if (temp == -2)
						{
							symbol_stack.pop();
						}
						else
						{
							symbol_stack.pop();
							vector<int> t = product[temp].right;
							for (int i = t.size() - 1; i >= 0; i--)
								symbol_stack.push(t[i]);
						}
					}
				}
				else
				{
					int temp = table[i - 1][num_terminal];
					if (temp == -1)
					{
						cout << "出现错误，输入的句子与文法不匹配\n";
						goto L1;
					}
					else
					{
						if (temp == -2)
						{
							symbol_stack.pop();
						}
						else
						{
							symbol_stack.pop();
							vector<int> t = product[temp].right;
							for (int i = t.size() - 1; i >= 0; i--)
								symbol_stack.push(t[i]);
						}
					}
				}
			}
			else
			{ cout << "出现错误，输入的句子与文法不匹配\n";
						goto L1;}
		}
	}
L1:;
}
void input_sentence()
{
	
	cout << "输入要识别的句子,输入$结束句子，单词间用空格分词\n";
	string sentence;
	while (cin >> sentence)
	{
		if (sentence == "$")
		{
			input_queue.push(0);
			return;
		}
		else match(sentence);
	}
}
void cal()
{

start->FOLLOW.insert(0);
bool change = 1;
while (change)
{
	change = 0;
	for (auto &c : product)
	{
		bool null = 1;
		for (const auto r : c.right)
		{
			if (0 == get(r).nullable)
				null = 0;
		}
		if ((1 == null) && ((get(c.left)).nullable == 0))
		{
			get(c.left).nullable = 1;
			change = 1;
		}
}
}
change = 1;
while (change)
{
	change = 0;
	for (auto &c : product)
	{
		/*bool null = 1;
		for (const auto r : c.right)
		{
			
			if (0 == get(r).nullable)
				null = 0;
			
		}
		if ((1 == null) && (get(c.left).nullable == 0))
		{
			get(c.left).nullable = 1;
			change = 1;
		}*/
		for (int i = 0; i < c.right.size(); i++)
		{
			bool temp2 = 1;
			for (int j = 0; j < i; j++)
			{
				temp2 &= get(c.right[j]).nullable;
			}
			if (temp2)
			{
				auto &temp = get(c.left).FIRST;
				auto tem = get(c.right[i]).FIRST;
				auto old_size = temp.size();
				temp.insert(tem.cbegin(), tem.cend());
				auto new_size = temp.size();
				if (new_size != old_size)
					change = 1;
			}
			bool temp = 1;
			for (int j = i + 1; j < c.right.size(); j++)
			{

				temp &= get(c.right[j]).nullable;
				for (int j = i + 1; j < c.right.size(); j++)
				{
					bool temp2 = 1;
					for (int k = i + 1; k < j; k++)
						temp2 &= get(c.right[k]).nullable;
					if (temp2)
					{
						auto &temp = get(c.right[i]).FOLLOW;
						auto tem = get(c.right[j]).FIRST;
						auto old_size = temp.size();
						temp.insert(tem.cbegin(), tem.cend());
						auto new_size = temp.size();
						if (new_size != old_size)
							change = 1;
					}
				}
			}
			if (temp)
			{
				auto &temp = get(c.right[i]).FOLLOW;
				auto tem = get(c.left).FOLLOW;
				auto old_size = temp.size();
				temp.insert(tem.cbegin(), tem.cend());
				auto new_size = temp.size();
				if (new_size != old_size)
					change = 1;
			}
		}
	}
}

}
int main()
{
	init();
	//for (auto c : product)
	//{
	//	c.show();
	//}
	draw();
	cout << endl;
	char con[2] = "y";
	while (!(strcmp(con, "y")) || !strcmp(con, "Y"))
	{
		queue<int> temp;
		input_queue = temp;
		stack<int> ne;
		symbol_stack = ne;
		input_sentence();
		cout << "符号栈\t当前输入符号串\t输入串\n";
		match_sentence();
		cout << "若需继续识别，输入y，否则退出程序\n";
		cin >> con;
	}
}

void production::update()
{
	nullable = get(left).nullable;
	
	for (auto c : right)
	{
		auto temp = get(c).FIRST;
		FIRST.insert(temp.begin(), temp.end());
		if (get(c).nullable == 0)
			break;
	}
	for (int i = right.size() - 1; i >= 0; i--)
	{
		auto temp = get(right[i]).FOLLOW;
		FOLLOW.insert(temp.begin(), temp.end());
		if (get(right[i]).nullable == 0)
			break;
	}
	if (right.size()==0)
	{
		for (auto c : right)
		{
			if (!c)
				SELECT.insert(c);
		}
		const auto *c = &get(left).FOLLOW;
		SELECT.insert(c->begin(), c->end());
	}
	else
	{
		SELECT = FIRST;
	}
}