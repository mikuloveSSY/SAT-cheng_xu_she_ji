#include"define.hpp"

int Display(){
    //初始化
    char file[20];
    SAT* sat=NULL;
    Result* result=NULL;
    int input=0;
    while(input!=-1){
        printf("-----------Menu------------\n");
        printf("   1. Readcnf    2. Solvecnf\n");
        printf("   3. Sudoku\n");
        printf("---------------------------\n");
        printf("   -1 Exit\n");
        printf("What's your choice:");
        scanf("%d",&input);
        switch(input){
            case 1:
                //读取模块
                if(sat!=NULL){
                    Clear(sat->Chead);
                    free(sat);
                }
                printf("Input your file name:");
                scanf("%s",file);
                sat=Readcnf(file);
                if(sat==NULL) printf("Open failed!\n");
                else printf("Open successful!\n");
                getchar();
                getchar();
                break;
            case 2:
                //解答模块
                if(sat==NULL) printf("no cnf file\n");
                else{
                    printf("whitch strategy:");
                    int strategy=0;
                    scanf("%d",&strategy);
                    result=Solve(sat,strategy);
                    if(result==NULL){
                        printf("no strategy,can't solve\n");
                        break;
                    }
                    //是否保存结果
                    printf("Do you want to save Result?(1/0)");
                    int save=0;
                    scanf("%d",&save);
                    if(save){
                        int save_ok=SaveResult(sat,result,strategy);
                        if(save_ok) printf("save ok\n");
                        else printf("save failed\n");
                    }else{
                        printf("no save\n");
                    }
                    //result随用随删
                    free(result->bl);
                    free(result);
                }
                getchar();
                getchar();
                break;
            case 3:
                //数独模块
                printf("Create or Read?(1/0)");
                int CoR=0;
                scanf("%d",&CoR);
                if(CoR!=0||CoR!=1){
                    printf("Wrong input!");
                    break;
                }
                SolveSudoku(CoR);
                getchar();
                getchar();
                break;
            case -1:
                break;
        }
        system("cls");
    }
    //清理内存占用
    Clear(sat->Chead);
    free(sat);

    printf("Thanks for using!");
    return 1;
}