#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

const int INF = 1000000007;
int graph[100][100];
int distance[100];
int next_hop[100];
int parent[100];

void bellman_ford_algo(int source_node, int num_nodes, int edges)
{

	for(int i = 1; i <= num_nodes; i++) 
    {
        distance[i] = INF; 
        next_hop[i] = -1;
        parent[i] = -1;
    }
	
	distance[source_node] = 0;
	next_hop[source_node] = source_node;

	for(int i = 0; i < num_nodes-1; i++) 
    {
		for(int j = 1; j <= num_nodes; j++) 
        {
			for(int k = 1; k <= num_nodes; k++) 
            {
				if(graph[j][k] == INF) 
                    continue;
				if(distance[j] + graph[j][k] < distance[k]) 
                {
					distance[k] = distance[j] + graph[j][k];
					parent[k] = j;
				}
			}
		}
	}

	for(int i = 1; i <= num_nodes; i++) 
    {
		if(parent[i] == -1) 
            next_hop[i] = i;
		else 
        {
			int j = i;
			while(parent[j] != source_node) 
            {
				j = parent[j];
			}
			next_hop[i] = j;
		}
	}
    printf("Destination Node \t Next Hop \t Distance\n");
	for(int i = 1; i <= num_nodes; i++) 
    {
        printf("\t%d \t\t    %d \t\t    %d\n", i, next_hop[i], distance[i]);
	}
}

int main()
{
	int num_nodes, edges;
    printf("Enter number of nodes and edges: \n");
    scanf("%d %d", &num_nodes, &edges);

	for(int i = 0; i < 100; i++) 
    {
		for(int j = 0; j < 100; j++) 
        {
			graph[i][j] = INF;
		}
	}

	for (int i = 0; i < edges; i++) 
    {
		int v1, v2, weight;
		scanf("%d %d %d", &v1, &v2, &weight);
		graph[v1][v2] = weight;
		graph[v2][v1] = weight;
	}

    while(1)
	{
        printf("Enter the node whose routing table is required\n");
        int routing_node;
        scanf("%d",&routing_node);
        if(routing_node==0)
            exit(0);
        printf("-------------------------------------------------\n");
        printf("Routing Table for node %d\n",routing_node);
        bellman_ford_algo(routing_node, num_nodes, edges);
        printf("-------------------------------------------------\n");
    }
	return 0;
}