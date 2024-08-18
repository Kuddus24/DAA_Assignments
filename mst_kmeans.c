#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Function to read data from file
float **vertex_set(const char *infile, int *vsize, int *dim, int *k) {
    FILE *file = fopen(infile, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    fscanf(file, "%d", vsize);
    fscanf(file, "%d", dim);
    fscanf(file, "%d", k);
    float **vertices = malloc(*vsize * sizeof(float*));
    for (int i = 0; i < *vsize; ++i) {
        vertices[i] = malloc(*dim * sizeof(float));
        for (int j = 0; j < *dim; ++j) {
            fscanf(file, "%f", &vertices[i][j]);
        }
    }
    fclose(file);
    return vertices;
}

// Structure to represent an edge in the graph
struct Edge {
    int src, dst;
    float weight;
};

// Function to calculate Euclidean distance between two points
float dist(float *p1, float *p2, int dim) {
    float sum = 0.0;
    for (int i = 0; i < dim; ++i) {
        sum += pow(p1[i] - p2[i], 2);
    }
    return sqrt(sum);
}

// Calculate distances between each pair of points
struct Edge* weighing(float **v, int vsize, int dim, int *esize) {
    *esize = (vsize * (vsize - 1)) / 2;
    int e = 0;
    struct Edge* Eset = malloc(sizeof(struct Edge) * (*esize));
    for (int i = 0; i < vsize; ++i) {
        for (int j = i + 1; j < vsize; ++j) {
            Eset[e].src = i;
            Eset[e].dst = j;
            Eset[e++].weight = dist(v[i], v[j], dim);
        }
    }
    return Eset;
}

// Structure to represent a disjoint set element
struct Dset {
    int parent;
    int rank;
};

// Function to initialize a disjoint set
void makeSet(struct Dset *set, int n) {
    for (int i = 0; i < n; ++i) {
        set[i].parent = i;
        set[i].rank = 0;
    }
}

// Function to find the root of an element with path compression
int find(struct Dset *set, int i) {
    if (set[i].parent != i) {
        set[i].parent = find(set, set[i].parent); // Path compression
    }
    return set[i].parent;
}

// Function to perform union of two sets by rank
void Union(struct Dset *set, int x, int y) {
    int rootX = find(set, x);
    int rootY = find(set, y);

    if (rootX != rootY) {
        // Union by rank
        if (set[rootX].rank < set[rootY].rank) {
            set[rootX].parent = rootY;
        } else if (set[rootX].rank > set[rootY].rank) {
            set[rootY].parent = rootX;
        } else {
            set[rootY].parent = rootX;
            set[rootX].rank++;
        }
    }
}

// Function to swap two elements
void swap(struct Edge* a, struct Edge* b) {
    struct Edge temp = *a;
    *a = *b;
    *b = temp;
}

// Function to partition the array using a random pivot element
int partition(struct Edge *arr, int low, int high) {
    // Generate a random index between low and high
    srand(time(NULL));
    int random = low + rand() % (high - low + 1);
    // Swap the pivot element with the last element
    swap(&arr[random], &arr[high]);

    struct Edge pivot = arr[high]; // Pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j].weight <= pivot.weight) {
            i++; // Increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Function to implement randomized quicksort
void quickSort(struct Edge *arr, int low, int high) {
    if (low < high) {
        // Partition the array
        int pi = partition(arr, low, high);

        // Recursively sort elements before and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Kruskal's Minimum Spanning Tree algorithm
struct Edge* Kruskal(struct Edge *edges, int V, int E) {
    // Allocate memory for creating V subsets
    struct Dset* subsets = (struct Dset*)malloc(V * sizeof(struct Dset));

    // Create V subsets with single elements
    makeSet(subsets, V);

    // Sort all the edges in increasing order of their weight
    quickSort(edges, 0, E-1);

    // Allocate memory for the result
    struct Edge* result = (struct Edge*)malloc((V - 1) * sizeof(struct Edge));

    int e = 0; // An index variable used for result
    int i = 0; // An index variable used for sorted edges
    while (e < V - 1 && i < E) {
        // Pick the smallest edge. Increment index for next iteration
        struct Edge next_edge = edges[i++];

        int x = find(subsets, next_edge.src);
        int y = find(subsets, next_edge.dst);

        // If including this edge does not cause cycle, include it in the result and increment the index
        if (x != y) {
            result[e++] = next_edge;
            Union(subsets, x, y);
        }
    }
    free(subsets);
    return result;
}

// Function to print the constructed MST
void printMST(struct Edge *result, int V) {
    printf("Following are the edges in the constructed MST:\n");
    float minimumCost = 0;
    for (int i = 0; i < V - 1; ++i) {
        printf("%d -- %d == %f\n", result[i].src, result[i].dst, result[i].weight);
        minimumCost += result[i].weight;
    }
    printf("Cost of the Spanning Tree(Minimum): %f\n", minimumCost);
}

// Function for cluster assignment
struct Dset* cluster(struct Edge* MST, int V, int k) {
    // Delete the heaviest k-1 weight from the MST
    quickSort(MST, 0, V - 2);

    // Construct disjoint sets corresponding to this updated edgeset
    struct Dset* set = malloc(V * sizeof(struct Dset));
    makeSet(set, V);
    for (int i = 0; i < V - k; i++) {
        int s = MST[i].src;
        int d = MST[i].dst;
        Union(set, s, d);
    }
    // Set rank to zero
    for (int i = 0; i < V; i++)
        set[i].rank = 0;

    // Assigning each vertex to its cluster group
    int flag = 1;
    for (int i = 0; i < V; i++) {
        int kcl = set[find(set, i)].rank;
        if (kcl != 0)
            set[i].rank = kcl;
        else set[find(set, i)].rank = set[i].rank = flag++;
    }
    return set;
}

// Function to print the vertices with the cluster group
void printKcluster(struct Dset* set, const char* outfile, float** vertices, int V, int dim) {
    FILE *file = fopen(outfile, "w");
    if (file != NULL) {
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < dim; ++j)
                fprintf(file, "%f  ", vertices[i][j]);
            fprintf(file, "%d \n", set[i].rank);
        }
        fclose(file);
    } else {
        printf("Error opening file!\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *infile = argv[1];
    char *outfile = argv[2];

    int vsize, dim, k;
    float **V = vertex_set(infile, &vsize, &dim, &k);
    int esize;
    struct Edge* E = weighing(V, vsize, dim, &esize);
    struct Edge* MST = Kruskal(E, vsize, esize);
    struct Dset* clusters = cluster(MST, vsize, k);
    printKcluster(clusters, outfile, V, vsize, dim);

    for (int i = 0; i < vsize; ++i) {
        free(V[i]);
    }
    free(V);
    free(E);
    free(MST);
    free(clusters);

    return 0;
}
