#include"define.hpp"
#include"cnfparser.cpp"
#include"solver.cpp"
#include"display.cpp"
#include"X-Sudoku.cpp"

int main(){
    char file[20];
    scanf("%s",file);
    //读取cnf文件
    SAT* sat=Readcnf(file);
    if(sat==NULL){
        printf("open error");
        return 0;
    }
    int start=clock();
    //开始dpll
    // Reoutput(sat);
    Solve(sat);


    int end=clock();
    printf("\ntime:%d ms",end-start);
    getchar();
    getchar();
    return 0;
}

