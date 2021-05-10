#include "head.h"
#include "mainwindow.h"
//无实际意义，仅是为了map重载的时候不报错
bool operator<(identifier a, identifier b)
{
    return a.id < b.id;
}

std::string display_clause(clause cls)
{
    std::string out;
    for (unsigned long long i = 0; i < cls.element.size(); i++) {
        if (!cls.element[i].positive)
            out += "!";
        out += function_name[cls.element[i].function_id] + "(";
        for (unsigned long long j = 0; j < cls.element[i].element.size(); j++) {
            if (cls.element[i].element[j].type == VARIABLE)
                out += variable_name[cls.element[i].element[j].id];
            else
                out += constant_name[cls.element[i].element[j].id];
            if (j != cls.element[i].element.size() - 1)
                out += ",";
        }
        out += ")";
        if (i != cls.element.size() - 1)
            out += "|";
    }
    return out;
}

//判断原子句是否除变量和真假外全部一致
bool exp_v_pos_equal(atomic atomic1, atomic atomic2)
{
    if (atomic1.function_id == atomic2.function_id&&
        atomic1.element.size() == atomic2.element.size()) {
        for (unsigned long long i = 0; i < atomic1.element.size() && i < atomic2.element.size(); i++) {
            if (atomic1.element[i].type == VARIABLE || atomic2.element[i].type == VARIABLE)
                continue;
            if (atomic1.element[i].id != atomic2.element[i].id)
                return false;
        }
        return true;
    }
    return false;
}

//判断原子句是否除了positive全部一致
bool exp_pos_equal(atomic atomic1, atomic atomic2)
{
    if (atomic1.function_id == atomic2.function_id&&
        atomic1.element.size() == atomic2.element.size()) {
        for (unsigned long long i = 0; i < atomic1.element.size() && i < atomic2.element.size(); i++) {
            if (atomic1.element[i].id != atomic2.element[i].id||atomic1.element[i].type != atomic2.element[i].type)
                return false;
        }
        return true;
    }
    return false;
}
//判断clause1是否是clause2的孩子节点
bool is_child(int clause1, int clause2)
{
    if (clause_set[clause1].father == -1 || clause_set[clause1].mother == -1)
        return false;
    if (clause_set[clause1].father == clause2 || clause_set[clause1].mother == clause2)
        return true;
    return(is_child(clause_set[clause1].father, clause2) || is_child(clause_set[clause1].mother, clause2));
}

//fact是事实，delete_clause是被替换的子句，delete_atomic是待删除的原子句
int replaced(int fact, int replaced_clause, int replaced_atomic)
{
    clause cls;
    std::map<identifier, identifier> replace_map;//<变量，常量>

    //构建替换的键值对
    for (unsigned long long i = 0; i < clause_set[fact].element[0].element.size(); i++) {
        if (clause_set[replaced_clause].element[replaced_atomic].element[i].type == VARIABLE)
            replace_map[clause_set[replaced_clause].element[replaced_atomic].element[i]] = clause_set[fact].element[0].element[i];
    }
    cls.element = clause_set[replaced_clause].element;
    cls.father = fact;
    cls.mother = replaced_clause;
    cls.element.erase(std::vector<atomic>::iterator(cls.element.begin() + replaced_atomic));

    for (unsigned long long i = 0; i < cls.element.size(); i++) {
        for (unsigned long long j = 0; j < cls.element[i].element.size(); j++) {
            if (cls.element[i].element[j].type == VARIABLE &&
                replace_map.end() != replace_map.find(cls.element[i].element[j])) {
                //变量替换
                cls.element[i].element[j] = replace_map[cls.element[i].element[j]];
            }
        }
    }
    if (cls.element.size() == 0) {
        return error;//子句集自身是矛盾的
    }
    clause_set.push_back(cls);

    if (cls.element.size() == goal_clause.element.size() &&
        cls.element[0].positive == goal_clause.element[0].positive &&
        exp_v_pos_equal(cls.element[0], goal_clause.element[0]))
        return success;
    return fail;
}

//用clause[num]替换clause[i]
int substitude(int num)//num为进行替换的事实所在位置
{
    for (unsigned long long i = 0; i < clause_set.size(); i++) {
        if (clause_set[i].valid && !is_child(num, i)) {//可以访问且i不是num的祖结点则用num的事实替换i中的原子句
            for (unsigned long long j = 0; j < clause_set[i].element.size(); j++) {
                if (exp_v_pos_equal(clause_set[num].element[0], clause_set[i].element[j]) &&
                    clause_set[num].element[0].positive != clause_set[i].element[j].positive) {//真假相反则可删去
                    int state = replaced(num, i, j);
                    if (state == error || state == success)
                        return state;
                }
            }
        }
    }
    return fail;
}

