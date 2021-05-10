#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "head.h"
#include "read.cpp"
#include "solution.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UsageClicked()
{
    QMessageBox::about(this, tr("Usage"), "type the clause in left box,then click <b>Go</b> button <br>  "
                          "you will see resolution process and result in the right box<br>"
                          "<b>Remember</b>:lower alpha represent variable,upper alpha represent constant<br>"
                          "              input each clause with a newline,and not input empty line<br>"
                          "'|'represent \"or\" <br>"
                          "other characters are not allowed in this program.<br><br>"
                          "take a problem for example:<br>"
                          "Input clause set:<br>"
                          "Kill(C,A)|Kill(B,A)|Kill(A,A)<br>"
                          "Hate(x,A)|!Kill(x,A)<br>"
                          "!Hate(A,x)|!Hate(C,x)<br>"
                          "Hate(A,A)<br>"
                          "Hate(A,C)<br>"
                          "Rich(x,A)|Hate(B,x)<br>"
                          "!Rich(x,A)|!Hate(B,x)<br>"
                          "Hate(A,x)|!Hate(B,x)<br>"
                          "!Hate(A,x)|Hate(B,x)<br>"
                          "!Hate(A,A)|!Hate(A,B)|!Hate(A,C)<br>"
                          "!Hate(B,A)|!Hate(B,B)|!Hate(B,C)<br>"
                          "!Hate(C,A)|!Hate(C,B)|!Hate(C,C)<br>"
                          "!Rich(x,A)|!Kill(x,A)<br>"
                          "Input goal clause:<br>"
                          "Kill(A,A)");
}

void MainWindow::MoreClicked()
{
    QMessageBox::about(this, tr("More"), "if you want to know more    <br>visit  "
                          "<a href='https://github.com/Iris-Song/clause-resolution'>my Github</a>  "     );
}

void MainWindow::init()
{
    constant_name.clear();
    variable_name.clear();
    function_name.clear();
    clause_set.clear();
    goal_clause.element.clear();
    result.clear();
    ui->textBrowser->clear();

}

void MainWindow::on_GoButton_clicked()
{
    init();

    //  获取输入框的内容；
    QString content = ui->textEdit->toPlainText();
    std::string content_char = content.toLatin1().data();

    QString content2 = ui->textEdit_2->toPlainText();
    std::string content_char2 = content2.toLatin1().data();



    if(content_char.empty()){
        QMessageBox::critical(this, tr("error"), tr("input clause set is empty"));
        return;
    }
    if(content_char2.empty()){
        QMessageBox::critical(this, tr("error"), tr("input goal clause is empty"));
        return;
    }

    content_char+="\n#";
    content_char2+="\n#";

    if (!read_clause(content_char))
    {
        QMessageBox::critical(this, tr("error"), tr("input clause set is incorrectly formed<br>"
                                                    "click TOOL->usage for more information"));
        return;
    }
    if (!read_dst(content_char2)){
        QMessageBox::critical(this, tr("error"), tr("input goal clause is incorrectly formed<br>"
                                                    "click TOOL->usage for more information"));
        return;
    }
    /*for(int i=0;i<constant_name.size();i++){
        reschar += constant_name[i] ;
        reschar += " " ;
    }
    for(int i=0;i<variable_name.size();i++){
        reschar += variable_name[i] ;
        reschar += " " ;
    }
    for(int i=0;i<function_name.size();i++){
        reschar += function_name[i] ;
        reschar += " " ;
    }
    for(int i=0;i<clause_set.size();i++){
        reschar += clause_out(clause_set[i]) ;
        reschar += "\n" ;
    }
    reschar += clause_out(result_clause) ;
    reschar += "\n" ;*/

    int origin_num=(int)clause_set.size();
    int state=resolution();
    if (state == success)
        find_critical_path(clause_set[clause_set.size()-1]);

     display_resolution(origin_num,state);

    ui->textBrowser->append(result);

}
