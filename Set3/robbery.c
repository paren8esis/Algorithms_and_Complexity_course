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
#include <string.h>

/* 
	It needs complete redesign. No time though.
	So here's my monstrous code.  
*/

#define MAX_M 500000
#define MAX_N 50000

long long int N;
long long int M;

long long int critical;
long long int noncritical;
long long int nonMST;

/*********/
/* Graph */
/*********/

/* A structure to represent a weighted edge in graph */
struct Edge {
	long long int src, dest, cost;
};

struct Edge mst[MAX_M];

/* A structure to represent an undirected weighted graph */
struct Graph {
	struct Edge* edge;  // array of edges
};
 
/* Initializes a new graph G(N,M,d) */
struct Graph* createGraph(long long int m) {
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
	
	graph->edge = (struct Edge*) malloc(m * sizeof(struct Edge));
 
	return graph;
}

/******************/
/* Adjacency List */
/******************/

// A structure to represent an adjacency list node
struct AdjListNode {
	long long int dest;
	long long int cost;
	struct AdjListNode* next;
};
 
// A structure to represent an adjacency list
struct AdjList {
	struct AdjListNode *head;  // pointer to head node of list
};
 
// A structure to represent a graph. A graph is an array of adjacency lists.
struct GraphNodes {
	struct AdjList* array;
};
 
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(long long int dest, long long int cost) {
	struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->cost = cost;
	newNode->next = NULL;
	return newNode;
}
 
// A utility function that creates a graph of N vertices
struct GraphNodes* createGraphNodes() {
	struct GraphNodes* graphNodes = (struct GraphNodes*) malloc(sizeof(struct GraphNodes));
 
	// Create an array of adjacency lists.  Size of array will be N
	graphNodes->array = (struct AdjList*) malloc(N * sizeof(struct AdjList));
 
	// Initialize each adjacency list as empty by making head as NULL
	long long int i;
	for (i = 0; i < N; i++) {
		graphNodes->array[i].head = NULL;
	}
 
	return graphNodes;
}
 
// Adds an edge to an undirected graph
void addEdge(struct GraphNodes* graphNodes, long long int src, long long int dest, long long int cost) {
	// Add an edge from src to dest.  A new node is added to the adjacency
	// list of src.  The node is added at the begining
	struct AdjListNode* newNode = newAdjListNode(dest, cost);
	newNode->next = graphNodes->array[src].head;
	graphNodes->array[src].head = newNode;
 
	// Since graph is undirected, add an edge from dest to src also
	newNode = newAdjListNode(src, cost);
	newNode->next = graphNodes->array[dest].head;
	graphNodes->array[dest].head = newNode;
}
 
// A utility function to print the adjacenncy list representation of graph
void printGraph(struct GraphNodes* graphNodes) {
	long long int v;
	for (v = 0; v < N; v++) {
		struct AdjListNode* pCrawl = graphNodes->array[v].head;
		printf("\n Adjacency list of vertex %lld\n head ", v);
		while (pCrawl) {
			printf("-> %lld(%lld)", pCrawl->dest, pCrawl->cost);
			pCrawl = pCrawl->next;
		}
		printf("\n");
	}
}

/*************/
/* HashTable */
/*************/

struct GraphCosts { /* table entry: */
	struct GraphCosts *next; /* next entry in chain */
	char *name; /* defined name of edge */
	long long int cost; /* cost of edge */
	long long int type; /* type of edge 
									0: NonMST
									1: critical for MST
									2: noncritical for MST */
};

static struct GraphCosts *graphCosts[MAX_M]; /* pointer table */

/* hash: form hash value for string s */
unsigned hash(char *s) {
	unsigned hashval;

	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % MAX_M;
}

/* lookup: look for s in graphCosts */
struct GraphCosts *lookup(char *s) {
	struct GraphCosts *np;

	for (np = graphCosts[hash(s)]; np != NULL; np = np->next) {
		if (strcmp(s, np->name) == 0) {
			return np; /* found */
		}
	}
	return NULL; /* not found */
}

/* make a duplicate of s */ 
char *strdupl(char *s) { 
	char *p;

	p = (char *) malloc(strlen(s)+1);   /* +1 for ’\0’ */
	if (p != NULL)
		strcpy(p, s);
	return p;
}

void changeType(char *s, int newType) {
	struct GraphCosts *np;

	for (np = graphCosts[hash(s)]; np != NULL; np = np->next) {
		if (strcmp(s, np->name) == 0) {
			np->type = newType; /* found */
		}
	}
}

/* install: put (name, cost) in graphCosts */
struct GraphCosts *install(char *name, long long int cost) {
	struct GraphCosts *np;
	unsigned hashval;

	np = (struct GraphCosts *)malloc(sizeof(*np));
	np->name = strdupl(name);
	np->cost = cost;
	np->type = 0;
	hashval = hash(name);
	np->next = graphCosts[hashval];
	graphCosts[hashval] = np;
   
	return np;
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

/*************/

struct QueueNode {
	long long int node;
	struct QueueNode* next;
};

struct QueueNode* head;

void enqueue(long long int id) {

	if (head == NULL) {   // Queue is empty
		head = (struct QueueNode*) malloc(sizeof(struct QueueNode));
		head->node = id;
		head->next = NULL;
		return;
	}

	struct QueueNode *curr, *prev;
	prev = head;
	curr = head->next;
	while (curr != NULL) {
		prev = prev->next;
		curr = curr->next;
	}

	curr = (struct QueueNode*) malloc(sizeof(struct QueueNode));
	curr->node = id;
	curr->next = NULL;
	prev->next = curr;

	return;
}

struct QueueNode* dequeue() {
	struct QueueNode* dequeued;
	dequeued = head;

