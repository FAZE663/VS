#include <stdio.h>
#include <unistd.h> 

int main() {
    int max_capacity, incoming_data, outgoing_data;
    int current_data = 0;
    int iteration = 0;

    printf("Enter the maximum bucket capacity: ");
    scanf("%d", &max_capacity);

    printf("Enter the incoming data rate (units per cycle): ");
    scanf("%d", &incoming_data);

    printf("Enter the outgoing data rate (units per cycle): ");
    scanf("%d", &outgoing_data);

    while (1) {
        iteration++;
        printf("\nIteration %d:\n", iteration);

        current_data += incoming_data;
        printf("Data added to bucket: %d units\n", incoming_data);

        if (current_data > max_capacity) {
            printf("Warning: Bucket overflow! Discarding excess data.\n");
            current_data = max_capacity; // Discard excess data
        }

        current_data -= outgoing_data;
        if (current_data < 0) {
            current_data = 0;
        }
        
        printf("Data removed from bucket: %d units\n", outgoing_data);
        printf("Current bucket status: %d units\n", current_data);

        sleep(1); 
    }

    return 0;
}

