#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <fstream>
#include <stack>
using namespace std;

struct tokens
{
	string str;
	int code;
};

class lexical
{
public:
	lexical();        //形式上给的默认构造函数，没用上
	lexical(string ss);
	~lexical(){};
	void start();
private:
	int recog_dig(char ch);  //只是返回token值
	int recog_str(char ch);
	int recog_id(char ch);
	int hand_com(char ch);
	int recog_del(char ch);

	void initype();          //创建种别码表格                   

	bool write_sym();		    //将符号表写入到文件中
	bool write_token();			//将token表写入到文件中

	bool iskeyword();			//判断当前单词是否是关键字
	bool isdelimeter();			//判断当前单词是否是界符
	bool isexist_sym();			//判断当前单词是否在sym表中，需要放进sym表的单词是：常数以及标识符

	bool ins_sym();				//将当前单词的内容以及token值插入到sym表
	bool ins_token();			//将当前单词的内容以及token值插入到token表

	bool isletter(char ch);		//判断是否是字符
	bool isdigital(char ch);	//判断是否是数字

	char get_nextchar();	//获取缓冲区下一个字符   //要如实反应所有的字符，包括空格和换行
	void get_line();		//读取一行数据到缓冲区
	int gettoken();			//获取当前字符对应的token值    //不用

	void error();			//出错信息处理

	string s;				//记录当前读取的单词

	string address;			//想要读取的文件的地址
	stack<char> huanchong;	//当行数据缓冲区
	ifstream in;			//源文件

	int tokenNum;			//记录当前读取单词对应的token值
	int row;				//记录当前行数
	int cow;				//记录当前列数

	queue<tokens> token;    //用队列存
	//	sym表;
	map<string, int> type;  //种别码表，仅存一词一码的情况
};

lexical::lexical()
{
	tokenNum = 0;
	row = 0;
	cow = 0;
	initype();
}
lexical::lexical(string ss)
{
	address = ss;
	tokenNum = 0;
	row = 0;
	cow = 0;
	initype();
}

