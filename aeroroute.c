
#include <stdio.h>
#include <string.h>

#define MAX 12
#define INF 1000000000

typedef struct {
    char name[50];
    int active;
} Airport;

Airport airports[MAX];
int graph[MAX][MAX];
int airportCount = 0;

/* Find an airport using its displayed ID (1 to MAX). */
int validAirport(int id) {
    return id >= 1 && id <= MAX && airports[id - 1].active;
}

/* CREATE: Reset the graph and load sample airports. */
void createGraph(void) {
    int i, j;

    for (i = 0; i < MAX; i++) {
        airports[i].active = 0;
        airports[i].name[0] = '\0';

        for (j = 0; j < MAX; j++)
            graph[i][j] = 0;
    }

    airportCount = 0;

    /* Sample airport network for demonstration. */
    strcpy(airports[0].name, "Pune");
    strcpy(airports[1].name, "Mumbai");
    strcpy(airports[2].name, "Delhi");
    strcpy(airports[3].name, "Bengaluru");
    strcpy(airports[4].name, "Hyderabad");
    strcpy(airports[5].name, "Chennai");

    for (i = 0; i < 6; i++) {
        airports[i].active = 1;
        airportCount++;
    }

    /* Distances in km (illustrative sample weights). */
    graph[0][1] = graph[1][0] = 150;
    graph[0][4] = graph[4][0] = 500;
    graph[1][2] = graph[2][1] = 1150;
    graph[1][4] = graph[4][1] = 620;
    graph[4][2] = graph[2][4] = 1250;
    graph[4][3] = graph[3][4] = 500;
    graph[4][5] = graph[5][4] = 520;
    graph[3][5] = graph[5][3] = 290;
    graph[2][5] = graph[5][2] = 1750;

    printf("\nSample airport graph created successfully.\n");
    printf("These are illustrative distances, not operational flight data.\n");
}

/* INSERT NODE: Add an airport. */
void insertAirport(void) {
    int i;

    if (airportCount == MAX) {
        printf("Airport limit reached.\n");
        return;
    }

    for (i = 0; i < MAX; i++) {
        if (!airports[i].active) {
            printf("Enter airport name: ");

            if (scanf(" %49[^\n]", airports[i].name) != 1) {
                printf("Invalid name.\n");
                return;
            }

            airports[i].active = 1;
            airportCount++;

            printf("Airport added. Its ID is %d.\n", i + 1);
            return;
        }
    }
}

/* INSERT EDGE: Add a route between two airports. */
void insertRoute(void) {
    int a, b, distance;

    printf("Enter source airport ID: ");
    if (scanf("%d", &a) != 1) return;

    printf("Enter destination airport ID: ");
    if (scanf("%d", &b) != 1) return;

    if (!validAirport(a) || !validAirport(b) || a == b) {
        printf("Invalid airport IDs.\n");
        return;
    }

    printf("Enter route distance in km: ");
    if (scanf("%d", &distance) != 1) return;

    if (distance <= 0) {
        printf("Distance must be positive.\n");
        return;
    }

    graph[a - 1][b - 1] = distance;
    graph[b - 1][a - 1] = distance;

    printf("Route added successfully.\n");
}

/* DELETE EDGE: Close/remove a route. */
void deleteRoute(void) {
    int a, b;

    printf("Enter first airport ID: ");
    if (scanf("%d", &a) != 1) return;

    printf("Enter second airport ID: ");
    if (scanf("%d", &b) != 1) return;

    if (!validAirport(a) || !validAirport(b) ||
        graph[a - 1][b - 1] == 0) {
        printf("Route not found or invalid airport ID.\n");
        return;
    }

    graph[a - 1][b - 1] = 0;
    graph[b - 1][a - 1] = 0;

    printf("Route removed. You can now recalculate the route.\n");
}

/* DELETE NODE: Remove an airport and all its connections. */
void deleteAirport(void) {
    int id, j;

    printf("Enter airport ID to delete: ");
    if (scanf("%d", &id) != 1) return;

    if (!validAirport(id)) {
        printf("Invalid airport ID.\n");
        return;
    }

    for (j = 0; j < MAX; j++) {
        graph[id - 1][j] = 0;
        graph[j][id - 1] = 0;
    }

    airports[id - 1].active = 0;
    airportCount--;

    printf("Airport and all its routes deleted.\n");
}

/* DISPLAY: Show airports and their connections. */
void displayGraph(void) {
    int i, j, found;

    printf("\n--- AEROROUTE AIRPORT NETWORK ---\n");

    for (i = 0; i < MAX; i++) {
        if (!airports[i].active) continue;

        printf("\nID %d: %s\n", i + 1, airports[i].name);
        found = 0;

        for (j = 0; j < MAX; j++) {
            if (graph[i][j] > 0) {
                printf("  -> ID %d (%s): %d km\n",
                       j + 1, airports[j].name, graph[i][j]);
                found = 1;
            }
        }

        if (!found)
            printf("  No connected routes.\n");
    }
}