	head = head->next;
	return dequeued;
}

int emptyQueue() {
	if (head == NULL) 
		return 1;
	else
		return 0;
}

void clearQueue() {
	struct QueueNode* n;
	
	while (head != NULL) {
		n = head;
		head = head->next;
		free(n);
	}
}

/* BFS algorithm in graph */
void getMaxInCycle(struct GraphNodes *g, long long int u, long long int v, long long int cost) {
	struct AdjListNode* curr;
	struct QueueNode* nextInQueue;
	
	long long int p[N][2];

	clearQueue();

	int m[N];
	long long int i;
	for (i = 0; i < N; i++) {
		m[i] = 0;   // Initialize all nodes as unvisited
	}
	m[u] = 1;  // Node u is under examination

	p[u][0] = -1;
	p[u][1] = 0;

	enqueue(u);

	while (head != NULL) {
		nextInQueue = dequeue();
		m[nextInQueue->node] = 2;  // Mark node as visited
		curr = g->array[nextInQueue->node].head;
		while (curr != NULL) {
			if (curr->dest == v) {
				p[curr->dest][0] = nextInQueue->node;
				if (curr->cost == cost) {
					p[curr->dest][1] = 1;
				} else
					p[curr->dest][1] = 0;
				break;
			}

			if ((m[curr->dest] == 0)) {
				enqueue(curr->dest);
				m[curr->dest] = 1;
				if (curr->cost == cost) {
					p[curr->dest][1] = 1;
				} else {
					p[curr->dest][1] = 0;
				}
				p[curr->dest][0] = nextInQueue->node;
			}
			curr = curr->next;
		}
	}

	char edgeName[14];
	int foundEqual = 0;

	while (v != -1) {
		if (p[v][1] == 1) {  // edge with same weight
			foundEqual = 1;
			if (v > p[v][0]) {
				sprintf(edgeName, "%lld-%lld", v, p[v][0]);
			} else {
				sprintf(edgeName, "%lld-%lld", p[v][0], v);
			}
			if (lookup(edgeName)->type != 2) {
				changeType(edgeName, 2);
				noncritical++;
				critical--;
			}
		}
		v = p[v][0];
	}

	if (foundEqual == 1) {
		nonMST--;
		noncritical++;
	}
}
 
/* Function to be used by qsort().
    It compares the weights of two edges. */
int ascendingCost(const void* a, const void* b) {
	struct Edge* a1 = (struct Edge*)a;
	struct Edge* b1 = (struct Edge*)b;
	
	if (a1->cost < b1->cost)
		return -1;
	if (a1->cost > b1->cost)
		return +1;
	return 0;
}

/* Kruskal's algorithm to determine Minimum Spanning Tree */
void KruskalMST(struct Graph* graph) {
	long long int e = 0;  
	long long int i = 0; 
	long long int v;
 
	/* Sorting the edges in ascending weight order */
	qsort(graph->edge, M, sizeof(graph->edge[0]), ascendingCost);

	i = 0;

	struct GraphNodes* graphNodes = createGraphNodes();

	struct subset *subsets = (struct subset*) malloc(N * sizeof(struct subset));
 
	/* Initialization of the union-find structure */
	for (v = 0; v < N; ++v) {
		subsets[v].parent = v;
		subsets[v].rank = 0;
	}

	while (i < M) {
		struct Edge next_edge = graph->edge[i++];

		long long int x = find(subsets, next_edge.src);
		long long int y = find(subsets, next_edge.dest);
 
		if (x != y) {  // next_edge does not form a cycle
			mst[e++] = next_edge;
			Union(subsets, x, y);
			
			struct GraphCosts *edge_cost;
			char edgeName[14];

			if (next_edge.src > next_edge.dest) 
				sprintf(edgeName, "%lld-%lld", next_edge.src, next_edge.dest);
			else
				sprintf(edgeName, "%lld-%lld", next_edge.dest, next_edge.src);
			edge_cost = lookup(edgeName);
			addEdge(graphNodes, next_edge.src, next_edge.dest, edge_cost->cost);
			critical++;
			changeType(edgeName, 1);
			nonMST--;
		} else {  // next_edge forms a cycle
			getMaxInCycle(graphNodes, next_edge.src, next_edge.dest, next_edge.cost);
		}
	}

	return;
}

int main(int argc, char *argv[]) {
	long long int i, pointA, pointB, cost;

	/* Read #cities, #roads and #road works: N M K */
	if(scanf("%lld %lld", &N, &M) != 2) {
		printf("\nError while reading file.\n");
		exit(1);
	}

	critical = 0;
	noncritical = 0;
	nonMST = M;

	struct Graph* graph = createGraph(M);
	char edgeName[14];

	/* Read road details */
	for (i = 0; i < M; i++) {
		scanf("%lld %lld %lld", &pointA, &pointB, &cost);
		graph->edge[i].src = pointA-1;
		graph->edge[i].dest = pointB-1;
		graph->edge[i].cost = cost;

		if (pointA > pointB) {
			sprintf(edgeName, "%lld-%lld", pointA-1, pointB-1);
			install(edgeName, cost);
		} else {
			sprintf(edgeName, "%lld-%lld", pointB-1, pointA-1);
			install(edgeName, cost);
		}
	}


	KruskalMST(graph);

	printf("%lld\n", critical);
	printf("%lld\n", nonMST);
	printf("%lld\n", noncritical);

	return 0;
}