int lexical::recog_dig(char ch)   //只是返回token值          //改过，跟书本不一样
{
	int state = 0;
	while (state != 8 && state != 9 && state != 10)
	{
		switch (state)
		{
		case 0:
			if (isdigital(ch))
			{
				state = 1;
				s += ch;
			}
			else
				error();
			break;
		case 1:
			ch = get_nextchar();
			if (isdigital(ch))
			{
				state = 1;
				s += ch;
			}
			else if (ch == 'E' || ch == 'e')
			{
				state = 4;
				s += ch;
			}
			else if (ch == '.')
			{
				state = 2;
				s += ch;
			}
			else
				state = 10;
			break;
		case 2:
			ch = get_nextchar();
			if (isdigital(ch))
			{
				state = 3;
				s += ch;
			}
			else
				error();
			break;
		case 3:
			ch = get_nextchar();
			if (isdigital(ch))
			{
				state = 3;
				s += ch;
			}
			else if (ch == 'E' || ch == 'e')
			{
				state = 4;
				s += ch;
			}
			else
			{
				state = 9;
			}
			break;
		case 4:
			ch = get_nextchar();
			if (ch == '+' || ch == '-')
			{
				state = 5;
				s += ch;
			}
			else if (isdigital(ch))
			{
				state = 6;
				s += ch;
			}
			else
				error();
			break;
		case 5:
			ch = get_nextchar();
			if (isdigital(ch))
			{
				state = 6;
				s += ch;
			}
			else
				error();
			break;
		case 6:
			ch = get_nextchar();
			if (isdigital(ch))
			{
				state = 6;
				s += ch;
			}
			else
			{
				state = 8;
			}
			break;

		}

	}

	huanchong.push(ch);   //回退
	cow--;
	if (state == 8 || state == 9)   //这里的bool型常数要怎么识别?????????????
		return 36;
	else if (state == 10)
		return 35;

}
int lexical::recog_str(char ch)         //不考虑转义字符
{
	int state = 0;
	while (state != 2)
	{
		switch (state)
		{
		case 0:
			if (ch == '\'')
				state = 1;
			else
				error();
			break;
		case 1:
			ch = get_nextchar();
			if (ch == '\'')
				state = 2;
			else
			{
				state = 1;
				s += ch;
			}
			break;
		}
	}
	//不用回退
	return 37;
}
int lexical::recog_id(char ch)
{
	char state = '0';
	while (state != '2')
	{
		switch (state)
		{
		case '0':
			if (isletter(ch))
			{
				state = '1';
				s += ch;
			}
			else
				error();
			break;
		case '1':
			ch = get_nextchar();		//有问题，万一最后为回车呢？ 已解决
			if (isletter(ch) || isdigital(ch))
			{
				state = '1';
				s += ch;
			}
			else
				state = '2';   //转到终态的话不需要将该字符加到s
			break;
		}
	}
	huanchong.push(ch);   //回退
	cow--;

	if (iskeyword() != 1)   //判断是否不是关键字
		return 34;   //标识符种别码
	return type[s];
}
int lexical::hand_com(char ch)
{
	int state = 0;
	while (state != 2 && state != 5)
	{
		switch (state)
		{
		case 0:
			if (ch == '/')
			{
				state = 1;
				s += ch;
			}
			else
				error();
			break;
		case 1:
			ch = get_nextchar();
			if (ch == '*')
			{
				state = 3;
			}
			else
			{
				state = 2;
			}
			break;
		case 3:
			ch = get_nextchar();
			if (ch == '*')
			{
				state = 4;
			}
			else
			{
				state = 3;
			}
			break;
		case 4:
			ch = get_nextchar();
			if (ch == '/')
			{
				state = 5;
			}
			else
			{
				state = 3;
			}
			break;
		}
	}
	if (state == 2)
	{
		huanchong.push(ch);
		cow--;
		return type["/"];
	}
	return 0;
}
int lexical::recog_del(char ch)
{
	int state = 0;
	while (state != 4 && state != 5)
	{
		switch (state)
		{
		case 0:
			if (ch == '<')
			{
				state = 1;
				s += ch;
			}
			else if (ch == '>')
			{
				state = 2;
				s += ch;
			}
			else if (ch == '=')
			{
				state = 3;
				s += ch;
			}
			else
			{
				state = 4;
				s += ch;
			}
			break;
		case 1:
			ch = get_nextchar();
			if (ch == '>')
			{
				state = 4;
				s += ch;
			}
			else if (ch == '=')
			{
				state = 4;
				s += ch;
			}
			else
			{
				state = 5;
			}
			break;
		case 2:
			ch = get_nextchar();
			if (ch == '=')
			{
				state = 4;
				s += ch;
			}
			else
			{
				state = 5;     //字符不确定的终态就要就要回退的；若字符确定的终态，那么获取到的字符已经进行了相应的处理，不要回退；
			}
			break;
		case 3:
			ch = get_nextchar();
			if (ch == '=')
			{
				state = 4;
				s += ch;
			}
			else
			{
				state = 5;
			}
			break;
		}
	}

	if (state == 5)
	{
		huanchong.push(ch);
		cow--;
	}

	return type[s];
}

void lexical::initype()
{
	ifstream in("Sample.txt");
	int i = 1;
	string temp;
	while (in >> temp)
	{
		type[temp] = i;
		i++;
	}
	in.close();
}

bool lexical::write_sym()		    //将符号表写入到文件中
{
	return true;
}
bool lexical::write_token()			//将token表写入到文件中
{
	ofstream out("Token.txt");
	while (token.empty() != 1)
	{
		tokens temp;
		temp = token.front();
		token.pop();
		out << "(" << temp.code << ", " << "\"" << temp.str << "\"" << ")" << endl;
	}
	out.close();
	return true;
}

