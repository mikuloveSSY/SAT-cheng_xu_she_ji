#include"define.hpp"

// 创建数独谜题
char* CreateSudoku(int choose){
    if(choose){
        static char* sudoku_file="Sudoku(Cre).txt";
        FILE* fp=fopen(sudoku_file,"w");
        if(fp==NULL) return NULL;
        // 生成一个简单的数独模板
        fprintf(fp, "0 0 3 0 2 0 6 0 0\n");
        fprintf(fp, "9 0 0 3 0 5 0 0 1\n");
        fprintf(fp, "0 0 1 8 0 6 4 0 0\n");
        fprintf(fp, "0 0 8 1 0 2 9 0 0\n");
        fprintf(fp, "7 0 0 0 0 0 0 0 8\n");
        fprintf(fp, "0 0 6 7 0 8 2 0 0\n");
        fprintf(fp, "0 0 2 6 0 9 5 0 0\n");
        fprintf(fp, "8 0 0 2 0 3 0 0 9\n");
        fprintf(fp, "0 0 5 0 1 0 3 0 0\n");
        fclose(fp);
    }else{

    }
}

//将数独问题转换为cnf格式,返回值是生成的cnf文件
char* Transformcnf(char* file){
    static char cnf_file[20] = "sudoku.cnf";
    FILE* sudoku_fp = fopen(file, "r");
    if(sudoku_fp == NULL) return NULL;

    FILE* cnf_fp = fopen(cnf_file, "w");
    if(cnf_fp == NULL) {
        fclose(sudoku_fp);
        return NULL;
    }
    // 数独有9x9=81个格子，每个格子有9种可能，共729个变量
    int clauses = 0;
    int grid[9][9];
    
    // 读取数独文件
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            fscanf(sudoku_fp, "%d", &grid[i][j]);
        }
    }
    fclose(sudoku_fp);
    //通过基本的约束条件,排列组合计算子句数量
    //1.每个格子至少有一个数字: 81条子句
    clauses += 81;
    //2.每个格子最多有一个数字: 81*C(9,2)=81*36=2916条子句
    clauses += 2916;
    //3.每行每个数字只能出现一次: 9*9*C(9,2)=729*36=26244条子句
    clauses += 26244;
    //4.每列每个数字只能出现一次: 同上26244条子句
    clauses += 26244;
    //5.每个3x3宫每个数字只能出现一次: 9*9*C(9,2)=729*36=26244条子句
    clauses += 26244;
    //6.两条对角线每个数字只能出现一次: 2*9*C(9,2)=2*324=648条子句
    clauses += 648;
    //特殊情况约束条件:数独中已填数字的数量
    int known = 0;
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(grid[i][j] != 0) known++;
        }
    }
    clauses += known; 
    //写入CNF文件头
    fprintf(cnf_fp, "p cnf 729 %d\n", clauses);