//进行归结，返回归结得到的状态
int resolution()
{
    //归结的主体
    for (unsigned long long i = 0; i < clause_set.size(); i++) {
        if (clause_set[i].element.size() == 1) {//是事实
            //1.检查该子句是否与目标子句中的原子句相同
            if(goal_clause.element.size()==0&&exp_pos_equal(clause_set[i].element[0], goal_clause.element[0])&&
                    clause_set[i].element[0].positive!=goal_clause.element[0].positive)
                return fail;//goal_clause是单一原子句的情况
            for(unsigned long long j=0;j<goal_clause.element.size();j++){
                if(exp_v_pos_equal(clause_set[i].element[0], goal_clause.element[j])&&
                        clause_set[i].element[0].positive==goal_clause.element[j].positive){
                    clause_set[i].valid=true;
                    return success;}
            }
            //2.对其它子句进行替换，并将这一子句置为不可访问
            clause_set[i].valid = false;//valid置为否
            int state = substitude(i);
            if (state == error || state == success)
                return state;
        }
    }
    return fail;
}

//找到关键路径
void find_critical_path(clause &present_clause)
{
    present_clause.key = true;//在关键路径上
    if (present_clause.father == -1 && present_clause.mother == -1)
        return;
    find_critical_path(clause_set[present_clause.father]);
    find_critical_path(clause_set[present_clause.mother]);
}

//计算clause的字符串长度
int count_clause_len(clause present_clause)
{
    int num=0;
    for(unsigned long long i=0;i<present_clause.element.size();i++){
        if(!present_clause.element[i].positive)
            num++;
        num+=(int)function_name[present_clause.element[i].function_id].length();
        num+=2;//括号长度
        for(unsigned long long j=0;j<present_clause.element[i].element.size();j++){
            if(present_clause.element[i].element[j].type==VARIABLE){
                num+=(int)variable_name[present_clause.element[i].element[j].id].size();
            }
            else{
                num+=(int)constant_name[present_clause.element[i].element[j].id].size();
            }
            num++;//','的长度
        }
        //最后‘,’的长度应该减一，然后加一‘|’的长度，但二者抵消
    }
    num--;
    return num;
}

void display_resolution(int num,int state)
{
    //1.对输入子句集进行标号
    result += "<font color=orange>Number input clause set:</font><br>";
    for (int i = 0; i < num; i++) {
        result += QString::fromStdString(std::to_string(i+1));
        result += ":";
        result += QString::fromStdString(display_clause(clause_set[i]));
        result += "<br>";
    }
    result += "<br>";

    //2.输出归结过程，关键路径标为蓝色
    result += "<font color=orange>Start to resolve:</font><br>" ;
    //如果没有进行归结，直接从子句集中得到，则直接输出提示信息
    if(clause_set.size()==(unsigned long long)num){
        if(state==success){
            result+="<font color=blue>goal clause has been declared in clause set directly</font><br>" ;
            for(int i=0;i<num;i++){
                if(clause_set[i].key){
                    result += "<font color=blue>"+ QString::fromStdString(std::to_string(i+1)) + ":";
                    result += QString::fromStdString(display_clause(clause_set[i]))+"<br>";
                }
            }
        }
        else if(state==fail)
            result+="<font color=blue>goal clause is conflict with clause set directly</font><br>" ;
        else if(state==error)
            result+="<font color=blue>clause set is logical wrong directly</font><br>" ;
    }

    for(unsigned long long i=num;i<clause_set.size(); i++){
        if(clause_set[i].key){
            result += "<font color=blue>"+ QString::fromStdString(std::to_string(i+1)) + ":";
            result += QString::fromStdString(display_clause(clause_set[i]));
            result += QString::fromStdString(std::string(39-count_clause_len(clause_set[i])-std::to_string(i+1).length()-2-std::to_string(clause_set[i].father+1).length()-std::to_string(clause_set[i].mother+1).length(),'.'));
            result += QString::fromStdString(std::to_string(clause_set[i].father+1)+ "&" + std::to_string(clause_set[i].mother+1) +"<br></font>");
        }
        else{
            result += QString::fromStdString(std::to_string(i+1)) + ":";
            result += QString::fromStdString(display_clause(clause_set[i])) ;
            result += QString::fromStdString(std::string(39-count_clause_len(clause_set[i])-std::to_string(i+1).length()-2-std::to_string(clause_set[i].father+1).length()-std::to_string(clause_set[i].mother+1).length(),'.'));
            result += QString::fromStdString(std::to_string(clause_set[i].father+1)+ "&" + std::to_string(clause_set[i].mother+1) +"<br>");
        }
    }

    //3.输出归结结果
    result += "<br><font color=orange>Result:</font><br>" ;
    if(state==success){
        result += "<font color=red><b>successfully resolve goal clause </b></font><br>" ;
        result += "(critical path highlight in <font color=blue>blue</font>)<br>";
    }
    else if(state==fail){
        result += "<font color=red><b>can not resolve the goal clause</b></font><br>" ;
    }
    else if(state==error){
        result += "<font color=red><b>clause logically wrong</b></font><br>" ;
    }

}

