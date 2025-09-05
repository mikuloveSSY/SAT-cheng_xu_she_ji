#include"define.hpp"
#include"cnfparser.cpp"
#include"solver.cpp"
#include"display.cpp"
#include"X-Sudoku.cpp"

int main(){
    char file[20];
    printf("Input your file name:");
    scanf("%s",file);
    //读取cnf文件
    SAT* sat=Readcnf(file);
    if(sat==NULL){
        printf("open error\n");
        getchar();
        getchar();
        return 0;
    }
    //开始dpll
    int choose=0;
    printf("What's your strategy:");
    scanf("%d",&choose);
    Result* result=Solve(sat,choose);
    SaveResult(sat,result,choose);
    printf("\ntime:%d ms",result->time);
    //释放结果
    free(result->bl);
    free(result);
    getchar();
    getchar();
    return 0;
}

