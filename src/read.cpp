#include "head.h"

//通过函数名查找其在function_name中的id,并用bool返回其是否找到
bool find_func_id(std::string present_function_name, int&id)
{
    std::vector<std::string>::iterator result = find(function_name.begin(), function_name.end(), present_function_name);
    if (result != function_name.end()) {
        id = result - function_name.begin();
        return true;
    }
    return false;
}

//通过标识符的名称返回一个identifier,并用bool返回其是否找到
bool find_identifier_id(std::string present_identifier_name, identifier& present_identifier)
{
    //找constant
    std::vector<std::string>::iterator result = find(constant_name.begin(), constant_name.end(), present_identifier_name); //查找
    if (result != constant_name.end()) { //找到
        present_identifier.id = result - constant_name.begin();
        present_identifier.type = CONSTANT;
        return true;
    }

    //找variable
    result = find(variable_name.begin(), variable_name.end(), present_identifier_name); //查找
    if (result != variable_name.end()) { //找到
        present_identifier.id = result - variable_name.begin();
        present_identifier.type = VARIABLE;
        return true;
    }

    return false;
}

//在已知标识符不在identifier的情况下，加入identifier中，用bool返回加入是否成功
bool add_identifier_id(std::string present_identifier_name, identifier& present_identifier)
{
    bool type=!!islower(present_identifier_name[0]);
    for(int i=1;i<(int)present_identifier_name.size();i++){
        if(type!=!!islower(present_identifier_name[i]))
            return false;
    }
    if(type==true){
        variable_name.push_back(present_identifier_name);
        present_identifier.id = (int)variable_name.size()-1;
        present_identifier.type = VARIABLE;
    }
    else{
        constant_name.push_back(present_identifier_name);
        present_identifier.id = (int)constant_name.size()-1;
        present_identifier.type = CONSTANT;
    }
    return true;

}

//读入一个原子句
bool read_one_atomic(clause& present_clause, std::string atomic_str)
{
    int identifier_begin=0, identifier_end, i;

    atomic present_atomic;
    int point_begin=0;
    int point_end=(int)atomic_str.size();

    //1.判断是否为非
    if (atomic_str[point_begin] == '!') {
        present_atomic.positive = false;
        point_begin++;
    }

    //2.分离函数名
    std::string present_function_name;
    for (i = point_begin; i < point_end; i++) {
        if (atomic_str[i] == '(') {
            present_function_name = atomic_str.substr(point_begin, i - point_begin);
            identifier_begin = i + 1;
            break;
        }
    }
    if (i == point_end)//不存在函数变量
        return false;

    //通过函数名查找其在function_name中的id
    int id;
    if (!find_func_id(present_function_name, id)) { //没找到,新加入
        function_name.push_back(present_function_name);
        present_atomic.function_id = (int)function_name.size() - 1;
    }
    else  //找到
        present_atomic.function_id = id;

    //3.分离每一个标识符
    for (i = identifier_begin; i < point_end; i++) {
        if (atomic_str[i] == ','|| atomic_str[i] == ')') {
            identifier_end = i;
            identifier present_identifier;
            //通过这个标识符的名称返回一个identifier
            if (!find_identifier_id(atomic_str.substr(identifier_begin, identifier_end - identifier_begin), present_identifier)){
                //没找到,新加入
               if(!add_identifier_id(atomic_str.substr(identifier_begin, identifier_end - identifier_begin), present_identifier))
                   return false;
            }
            present_atomic.element.push_back(present_identifier);
            identifier_begin = i + 1;
        }
    }
    present_clause.element.push_back(present_atomic);

    return true;
}

bool read_clause(std::string x)
{
    int i=0;
    std::string present_atomic_str;
    while (x[i] != '#') {

       clause present_clause;

       while(true){//对原子句的处理
           if (x[i] != '\n'&&x[i]!='|')
                present_atomic_str+=x[i];
           else{
               if(present_atomic_str.empty()&&!clause_set.empty())
                   ;
               else if (!read_one_atomic(present_clause, present_atomic_str))
                    return false;
               else
                   present_atomic_str.clear();
           }
           if(x[i]=='\n'){
               if(!present_clause.element.empty())
                   clause_set.push_back(present_clause);
               i++;
               break;
           }
           i++;
       }

    }
    return true;
}

bool read_dst(std::string x)
{
    std::string  present_atomic_str;

    for (int i = 0; x[i] != '#'; i++) {
        if (x[i] != '|'&&x[i] != '\n')
             present_atomic_str.push_back(x[i]);
        else {
            if( present_atomic_str.empty())
                continue;
            if (!read_one_atomic(goal_clause,  present_atomic_str))
                return false;
             present_atomic_str.clear();
        }
    }
    if(goal_clause.element.empty())
        return false;
    return true;
}
