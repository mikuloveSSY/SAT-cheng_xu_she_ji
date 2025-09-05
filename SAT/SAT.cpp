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
        getchar();
        getchar();
        return 0;
    }
    //开始dpll
    int* bl=Solve(sat);


    printf("\ntime:%d ms",bl[0]);
    free(bl);
    getchar();
    getchar();
    return 0;
}

