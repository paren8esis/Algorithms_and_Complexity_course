/* Copyright 2015 paren8esis*/

/* This program is free software: you can redistribute it and/or modify*/
/*    it under the terms of the GNU General Public License as published by*/
/*    the Free Software Foundation, either version 3 of the License, or*/
/*    (at your option) any later version.*/

/*    This program is distributed in the hope that it will be useful,*/
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of*/
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the*/
/*    GNU General Public License for more details.*/

/*    You should have received a copy of the GNU General Public License*/
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <stdlib.h>
#include <stdio.h>

#define MAX_NMK 300000
#define inf 2000000001

long long int N;  // number of cities
long long int M;  // number of roads
long long int K;  // number of road works


/*********/
/* Graph */
/*********/

/* A structure to represent a weighted edge in graph */
struct Edge {
	long long int src, dest, time;
};

struct Edge mst[MAX_NMK];   // The MST computed by Kruskal

/* A structure to represent an undirected weighted graph */
struct Graph {
	struct Edge* edge;  // array of edges
	long long int numEdges;   // number of edges in array
};

/* Initialization of a new graph G(N,M,d) */
struct Graph* createGraph(long long int m) {
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));

	graph->edge = (struct Edge*) malloc(m * sizeof(struct Edge));
	graph->numEdges = 0;
 
	return graph;
}

/**************/
/* Union-Find */
/**************/

/* A structure to represent a subset for union-find */
struct subset {
	long long int parent;
	long long int rank;
};
 
/* Find utility (with path compression) */
long long int find(struct subset subsets[], long long int i) {
	// find root and make root as parent of i (path compression)
	if (subsets[i].parent != i)
		subsets[i].parent = find(subsets, subsets[i].parent);
 
	return subsets[i].parent;
}

/* Union utility (by rank) */
void Union(struct subset subsets[], long long int x, long long int y) {
	long long int xroot = find(subsets, x);
	long long int yroot = find(subsets, y);
 
	// Attach smaller rank tree under root of high rank tree
	// (Union by Rank)
	if (subsets[xroot].rank < subsets[yroot].rank)
		subsets[xroot].parent = yroot;
	else if (subsets[xroot].rank > subsets[yroot].rank)
		subsets[yroot].parent = xroot;
 
	// If ranks are same, then regard one as root and increment
	// its rank by one
	else {
		subsets[yroot].parent = xroot;
		subsets[xroot].rank++;
	}
}
 
/* Function to be used by qsort().
    It compares the weights of two edges. */
int descendingTime(const void* a, const void* b) {
	struct Edge* a1 = (struct Edge*)a;
	struct Edge* b1 = (struct Edge*)b;
	
	if (a1->time > b1->time)
		return -1;
	if (a1->time < b1->time)
		return +1;
	return 0;
}

/* Kruskal's algorithm to determine Maximum Spanning Tree */
void KruskalMST(struct Graph* graph) {
	long long int e = 0;  
	long long int i = 0; 
	long long int v;
 
	/* Sorting the edges in descending weight order */
	qsort(graph->edge, M, sizeof(graph->edge[0]), descendingTime);
 
	struct subset *subsets = (struct subset*) malloc( N * sizeof(struct subset) );
 
	/* Initialization of the union-find structure */
	for (v = 0; v < N; ++v) {
		subsets[v].parent = v;
		subsets[v].rank = 0;
	}

	while (e < N - 1) {
		struct Edge next_edge = graph->edge[i++];
 
		long long int x = find(subsets, next_edge.src);
		long long int y = find(subsets, next_edge.dest);
 
		/* Check if next_edge forms a cycle */
		if (x != y) {
			mst[e++] = next_edge;
			Union(subsets, x, y);
		}
	}

	return;
}

/* Takes two integers as arguments and returns the maximum of the two */
long long int max(long long int a, long long int b) {
	if (a > b)
		return a;
	else
		return b;
}
 

int main(int argc, char *argv[]) {
	long long int i, cityA, cityB, t;

	/* Read #cities, #roads and #road works: N M K */
	if(scanf("%lld %lld %lld", &N, &M, &K) != 3) {
		printf("\nError while reading file.\n");
		exit(1);
	}

	/* Road works can cover all roads available */
	if (K >= M) {
		printf("infinity\n");
		return 0;
	}

	struct Graph* graph = createGraph(max(M, N-1)+N);
	long long int cc = N;

	/* Union-find structure to compute the number of 
		connected components in initial graph */
	struct subset *connectedComponents = (struct subset*) malloc(N * sizeof(struct subset));
 
	/* Initialization of the structure */
	for (i = 0; i < N; ++i) {
		connectedComponents[i].parent = i;
		connectedComponents[i].rank = 0;
	}

	/* Initialization of the array containing the roots 
		of the connected components marked with 0 */
	int rootsCC[N];
	for (i = 0; i < N; i++)
		rootsCC[i] = 0;

	long long int root1, root2, last, j;

	/* Read road details and construct graph */
	for (i = 0; i < M; i++) {
		scanf("%lld %lld %lld", &cityA, &cityB, &t);
		graph->edge[i].src = cityA-1;
		graph->edge[i].dest = cityB-1;
		graph->edge[i].time = t;
	
		graph->numEdges += 1;

		root1 = find(connectedComponents, cityA-1);
		root2 = find(connectedComponents, cityB-1);
		if (root1 != root2) {  // Two connected components were linked
			cc--;
			Union(connectedComponents, cityA-1, cityB-1);
			if (root1 != find(connectedComponents, cityA-1)) {
				rootsCC[root1] = 1;
			} else {
				rootsCC[root2] = 1;
			}
		}
	}

	if (cc > 1) {  // We have more than one connected components
	/* In this case we will link all connected components
		through edges with very big weight (inf) in order to form 
		a fully connected graph */
		i = 0; 
		while (i < N) {
			if (rootsCC[i] == 0) {
				last = i++;
				break;
			}
			i++;
		}

		while (i < N) {
			if (rootsCC[i] == 0) {
				rootsCC[last] = 1;
				graph->edge[graph->numEdges].src = last;
				graph->edge[graph->numEdges].dest = i;
				graph->edge[graph->numEdges].time = inf;

				graph->numEdges += 1;

				last = i;
			}
			i++;
		}	
	}

	/* Kruskal's algorithm in order to compute Maximum Spanning Tree of graph */
	KruskalMST(graph);

	/* Print results */
	printf("%lld\n", mst[(N-1)-cc-K].time);

	return 0;
}

