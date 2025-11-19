#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define min(a,b) (a<b)?(a):(b)
#define max(a,b) (a>b)?(a):(b)

#define MONTE_CARLO_SAMPLES 1000000


struct Node{
    double x,y;
    double r;
};

struct Graph{
    int n;
    struct Node* nodes;
    int **matrix;
};

struct Stack{
    int index;
    struct Stack* next;
};

struct Stack* pop(struct Stack* s){
    struct Stack* p=s->next;
    free(s);
    return p;
}

struct Stack* push(struct Stack* s,int index){
    struct Stack* p=malloc(sizeof(struct Stack));
    p->index=index;
    p->next=s;
    return p;
}

double random() {
    return (double)rand()/RAND_MAX;
}

struct Graph* createGraph(int n){
    struct Graph* g=malloc(sizeof(struct Graph));
    struct Node* nodes=malloc(n*sizeof(struct Node));
    g->n=n;
    g->nodes=nodes;
    g->matrix=malloc(n*sizeof(int*));
    for(int i=0;i<n;i++)
        g->matrix[i]=calloc(n,sizeof(int));
    return g;
}

void freeGraph(struct Graph* g){
    free(g->nodes);
    for(int i=0;i<g->n;i++)
        free(g->matrix[i]);
    free(g->matrix);
    free(g);
}

void addEdge(struct Graph* g,int i,int j){
    g->matrix[i][j]=1;
}

void removeEdge(struct Graph* g,int i,int j){
    g->matrix[i][j]=0;
}

int addNode(struct Graph* g) {
    int index = g->n;
    g->n = index+1;
    g->nodes=realloc(g->nodes,(g->n)*sizeof(struct Node));
    g->matrix=realloc(g->matrix,(g->n)*sizeof(int*));
    g->matrix[index]=calloc(g->n,sizeof(int));
    for(int i = 0; i < index; i++) {
        g->matrix[i]=realloc(g->matrix[i],(g->n)*sizeof(int));
        g->matrix[i][index] = 0;
    }
    return index;
}

void removeNode(struct Graph* g, int index) {
    // remove from matrix
    for(int i = 0; i < g->n; i++) {
        memcpy(g->matrix[i]+index,g->matrix[i]+(index+1),(g->n-index)*sizeof(int));
        g->matrix[i]=realloc(g->matrix[i],(g->n-1)*sizeof(int));
    }
    memcpy(g->matrix+index,g->matrix+(index+1),(g->n-index)*sizeof(int*));
    g->matrix=realloc(g->matrix,(g->n-1)*sizeof(int*));
    // remove from nodes
    memcpy(g->nodes+index,g->nodes+(index+1),(g->n-index)*sizeof(struct Node));
    g->nodes=realloc(g->nodes,(g->n-1)*sizeof(struct Node));

    g->n--;
}

int inCircle(double x, double y, struct Node node){
    double dx=x-node.x;
    double dy=y-node.y;
    return (dx*dx+dy*dy)<=node.r*node.r;
}

void setInfo(struct Graph* g,int i,double x,double y,double r){
    g->nodes[i].x=x;
    g->nodes[i].y=y;
    g->nodes[i].r=r;
    for(int j=0;j<g->n;j++){
        if(j==i)
            continue;

        if(inCircle(x,y,g->nodes[j]))
            addEdge(g,j,i);
        else
            removeEdge(g,j,i);

        if(inCircle(g->nodes[j].x,g->nodes[j].y,g->nodes[i]))
            addEdge(g,i,j);
        else
            removeEdge(g,i,j);
    }
}

struct Graph* loadGraph(char* fn){
    char line[100];
    FILE *f=fopen(fn,"r");
    if(f==NULL) {
        printf("[Error] File does not exist.\n");
        return NULL;
    }
    fgets(line,100,f);
    int n;
    sscanf(line,"%d",&n);
    struct Graph* g=createGraph(n);
    double x;
    double y;
    double r;
    for(int i=0;i<n;i++){
        fgets(line,100,f);
        sscanf(line,"%lf %lf %lf",&x,&y,&r);
        setInfo(g,i,x,y,r);
    }
    printf("Minefield loaded. Number of mines: %d\n", n);
    fclose(f);
    return g;
}

double area(struct Node* nodes, int n){
    double minX = nodes[0].x - nodes[0].r;
    double maxX = nodes[0].x + nodes[0].r;
    double minY = nodes[0].y - nodes[0].r;
    double maxY = nodes[0].y + nodes[0].r;
    for(int i=1;i<n;i++){
        minX=min(minX,nodes[i].x - nodes[i].r);
        maxX=max(maxX,nodes[i].x + nodes[i].r);
        minY=min(minY,nodes[i].y - nodes[i].r);
        maxY=max(maxY,nodes[i].y + nodes[i].r);
    }
    double p=(maxX-minX)*(maxY-minY);
    int k=0;
    srand(time(NULL));
    for(int i=0;i<MONTE_CARLO_SAMPLES;i++){
        double x=random()*(maxX-minX)+minX;
        double y=random()*(maxY-minY)+minY;
        for(int j=0;j<n;j++){
            if(inCircle(x,y,nodes[j])){
                k++;
                break;
            }
        }
    }
    return p*k/MONTE_CARLO_SAMPLES;
}

