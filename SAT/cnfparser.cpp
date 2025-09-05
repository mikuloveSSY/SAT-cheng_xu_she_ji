#include"define.hpp"

SAT* Readcnf(char* file){
    FILE *fp=fopen(file,"r");
    if(fp==NULL){
        return NULL;
    }
    SAT *sat=(SAT*)malloc(sizeof(SAT));
    //跳过注释
    char x=fgetc(fp);
    while(x=='c'){
        while(fgetc(fp)!='\n');
        x=fgetc(fp);
    }
    //跳过第一行,并读出数据
    char line[100];fscanf(fp,"%s",line);
    fscanf(fp,"%d %d",&sat->m,&sat->n);
    //读入数据
    sat->Chead.next=NULL;
    Clause *chead=&sat->Chead;
    for(int i=0;i<sat->n;i++){
        chead->next=(Clause*)malloc(sizeof(Clause));
        chead=chead->next;
        chead->next=NULL;
        L* tail=&chead->Lhead;
        int num=0;
        while(1){
            fscanf(fp,"%d",&num);
            if(num==0) break;
            tail->next=(L*)malloc(sizeof(L));
            tail=tail->next;
            tail->next=NULL;
            tail->x=num;
        }
    }
    fclose(fp);
    return sat;
}

int Reoutput(SAT *sat){
    FILE* fp=fopen("output.cnf","w");
    if(fp==NULL) return -1;
    fprintf(fp,"p cnf %d %d\n",sat->m,sat->n);
    Clause* h1=sat->Chead.next;
    L* h2=h1->Lhead.next;
    while(h1!=NULL){
        h2=h1->Lhead.next;
        while(h2!=NULL){
            fprintf(fp,"%d ",h2->x);
            h2=h2->next;
        }
        fprintf(fp,"0\n");
        h1=h1->next;
    }
    fclose(fp);
    return 0;
}