/* BFS: Check reachability using a queue. */
void breadthFirstSearch(void) {
    int start, destination;
    int queue[MAX], visited[MAX] = {0};
    int front = 0, rear = 0, u, v;

    printf("Enter starting airport ID: ");
    if (scanf("%d", &start) != 1) return;

    printf("Enter destination airport ID: ");
    if (scanf("%d", &destination) != 1) return;

    if (!validAirport(start) || !validAirport(destination)) {
        printf("Invalid airport ID.\n");
        return;
    }

    queue[rear++] = start - 1;
    visited[start - 1] = 1;

    while (front < rear) {
        u = queue[front++];

        if (u == destination - 1) {
            printf("Destination is reachable from %s.\n",
                   airports[start - 1].name);
            return;
        }

        for (v = 0; v < MAX; v++) {
            if (airports[v].active &&
                graph[u][v] > 0 && !visited[v]) {
                visited[v] = 1;
                queue[rear++] = v;
            }
        }
    }

    printf("No route currently connects these airports.\n");
}


/* Count active airports reachable from a starting airport.
   A route with distance 0 means no connection. */
int countReachable(int start, int temporarilyRemoved)
{
    int visited[MAX] = {0};
    int queue[MAX];
    int front = 0, rear = 0;
    int count = 0;
    int i, current;

    if (!airports[start].active ||
        start == temporarilyRemoved)
        return 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear)
    {
        current = queue[front++];
        count++;

        for (i = 0; i < MAX; i++)
        {
            if (airports[i].active &&
                i != temporarilyRemoved &&
                graph[current][i] > 0 &&
                !visited[i])
            {
                visited[i] = 1;
                queue[rear++] = i;
            }
        }
    }

    return count;
}

/* Identify airports whose removal reduces reachability. */
void analyzeCriticalAirports(void)
{
    int i;
    int baseline;
    int afterRemoval;
    int loss;

    printf("\n--- CRITICAL AIRPORT ANALYSIS ---\n");
    printf("Reference airport: Pune (ID 1)\n\n");

    baseline = countReachable(0, -1);

    printf("Reachable airports before disruption: %d\n\n",
           baseline);

    printf("%-15s %-18s %-15s\n",
           "Airport", "Reachable after", "Connectivity lost");

    for (i = 0; i < MAX; i++)
    {
        if (airports[i].active)
        {
            afterRemoval = countReachable(0, i);
            loss = baseline - afterRemoval;

            printf("%-15s %-18d %-15d\n",
                   airports[i].name,
                   afterRemoval,
                   loss);
        }
    }

    printf("\nNote: This is a simplified graph simulation.\n");
    printf("Results depend on the selected reference airport.\n");
}

/* DIJKSTRA: Find the minimum-distance route. */
void dijkstra(void) {
    int start, destination;
    int dist[MAX], visited[MAX] = {0}, parent[MAX];
    int i, j, u, minDist;

    printf("Enter starting airport ID: ");
    if (scanf("%d", &start) != 1) return;

    printf("Enter destination airport ID: ");
    if (scanf("%d", &destination) != 1) return;

    if (!validAirport(start) || !validAirport(destination)) {
        printf("Invalid airport ID.\n");
        return;
    }

    for (i = 0; i < MAX; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[start - 1] = 0;

    for (i = 0; i < MAX; i++) {
        u = -1;
        minDist = INF;

        for (j = 0; j < MAX; j++) {
            if (airports[j].active && !visited[j] &&
                dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1) break;

        visited[u] = 1;

        for (j = 0; j < MAX; j++) {
            if (airports[j].active && graph[u][j] > 0 &&
                !visited[j] &&
                dist[u] + graph[u][j] < dist[j]) {
                dist[j] = dist[u] + graph[u][j];
                parent[j] = u;
            }
        }
    }

    if (dist[destination - 1] == INF) {
        printf("No available route to the destination.\n");
        return;
    }

    {
        int path[MAX], count = 0;
        int current = destination - 1;

        while (current != -1 && count < MAX) {
            path[count++] = current;
            current = parent[current];
        }

        printf("\n--- ROUTE PLAN ---\n");
        printf("Route: ");

        for (i = count - 1; i >= 0; i--) {
            printf("%s", airports[path[i]].name);

            if (i > 0) printf(" -> ");
        }

        printf("\nTotal distance: %d km\n", dist[destination - 1]);
    }
}

int main(void) {
    int choice;

    createGraph();

    do {
        printf("\n========== AEROROUTE AI ==========\n");
        printf("1. Create/reset sample graph\n");
        printf("2. Insert airport (node)\n");
        printf("3. Delete airport (node)\n");
        printf("4. Display airport graph\n");
        printf("5. Insert flight route (edge)\n");
        printf("6. Delete flight route (edge)\n");
        printf("7. Find shortest route (Dijkstra)\n");
        printf("8. Check reachability (BFS)\n");
        printf("9. Analyze critical airports\n");
        printf("10. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Restart the program.\n");
            return 1;
        }

        switch (choice) {
            case 1: createGraph(); break;
            case 2: insertAirport(); break;
            case 3: deleteAirport(); break;
            case 4: displayGraph(); break;
            case 5: insertRoute(); break;
            case 6: deleteRoute(); break;
            case 7: dijkstra(); break;
            case 8: breadthFirstSearch(); break;
            case 9: analyzeCriticalAirports(); break;
            case 10: printf("Exiting AeroRoute AI.\n"); break;
            default: printf("Invalid choice.\n");
        }

    } while (choice != 10);

    return 0;
}