void printGraph(struct Graph* g){
    for(int i=0;i<g->n;i++){
        for(int j=0;j<g->n;j++)
            printf("%d ",g->matrix[i][j]);
        printf("\n");
    }
        printf("\n");
}

int* explode(struct Graph* g,double x, double y, double r, int* k){
    int* result=calloc(g->n,sizeof(int));
    int* visited=calloc(g->n,sizeof(int));
    struct Stack* s=NULL;
    struct Node* rocket = malloc(sizeof(struct Node));
    rocket->x = x;
    rocket->y = y;
    rocket->r = r;

    for(int i=0;i<g->n;i++)
        if(inCircle(g->nodes[i].x, g->nodes[i].y, *rocket))
            s=push(s,i);

    while(s!=NULL){
        int i = s->index;
        s=pop(s);
        if(visited[i])
            continue;
        visited[i]=1;
        result[*k] = i;
        *k = *k + 1;
        for(int j=0;j<g->n;j++)
            if(g->matrix[i][j])
                s=push(s,j);
    }
    free(s);
    result = realloc(result, (*k) * sizeof(int));
    return result;
}

int efficiency(struct Graph* g, int index){
    int k = 0;

    // varianta 1
    // int* result = explode(g, g->nodes[index].x, g->nodes[index].y, 0, &k);
    // free(result);

    // varianta 2
    int* visited=calloc(g->n,sizeof(int));
    struct Stack* s=NULL;
    s=push(s,index);
    while(s!=NULL){
        int i = s->index;
        s=pop(s);
        if(visited[i])
            continue;
        visited[i]=1;
        k++;
        for(int j=0;j<g->n;j++)
            if(g->matrix[i][j])
                s=push(s,j);
    }
    free(s);

    return k;
}

int maxEfficiencyNodeIndex(struct Graph* g) {
    int index = 0;
    int maxEfficiency = efficiency(g, index);
    for(int i = 1; i < g->n; i++) {
        int efficiency_i = efficiency(g, i);
        if(efficiency_i > maxEfficiency) {
            maxEfficiency = efficiency_i;
            index = i;
        }
    }
    return index;
}

struct Node* nodes(struct Graph* g, int* indexes, int n) {
    struct Node* nodes = malloc(n * sizeof(struct Node));
    for(int i = 0; i < n; i++)
        nodes[i] = g->nodes[indexes[i]];
    return nodes;
};

double maxEfficiencyArea(struct Graph* g) {
    int index = maxEfficiencyNodeIndex(g);
    int k = 0;
    int* indexes = explode(g, g->nodes[index].x, g->nodes[index].y, 0, &k);
    struct Node* nodeList = nodes(g, indexes, k);
    double r = area(nodeList, k);
    free(nodeList);
    free(indexes);
    return r;
}


void menu(struct Graph* g){
    int command,index,k,*result;
    double x,y,r;
    while(1){
        printf("\nMenu:\n");
        printf("[1] Find efficiency of selected mine\n");
        printf("[2] Find max efficiency mine\n");
        printf("[3] Rocket explosion\n");
        printf("[4] Calculate max efficiency mine detonated area\n");
        printf("[5] Back to start\n\n");
        char command = getch(); // scanf("%d",&command);
        switch (command){
            case '1':
                do{
                    printf("Mine index [0-%d] = ", g->n - 1);
                    scanf("%d",&index);
                    if(index<0 || index>=g->n)
                        printf("[Error] Mine index out of bounds.\n");
                }while(index<0 || index>=g->n);
                printf("Efficiency: %d\n",efficiency(g,index));
                break;
            case '2':
                index = maxEfficiencyNodeIndex(g);
                printf("Max efficiency is %d (mine index = %d).\n", efficiency(g,index), index);
                break;
            case '3':
                printf("x = ");
                scanf("%lf",&x);
                printf("y = ");
                scanf("%lf",&y);
                printf("r = ");
                scanf("%lf",&r);
                int k = 0;
                int* result = explode(g, x, y, r, &k);
                printf("Number of detonated mines: %d\n", k);
                if(k > 0) {
                    printf("Detonated mines: ");
                    for(int i = 0; i < k; i++)
                        printf("%d ",result[i]);
                    printf("\n");
                }
                break;
            case '4':
                printf("Area = %.2lf\n",maxEfficiencyArea(g));
                break;
            case '5':
                freeGraph(g);
                printf("\n\n");
                return;
            default:
                printf("[Error] Unknown command.\n");
        }
    }
}

void start(){
    int command;
    while(1){
        printf("Menu:\n");
        printf("[1] Load minefield from file\n");
        printf("[2] Quit\n\n");
        char command = getch(); // scanf("%d",&command);

        struct Graph* g=NULL;
        char fn[100];

        switch(command) {
        case '1':
            while(g == NULL){
                printf("Filename: ");
                scanf("%s",fn);
                g = loadGraph(fn);
            }
            menu(g);
            break;
        case '2':
            printf("Goodbye.\n");
            return;
        default:
            printf("[Error] Unknown command.\n", command);
        }
    }
}

int main()
{
    start();
    return 0;
}
