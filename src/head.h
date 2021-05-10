#pragma once
#ifndef HEAD_H
#define HEAD_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <QString>

std::vector<std::string> constant_name;//声明用于存储所有常量名的集合
std::vector<std::string> variable_name;//声明用于存储所有变量名的集合
std::vector<std::string> function_name;//声明用于存储所有函数名的集合

//标识符的定义
enum identifier_type {CONSTANT,VARIABLE};
class identifier
{
public:
    identifier_type type;//常量还是变量
    int id;//vector中的标号
};

//原子式的定义
class atomic{
public:
    std::vector<identifier> element;//按顺序存储所有的标识符
    int function_id;//原子式在function_name的下标
    bool positive=true;//原子式前面有非，则为false，否则为true
};

//子句的定义（element的大小为1则可以判定为为事实）
class clause {
public:
    std::vector<atomic> element;//按顺序存储所有以或连接的原子句
    int mother=-1;//来源1，没有则置为-1
    int father=-1;//来源2，没有则置为-1
    bool valid=true;//子句集是否可访问
    bool key=false;//是否是关键子句
};

std::vector<clause> clause_set;//声明存储所有的子句的集合
clause goal_clause;//声明预期得到的结论
QString result;//textbrowser内的字符串
enum state{ success,fail,error};//归结得到的结果状态
#endif // HEAD_H
