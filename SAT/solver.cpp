#include"define.hpp"

//用于实时记录用时,便于监控
int T=0;
void clear_line() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    columns = 8;
    printf("\r");
    for (int i = 5; i < columns; i++){
        printf(" ");
    }
    printf("\r");
    fflush(stdout);
}

//复制，用深拷贝
Clause Copy(Clause CL){
    Clause cp_clause=CL;
    //复制句子
    Clause* ctail=&cp_clause, *chead=CL.next;
    while(chead!=NULL){
        ctail->next=(Clause*)malloc(sizeof(Clause));
        ctail=ctail->next;
        //复制句头
        ctail->next=NULL;
        //复制字
        L *ltail=&ctail->Lhead,*lhead=chead->Lhead.next;
        while(lhead!=NULL){
            ltail->next=(L*)malloc(sizeof(L));
            ltail=ltail->next;
            ltail->x=lhead->x;
            ltail->next=NULL;
            lhead=lhead->next;
        }
        chead=chead->next;
    }
    return cp_clause;
}
//清理函数
//清理字
int ClearL(Clause* head){
    L* ltemp;
    while(head->Lhead.next!=NULL){
        ltemp=head->Lhead.next;
        head->Lhead.next=head->Lhead.next->next;
        free(ltemp);
    }
    return OK;
}
//清理句子链表
int Clear(Clause &head){
    if(head.next==NULL) return OK;
    Clause *ctemp;
    while(head.next!=NULL){
        ClearL(head.next);
        ctemp=head.next;
        head.next=head.next->next;
        free(ctemp);
    }
    return OK;
}
//化简,若出现真则删句子;若出现假字,则删字
int Change(Clause &CL,int l){
    if(CL.next==NULL) return OK;
    Clause* Cpre=&CL,*Cp=CL.next;
    while(Cp!=NULL){
        L* Lpre=&Cp->Lhead,*Lp=Cp->Lhead.next;
        int ifdelete=0;
        while(Lp!=NULL){
            if(Lp->x==l){
                ifdelete=1;
                break;
            }else if(Lp->x==-l){
                Lpre->next=Lp->next;
                free(Lp);
            }else{
                Lpre=Lpre->next;    
            }
            Lp=Lpre->next;
        }
        if(ifdelete){
            ClearL(Cp);
            Cpre->next=Cp->next;
            free(Cp);
        }else{
            Cpre=Cpre->next;
        }
        Cp=Cpre->next;
    }
    return OK;
}
//但是注意以下的特别的情况:
//若字都删完了,句子依然在,就成了空句子,永远不能满足,所以该方案必然失败
int EmptyClause(Clause CL){
    Clause* Cp=CL.next;
    while(Cp!=NULL){
        if(Cp->Lhead.next==NULL) return OK;
        Cp=Cp->next;
    }
    return NO;
}
//寻找单子句优先处理
int FindOne(Clause CL){
    Clause* Cp=CL.next;
    while(Cp!=NULL){
        if(Cp->Lhead.next->next==NULL) return Cp->Lhead.next->x;
        Cp=Cp->next;
    }
    return NO;
}
//加入结点
int AddClause(Clause &CP,int l){
    Clause* temp=(Clause*)malloc(sizeof(Clause));
    temp->Lhead.next=(L*)malloc(sizeof(L));
    temp->Lhead.next->x=l;
    temp->Lhead.next->next=NULL;
    temp->next=CP.next;
    CP.next=temp;
    return OK;
}

//优化选择
// int Choose0(Clause CL){
//     return CL.next->Lhead.next->x;
// }

int Choose1(Clause CL,int M){
    int* cnt = (int*)calloc(2*M + 10, sizeof(int));
    Clause* chead=CL.next;
    L* lhead=NULL;
    while(chead!=NULL){
        lhead=chead->Lhead.next;
        while(lhead!=NULL){
            cnt[lhead->x+M]++;
            lhead=lhead->next;
        }
        chead=chead->next;
    }
    int max=0,max_i=M;
    for(int i=0;i<=2*M;i++){
        if(max<cnt[i]){
            max=cnt[i];
            max_i=i;
        }
    }
    free(cnt);
    return max_i-M;
}

//递归函数
//隐式回溯,因为赋值只由选择的那一个字决定后续的所有赋值,所以用全局变量各条分支也不会互相影响
int DPLL(Clause &CL,int* bl,int M,int choose){
    //实时显示运行时间
    clear_line();
    printf("%d",clock()-T);
    
    if(CL.next==NULL) return OK;
    Clause cp_CL;
    int chs=0;
    while((chs=FindOne(CL))!=0){
        bl[abs(chs)] = (chs > 0 )? 1 : -1;
        Change(CL,chs);
        if(CL.next==NULL){
            Clear(cp_CL);
            return OK;
        }else if(EmptyClause(CL)==1){
            Clear(cp_CL);
            return NO;
        }
    }
    if(choose==0) chs=CL.next->Lhead.next->x;
    else if(choose==1) chs=Choose1(CL,M);
    //假设字chs为真
    cp_CL=Copy(CL);
    AddClause(cp_CL,chs);
    if(DPLL(cp_CL,bl,M,choose)==1){
        Clear(cp_CL);
        return OK;
    }
    Clear(cp_CL);
    cp_CL=Copy(CL);
    //回溯,尝试假设chs为假
    AddClause(cp_CL,-chs);
    if(DPLL(cp_CL,bl,M,choose)==1){
        Clear(cp_CL);
        return OK;
    }
    Clear(cp_CL);
    return NO;
}


int SaveResult(SAT* sat,Result* result,int choose){
    //处理输出文件名称
    char res_file[20];
    strcpy(res_file,sat->name);
    int t=0;
    while(res_file[t++]!='.');
    //标记优化的结果
    if(choose!=0){
        t--;
        res_file[t++]='_',res_file[t++]='0'+choose,res_file[t++]='.';
    }
    res_file[t++]='r',res_file[t++]='e',res_file[t++]='s',res_file[t]='\0';
    //写入答案
    FILE* fp=fopen(res_file,"w");
    if(fp==NULL) return 0;
    if(result->ans==1){
        fprintf(fp,"s 1\nv ");
        for(int i=1;i<=sat->m;i++){
            fprintf(fp,"%d ",i*result->bl[i]);
        }
    }else{
        fprintf(fp,"s -1");
    }
    fprintf(fp,"\nt %dms",result->time);
    fclose(fp);
    return 1;
}

//调用主函数
Result* Solve(SAT* sat,int choose){
    if(choose<0||choose>1) return NULL;
    Result* result=(Result*)malloc(sizeof(Result));
    result->bl=(int*)malloc(sizeof(int)*(sat->m+10));
    for(int i=0;i<sat->m+10;i++){
        result->bl[i]=0;
    }
    result->time=0;//记录用时
    
    int start=T=clock();
    printf("        ms");
    result->ans=DPLL(sat->Chead,result->bl,sat->m,choose);
    int end=clock();
    result->time=end-start;
    
    printf("\rtime:%d  ms\n",result->time);//为了与clearline函数一致
    return result;
}