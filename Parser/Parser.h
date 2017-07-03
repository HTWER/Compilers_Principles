#ifndef __PARSER_H__
#define __PARSER_H__

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <sstream>

using namespace std;
/*注意事项*/
/*token串源文件的结尾不能够有任何的回车符号*/

class Parsing
{
public:
	Parsing();
	Parsing(string address);
	~Parsing(){};
	void start();
private:
	//	vector<sym> excel[][49];
	//用于存储预测分析表，其中的每一个元素都是代表着一串符号的sym串
	//由于源文件全部都是终结符，都对应着唯一的token编码，所以
	//第二维用于标识关键字，直接用token码来访问,从1开始存的；
	//第一维用于标识非终结符，编码同样是用int来指示；

	map<string, map<string, vector<string> > > excel;  //存放预测分析表
	stack<string> symstack;		                       //当前符号栈
	string X;                       //用于存储当前栈顶的元素
	string LA;                      //用于存储上一次从原文件中读取到的A
	string A;						//用于存储当前中源文件读取到的元素
	ifstream inOfSour;              //用于记录需要分析的token文件

	void initExcel();               //用于初始化预测分析表
	bool isVT();					//判断是否终结符,直接对内部X操作
	void readX();                   //将symstack符号栈顶的元素读取到X
	void readA();					//将输入文件的下一个token读取到A中
	void checkExcel();              //查表,如果表中的那个vector是空的，则查表失败直接退出
	void error();
};

Parsing::Parsing()
{
	initExcel();
	inOfSour.open("Source.txt");
}

Parsing::Parsing(string address)
{
	initExcel();
	inOfSour.open(address);
}

void Parsing::initExcel()    //通过读取文件的方式来创建     //test pass
{
	string temp;
	string temp1;
	string temp2;
	string temp3;
	stringstream ss;
	ifstream in("PAT.txt");

	while (in.eof() != 1)
	{
		getline(in, temp);  //读取一行数据
		ss.str(temp);      //创建数据流
		ss >> temp1;
		ss >> temp2;
		while (ss.eof() != 1)    //若ss不空      //test
		{
			ss >> temp3;
			ss.get();
			excel[temp1][temp2].push_back(temp3);
		}
		ss.clear();   //ss用 >> 来说入数据的时候并不是从ss中抽取出来，仅仅是读取，只有用了get（）之后才会读取并抽取出来
	}
}

bool Parsing::isVT()       //用来判断X是不是终结符
{
	ifstream in("Sample.txt");
	string temp;
	while (in.eof() != 1)
	{
		in >> temp;
		if (X == temp)
			return true;
	}
	in.close();
	return false;
}

void Parsing::readX()
{
	X = symstack.top();
	symstack.pop();
}
void Parsing::readA()    //test pass
{
	LA = A;
	if (inOfSour.eof() == 1)   //就好像文件尾有个“#”一样
		A = "#";
	else
	{
		inOfSour.get();
		int code;
		inOfSour >> code;
		if (code == 34)    //对那几个特殊字符进行处理
		{
			A = "0idtype";
			inOfSour.get();   //吸收掉 ，号
			string GB;
			inOfSour >> GB;   //吸收掉剩余段
		}
		else if (code == 35)
		{
			A = "0itype";
			inOfSour.get();
			string GB;
			inOfSour >> GB;
		}
		else if (code == 36)
		{
			A = "0ftype";
			inOfSour.get();
			string GB;
			inOfSour >> GB;
		}
		else if (code == 37)
		{
			A = "0ctype";
			inOfSour.get();
			string GB;
			inOfSour >> GB;
		}
		else if (code == 38)
		{
			A = "0btype";
			inOfSour.get();
			string GB;
			inOfSour >> GB;
		}
		else
		{
			inOfSour.get();
			inOfSour.get();
			string temp;
			inOfSour >> temp;
			A = temp.substr(1, temp.size() - 3);
		}
		inOfSour.get();  //不要忘记最后的换行符也要吸收掉
	}

}

void Parsing::checkExcel()
{
	string s;
	if (excel[X][A].empty() == 1)
		error();
	else
	{
		vector<string> temp = excel[X][A];
		while (temp.empty() != 1)  //当前表格有内容
		{
			s = temp.back();
			temp.pop_back();
			if (s == "#")      //表格中的#代表着 姨婆洗咯
				return;
			else
				symstack.push(s);
		}
	}
}

void Parsing::error()
{
	if (LA.empty() != 1)
		cout << "语法错误：\n" << "当前字符 " << A << " 不可以直接放在 " << LA << " 之后" << endl;
	else
		cout << "语法错误：\n" << "当前字符 " << A << " 不可以放在源文件开头位置" << endl;
	cin.get();
	exit(0);
}

void Parsing::start()
{
	symstack.push("#");
	//	symstack.push("PROGRAM");    //改改这个，只是分析算术表达式
	symstack.push("E");
	readA();

	while (1)
	{
		readX();
		if (isVT())
		{
			if (X == A)
				readA();
			else
				error();
		}
		else
		{
			if (X == "#")    //这里一旦成立，则说明symstack空了；
			{
				if (X == A)   //这时去检查输入文件是不是已经空了,这个条件可以更改为               ///
				{
					cout << "分析结束，语法正确！" << endl;   //分析结束，成功
					break;
				}
				else
					error();
			}
			else
				checkExcel();    //查完表就要读一个X出来
		}
	}
}


#endif