/*
变元按序编码:(i-1)*81+(j-1)*9+k
每个格子都会产生9个变元,所以对于某一个,前面有(i-1)*81+(j-1)*9个,再加上其在当前格子9个变元里的序号,就是其编号
*/
    //先写已知条件,便于dpll求解
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(grid[i][j] != 0){
                fprintf(cnf_fp, "%d 0\n", i*81 + j*9 + grid[i][j]);
            }
        }
    }
    //1.每个格子至少有一个数字
    for(int i=1; i<=9; i++){
        for(int j=1; j<=9; j++){
            for(int k=1; k<=9; k++){
                fprintf(cnf_fp, "%d ", (i-1)*81 + (j-1)*9 + k);
            }
            fprintf(cnf_fp, "0\n");
        }
    }
    //2.每个格子最多有一个数字
    for(int i=1; i<=9; i++){
        for(int j=1; j<=9; j++){
            for(int k=1; k<=8; k++){
                for(int l=k+1; l<=9; l++){
                    fprintf(cnf_fp, "-%d -%d 0\n", (i-1)*81 + (j-1)*9 + k,(i-1)*81 + (j-1)*9 + l);
                }
            }
        }
    }
    //3.每行每个数字只能出现一次
    for(int i=1; i<=9; i++){
        for(int k=1; k<=9; k++){
            for(int j=1; j<=8; j++){
                for(int l=j+1; l<=9; l++){
                    fprintf(cnf_fp, "-%d -%d 0\n", 
                           (i-1)*81 + (j-1)*9 + k,
                           (i-1)*81 + (l-1)*9 + k);
                }
            }
        }
    }
    //4.每列每个数字只能出现一次
    for(int j=1; j<=9; j++){
        for(int k=1; k<=9; k++){
            for(int i=1; i<=8; i++){
                for(int l=i+1; l<=9; l++){
                    fprintf(cnf_fp, "-%d -%d 0\n", 
                           (i-1)*81 + (j-1)*9 + k,
                           (l-1)*81 + (j-1)*9 + k);
                }
            }
        }
    }
    //5.每个3x3宫每个数字只能出现一次
    for(int boxRow=0; boxRow<3; boxRow++){
        for(int boxCol=0; boxCol<3; boxCol++){
            for(int k=1; k<=9; k++){
                for(int i=1; i<=3; i++){
                    for(int j=1; j<=3; j++){
                        int cell1_i = boxRow*3 + i;
                        int cell1_j = boxCol*3 + j;
                        for(int m=1; m<=3; m++){
                            for(int n=1; n<=3; n++){
                                int cell2_i = boxRow*3 + m;
                                int cell2_j = boxCol*3 + n;
                                if(cell1_i != cell2_i || cell1_j != cell2_j){
                                    fprintf(cnf_fp, "-%d -%d 0\n", 
                                           (cell1_i-1)*81 + (cell1_j-1)*9 + k,
                                           (cell2_i-1)*81 + (cell2_j-1)*9 + k);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //6.两条对角线每个数字只能出现一次
    // 主对角线 (i = j)
    for(int k=1; k<=9; k++){
        for(int i=1; i<=8; i++){
            for(int j=i+1; j<=9; j++){
                fprintf(cnf_fp, "-%d -%d 0\n", 
                       (i-1)*81 + (i-1)*9 + k,
                       (j-1)*81 + (j-1)*9 + k);
            }
        }
    }
    
    //副对角线 (i + j = 10)
    for(int k=1; k<=9; k++){
        for(int i=1; i<=8; i++){
            int j1 = 10 - i;
            for(int m=i+1; m<=9; m++){
                int j2 = 10 - m;
                fprintf(cnf_fp, "-%d -%d 0\n", 
                       (i-1)*81 + (j1-1)*9 + k,
                       (m-1)*81 + (j2-1)*9 + k);
            }
        }
    }
    
    
    fclose(cnf_fp);
    return cnf_file;
}

//解决数独问题
int SolveSudoku(int choose){
    char* sudoku_file=CreateSudoku(choose);
    if(sudoku_file==NULL){
        printf("create failed!\n");
        return 0;
    }
    char* cnf_file=Transformcnf(sudoku_file);
    if(cnf_file==NULL){
        printf("transform failed!\n");
        return 0;
    }
    
    //读取CNF文件
    SAT* sat = Readcnf(cnf_file);
    if(sat == NULL) {
        printf("Failed to read CNF file\n");
        return 0;
    }
    
    //解决SAT问题
    Result* result = Solve(sat, choose);
    if(result == NULL) {
        printf("Failed to solve SAT problem\n");
        Clear(sat->Chead);
        free(sat);
        return 0;
    }
    
    //输出结果
    if(result->ans == 1){
        printf("Sudoku solution found:\n");
        int grid[9][9] = {0};
        
        //解析SAT解为数独网格
        for(int i=1; i<=729; i++){
            if(result->bl[i] > 0){
                int var = i;
                int row = (var-1) / 81 + 1;
                int col = ((var-1) % 81) / 9 + 1;
                int num = (var-1) % 9 + 1;
                grid[row-1][col-1] = num;
            }
        }
        
        //打印数独解
        for(int i=0; i<9; i++){
            for(int j=0; j<9; j++){
                printf("%d ", grid[i][j]);
                if((j+1) % 3 == 0 && j < 8) printf("| ");
            }
            printf("\n");
            if((i+1) % 3 == 0 && i < 8) printf("------+-------+------\n");
        }
    }else{
        printf("no solution  for sudoku .\n");
    }
    printf("time:%dms\n", result->time);

    //清理内存
    free(result->bl);
    free(result);
    Clear(sat->Chead);
    free(sat);    
    return 1;
}