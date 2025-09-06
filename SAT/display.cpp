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
        printf("   3. SaveResult 4. Sudoku\n");
        printf("What's your choice:");
        scanf("%d",&input);
        switch(input){
            case 1:
                //读取模块
                printf("Input your file name:");
                scanf("%s",file);
                sat=Readcnf(file);
                if(sat==NULL) printf("Open failed!\n");
                else printf("Open successful!\n");
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
                break;
            case 3:
                //数独模块
                break;
            case -1:
                break;
        }
        getchar();
        getchar();
        system("cls");
    }
    //清理内存占用
    Clear(sat->Chead);
    free(sat);

    printf("Thanks for using!");
    return 1;
}