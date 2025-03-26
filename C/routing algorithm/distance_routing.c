#include<stdio.h>
#include<stdlib.h>


struct node {
        int id;
        int routing_table[10][3];
        int num_neighbours;
        struct node* neighbour[10];
    }node;



void initialize_node(struct node* Node,int id){
    Node->id=id;
    int val=0;
    int n, c;
    for (int i=0;i<10;i++){
        Node->routing_table[i][0]=-1;
        Node->routing_table[i][1]=-1;
        Node->routing_table[i][2]=-1;

    }
    printf("\ninput the number of neighbours of Node %d :",id);
    scanf("%d",Node->num_neighbours);
    for (int i=0;i< Node->num_neighbours;i++){
        printf("\ninput the node and cost of the neighbour of Node %d : ",id);
        scanf("%d%d",&n,&c);//gets the id and cost of each neighbour
        Node->routing_table[n][0]=n;
        Node->routing_table[i][2]=c;
        Node->routing_table[i][1]=Node->routing_table[i][0]; //set the next hop as the same edge itself
        Node->neighbour[val]=Node->routing_table[i][0];
        val++;
    }
}



int main(){
    printf("---distane routing algorithm---");
    struct node* router;
    initialize_node(router,0);
    for (int i=0;i< router->num_neighbours;i++){
        for(int j=0;j<3;j++){
            printf("%d\t",router->routing_table[i][j]);
        }
        printf("\n");
    }



}