#include"define.hpp"
//记录状态
int *bl,bnum;

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
    return FALSE;
}
//寻找单子句优先处理
int FindOne(Clause CL){
    Clause* Cp=CL.next;
    while(Cp!=NULL){
        if(Cp->Lhead.next->next==NULL) return Cp->Lhead.next->x;
        Cp=Cp->next;
    }
    return FALSE;
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
int Choose0(Clause CL){
    return CL.next->Lhead.next->x;
}
int Choose1(Clause CL){
    
}


//递归函数
//隐式回溯,因为赋值只由选择的那一个字决定后续的所有赋值,所以用全局变量各条分支也不会互相影响
int DPLL(Clause &CL){
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
            return FALSE;
        }
    }
    chs=CL.next->Lhead.next->x;
    //假设字chs为真
    cp_CL=Copy(CL);
    AddClause(cp_CL,chs);
    if(DPLL(cp_CL)==1){
        Clear(cp_CL);
        return OK;
    }
    Clear(cp_CL);
    cp_CL=Copy(CL);
    //回溯,尝试假设chs为假
    AddClause(cp_CL,-chs);
    if(DPLL(cp_CL)==1){
        Clear(cp_CL);
        return OK;
    }
    Clear(cp_CL);
    return FALSE;
}


//调用主函数
int Solve(SAT* sat){
    bnum=sat->m;
    bl=(int*)malloc(sizeof(int)*(sat->m+10));
    for(int i=0;i<sat->m+10;i++){
        bl[i]=0;
    }
    bl[0]=sat->m;

    int ans=DPLL(sat->Chead);
    
    if(ans==1){
        for(int i=1;i<=bl[0];i++){
            printf("%d ",i);
            bl[i]>0?printf("TRUE\n"):printf("FALSE\n");
        }
    }else{
        printf("\nno ans");
    }
    free(bl);
    return 0;
}