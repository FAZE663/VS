#include <stdio.h>
#include <limits.h>
#define MAX_NODES 10

int node_count;
int distances[MAX_NODES][MAX_NODES];
int next_hop[MAX_NODES][MAX_NODES];

void perform_distance_vector_routing();

int main() {
    printf("Enter the number of nodes (max %d): ", MAX_NODES);
    scanf("%d", &node_count);

    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (i == j) {
                distances[i][j] = 0;
            } else {
                distances[i][j] = INT_MAX;
            }
            next_hop[i][j] = j;
        }
    }

    printf("\nProvide distances between nodes (-1 for no direct connection):\n");
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (i != j) {
                int distance;
                printf("Distance from Node %d to Node %d: ", i, j);
                scanf("%d", &distance);
                if (distance != -1) {
                    distances[i][j] = distance;
                }
            }
        }
    }

    perform_distance_vector_routing();

    printf("\nFinal Routing Tables:\n");
    for (int i = 0; i < node_count; i++) {
        printf("Router %d\n", i);
        printf("Destination\tNext Hop\tDistance\n");
        for (int j = 0; j < node_count; j++) {
            printf("%d\t\t%d\t\t%d\n", j, next_hop[i][j], distances[i][j]);
        }
        printf("\n");
    }

    return 0;
}

void perform_distance_vector_routing() {
    int is_updated;
    do {
        is_updated = 0;
        for (int i = 0; i < node_count; i++) {
            for (int j = 0; j < node_count; j++) {
                if (distances[i][j] != INT_MAX) {
                    for (int k = 0; k < node_count; k++) {
                        if (distances[j][k] != INT_MAX && distances[i][j] + distances[j][k] < distances[i][k]) {
                            distances[i][k] = distances[i][j] + distances[j][k];
                            next_hop[i][k] = next_hop[i][j];
                            is_updated = 1;
                        }
                    }
                }
            }
        }
    } while (is_updated);
}

