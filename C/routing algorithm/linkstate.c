#include <stdio.h>
#include <limits.h>
#define MAX_NODES 10

int node_count;
int graph[MAX_NODES][MAX_NODES];
int visited_nodes[MAX_NODES];
int shortest_distances[MAX_NODES];
int previous_node[MAX_NODES];

void run_dijkstra(int start_node);

int main() {
    int start_node;

    printf("Enter the number of nodes (max %d): ", MAX_NODES);
    scanf("%d", &node_count);

    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            graph[i][j] = (i == j) ? 0 : INT_MAX;
        }
    }

    printf("\nProvide distances between nodes (-1 for no direct connection):\n");
    for (int i = 0; i < node_count; i++) {
        for (int j = i + 1; j < node_count; j++) {
            int dist;
            printf("Distance between Node %d and Node %d: ", i, j);
            scanf("%d", &dist);

            if (dist != -1) {
                graph[i][j] = graph[j][i] = dist;
            }
        }
    }

    printf("Enter the starting node: ");
    scanf("%d", &start_node);

    for (int i = 0; i < node_count; i++) {
        visited_nodes[i] = 0;
        shortest_distances[i] = INT_MAX;
        previous_node[i] = -1;
    }

    run_dijkstra(start_node);

    printf("\nShortest Paths from Node %d:\n", start_node);
    printf("Node\tDistance\tPath\n");

    for (int i = 0; i < node_count; i++) {
        printf("%d\t%d\t\t", i, shortest_distances[i]);
        int temp_node = i;

        if (previous_node[temp_node] == -1) {
            printf("%d", temp_node);
        } else {
            int path[MAX_NODES], path_index = 0;
            while (temp_node != -1) {
                path[path_index++] = temp_node;
                temp_node = previous_node[temp_node];
            }

            for (int j = path_index - 1; j >= 0; j--) {
                printf("%d", path[j]);
                if (j > 0) printf(" -> ");
            }
        }
        printf("\n");
    }

    return 0;
}

void run_dijkstra(int start_node) {
    shortest_distances[start_node] = 0;

    for (int count = 0; count < node_count - 1; count++) {
        int min_distance = INT_MAX;
        int min_node = -1;

        for (int i = 0; i < node_count; i++) {
            if (!visited_nodes[i] && shortest_distances[i] < min_distance) {
                min_distance = shortest_distances[i];
                min_node = i;
            }
        }

        int current_node = min_node;
        visited_nodes[current_node] = 1;

        for (int v = 0; v < node_count; v++) {
            if (!visited_nodes[v] && graph[current_node][v] != INT_MAX && 
                shortest_distances[current_node] + graph[current_node][v] < shortest_distances[v]) {
                shortest_distances[v] = shortest_distances[current_node] + graph[current_node][v];
                previous_node[v] = current_node;
            }
        }
    }
}

