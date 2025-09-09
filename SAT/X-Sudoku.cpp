#include"define.hpp"

// 创建数独谜题
char* CreateSudoku(int choose){
    static char sudoku_file[20];
    if(choose){
        strcpy(sudoku_file,"Sudoku(Cre).txt");
        FILE* fp=fopen(sudoku_file,"w");
        if(fp==NULL) return NULL;
        // 生成一个简单的数独模板
        // fprintf(fp, "0 0 3 0 2 0 6 0 0\n");
        // fprintf(fp, "9 0 0 3 0 5 0 0 1\n");
        // fprintf(fp, "0 0 1 8 0 6 4 0 0\n");
        // fprintf(fp, "0 0 8 1 0 2 9 0 0\n");
        // fprintf(fp, "7 0 0 0 0 0 0 0 8\n");
        // fprintf(fp, "0 0 6 7 0 8 2 0 0\n");
        // fprintf(fp, "0 0 2 6 0 9 5 0 0\n");
        // fprintf(fp, "8 0 0 2 0 3 0 0 9\n");
        // fprintf(fp, "0 0 5 0 1 0 3 0 0\n");
        fprintf(fp,"..3.2.6..9..3.5..1..18.64....81.29..7.......8..67.82....26.95..8..2.3..9..5.1.3..\n");
        fclose(fp);
    }else{
        strcpy(sudoku_file,"Sudoku.txt");
    }
    return sudoku_file;
}

//将数独问题转换为cnf格式,返回值是生成的cnf文件
char* Transformcnf(char* file){
    static char cnf_file[20] = "Sudoku.cnf";
    FILE* sudoku_fp = fopen(file, "r");
    if(sudoku_fp == NULL) return NULL;

    FILE* cnf_fp = fopen(cnf_file, "w");
    if(cnf_fp == NULL) {
        fclose(sudoku_fp);
        return NULL;
    }
/*
变元按序编码:(i-1)*81+(j-1)*9+k
数独有9x9=81个格子，每个格子有9种可能，共729个变量
每个格子都会产生9个变元,所以对于某一个,前面有(i-1)*81+(j-1)*9个,再加上其在当前格子9个变元里的序号,就是其编号
*/
    //读取数独文件,记录已知位置
    int known[81]={0};
    int known_cnt = 0;
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            char sudoku_char=fgetc(sudoku_fp);
            if(sudoku_char!='.'){
                known[known_cnt++]=i*81+j*9+sudoku_char-'0';
            }
        }
    }
    fclose(sudoku_fp);

    int clauses = 0;
//通过基本的约束条件(由于数独的特殊性,约束条件大部分都是二者关系,即两两约束),排列组合计算子句数量
    //1.每个格子数字只能填1~9,即9个变元必有一真:81*1=81条
    clauses += 81;
    //2.每个格子最多有一个数字,即9个变元间两两互斥:81*C(9,2)=81*36=2916条
    clauses += 2916;
//以下情况本质类似:格子两两组合,每个组合里都有9个相同变元不同时出现的限制,所以是9*C(9,2)
    //3.每行每个数字只能出现一次:9*9*C(9,2)=81*36=2916条
    clauses += 2916;
    //4.每列每个数字只能出现一次:同 2916条
    clauses += 2916;
    //5.每个3x3宫每个数字只能出现一次:9*9*C(9,2)=81*36=2916条
    clauses += 2916;
    //6.一条对角线每个数字只能出现一次:1*9*C(9,2)=324条
    clauses += 324;
    //7.百分号两个圆构成的九宫格:2*9*C(9,2)=648条
    clauses+=648;
    //特殊情况约束条件:数独中已填数字的数量
    clauses += known_cnt; 
    //写入CNF文件头
    fprintf(cnf_fp, "p cnf 729 %d\n", clauses);

    //先写已知条件,便于dpll求解
    for(int t=0;t<known_cnt;t++){
        fprintf(cnf_fp,"%d 0\n",known[t]);
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
/*
将二者互斥关系转换成真假满足条件
*/
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
    //5.每个3x3九宫格每个数字只能出现一次
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
    //6.对角线 (i + j = 10)
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
    //7.百分号的两个圆
    for (int k = 1; k <= 9; k++){
        for (int i = 1; i <= 3; i++){
            for (int j = 1; j <= 3; j++){
                int cell1_i = 1 + i;
                int cell1_j = 1 + j;
                for (int m = 1; m <= 3; m++){
                    for (int n = 1; n <= 3; n++){
                        int cell2_i = 1 + m;
                        int cell2_j = 1 + n;
                        if (cell1_i != cell2_i || cell1_j != cell2_j){
                            fprintf(cnf_fp, "-%d -%d 0\n",
                                (cell1_i-1)*81+(cell1_j-1)*9+k,
                                (cell2_i-1)*81+(cell2_j-1)*9+k);
                        }
                    }
                }
            }
        }
    }
    for (int k = 1; k <= 9; k++){
        for (int i = 1; i <= 3; i++){
            for (int j = 1; j <= 3; j++){
                int cell1_i = 5 + i;
                int cell1_j = 5 + j;
                for (int m = 1; m <= 3; m++){
                    for (int n = 1; n <= 3; n++){
                        int cell2_i = 5 + m;
                        int cell2_j = 5 + n;
                        if (cell1_i != cell2_i || cell1_j != cell2_j){
                            fprintf(cnf_fp, "-%d -%d 0\n",
                                (cell1_i-1)*81+(cell1_j-1)*9+k,
                                (cell2_i-1)*81+(cell2_j-1)*9+k);
                        }
                    }
                }
            }
        }
    }
    
    fclose(cnf_fp);
    return cnf_file;
}

//解决数独问题
int SolveSudoku(int CoR){
    //创建数独文件
    char* sudoku_file=CreateSudoku(CoR);
    if(sudoku_file==NULL){
        printf("create failed!\n");
        return 0;
    }
    //数独文件转化为cnf文件
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
    printf("what's your strategy?(0/1)");
    int strategy=0;
    scanf("%d",&strategy);
    Result* result = Solve(sat, strategy);
    if(result == NULL) {
        printf("Failed to solve SAT problem\n");
        Clear(sat->Chead);
        free(sat);
        return 0;
    }
    
    //输出结果
    if(result->ans == 1){
        printf("Sudoku solution found:\n");
        SaveResult(sat,result,strategy);
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