bool lexical::iskeyword()   //判断当前单词是否是关键字
{
	if (type[s] >= 1 && type[s] <= 23)
		return true;
	else
		return false;
}
bool lexical::isdelimeter() //判断当前单词是否是界符
{
	if (type[s] >= 39 && type[s] <= 48)
		return true;
	else
		return false;
}
bool lexical::isexist_sym() //判断当前单词是否在sym表中，需要放进sym表的单词是：常数以及标识符
{
	return false;
}

bool lexical::isletter(char ch)
{
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		return true;
	else
		return false;
}
bool lexical::isdigital(char ch)
{
	if (ch >= '0'&&ch <= '9')
		return true;
	else
		return false;
}

bool lexical::ins_sym()	   //将当前单词的内容以及token值插入到sym表
{
	return true;
}
bool lexical::ins_token()    //将当前单词的内容以及token值插入到token表
{
	if (tokenNum != 0)
	{
		tokens temp;
		temp.str = s;
		temp.code = tokenNum;
		token.push(temp);
	}
	return true;
}

char lexical::get_nextchar()   //测试正常
{
	char ch;
	ch = huanchong.top();                //读取缓冲区中第一个单词到ch;
	//	cout << ch;
	huanchong.pop();
	cow++;
	return ch;
}
void lexical::get_line()    //测试正常
{
	string temp;
	getline(in, temp);							//读取一行数据到缓冲区中;        //test

	huanchong.push('\n');
	for (int i = temp.size() - 1; i >= 0; i--)   //这样存进去的数据结尾是没有任何东西的,上面手动加了个‘\n’
	{
		huanchong.push(temp[i]);
	}
	row++;
	cow = 0;
}
int lexical::gettoken()
{

	return 0;
}

void lexical::error()    //出错信息处理
{
	cout << row << "行" << cow << "列有词法错误";
	cin.get();
	exit(0);
}

void lexical::start()
{
	row = 0;
	cow = 0;

	cout << "正在分析，请等待...." << endl;
	//打开源文件;
	in.open(address);       //可以改

	while (in.eof() != 1)
	{
		get_line();
		char ch;
		while ((ch = get_nextchar()) != '\n')
		{
			while (ch == 32)       //将空格过滤
				ch = get_nextchar();                          //这一块代码可以整合到get_nextchar()中去
			if (ch == '\n')
				break;

			//			cout << ch;
			//入口由if语句来确定了，每种单词的首字母都不同的，所以状态转换图中的初态则不需要再写进状态转换函数中
			if (isdigital(ch))           //识别数字常量
			{
				tokenNum = recog_dig(ch);
				if (isexist_sym() != 1)  //判断是否不在符号表中
				{
					ins_sym();  //插入到符号表中
				}
			}
			else if (ch == '\'')      //识别字符常量
			{
				tokenNum = recog_str(ch);
				if (isexist_sym() != 1)  //判断是否不在符号表中
				{
					ins_sym();  //插入到符号表中
				}
			}
			else if (isletter(ch))   //识别标识符
			{
				tokenNum = recog_id(ch);
				if (iskeyword() != 1)   //判断是否不是关键字
				{
					if (isexist_sym() != 1)  //判断是否不在符号表中
					{
						ins_sym();  //插入到符号表中
					}
				}
			}
			else if (ch == '/')      //识别除号和注释
			{
				tokenNum = hand_com(ch);
			}
			else                     //识别界符
			{
				tokenNum = recog_del(ch);
				/*				if (isdelimeter() != 1)   //判断是否不在界符表中   为什么？？运算符不算吗
				{
				error();
				return;
				}*/
			}

			ins_token();          // 将识别出来的单词加入到token中
			s.clear();
		}

	}
	in.close();

	cout << "分析完成,开始写入文件" << endl;

	write_sym();
	if (write_token())
		cout << "文件写入完成" << endl;

	cout << "token文件已保存到项目文件夹下" << endl;

	//test通过
	/*
	// 	while (token.empty()!=1)
	// 	{
	// 		tokens temp;
	// 		temp = token.front();
	// 		token.pop();
	// 		cout << temp.str << " " << temp.code << endl;
	// 	}
	*/
	return;
}


#endif