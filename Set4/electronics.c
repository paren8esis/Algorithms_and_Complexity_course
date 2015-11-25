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

/* The initial implementation of Dijkstra's shortest paths algorithm
	was taken from Rosetta Code: http://rosettacode.org/wiki/Dijkstra%27s_algorithm#C */

int L, W;
int startx, starty, finishx, finishy;

typedef struct node_t node_t, *heap_t;

struct node_t {
	node_t *via;	/* where previous node is in shortest path */
	long long int cost;	/* cost of node */
	long int name;	/* the, er, name */
	int horizontal; /* 1 if previous move was horizontal, 0 otherwise */
	long int heap_idx;	/* link to heap position for updating distance */
};


/* Priority queue */

heap_t *heap;
long int heap_len;
 
/* Inserts an element into the priority queue in the right place */
void set_dist(node_t *nd, node_t *via, long long int d) {
	long int i, j;
 
	/* already knew better path */
	if (nd->via && d >= nd->cost) return;
 
	/* find existing heap entry, or create a new one */
	nd->cost = d;
	nd->via = via;
 
	i = nd->heap_idx;
	if (!i) i = ++heap_len;
 
	/* upheap */
	for (; i > 1 && nd->cost < heap[j = i/2]->cost; i = j)
		(heap[i] = heap[j])->heap_idx = i;
 
	heap[i] = nd;
	nd->heap_idx = i;
}

/* Pops and returns an element from the priority queue */
node_t * pop_queue() {
	node_t *nd, *tmp;
	long int i, j;
 
	if (!heap_len) return 0;
 
	/* remove leading element, pull tail element there and downheap */
	nd = heap[1];
	tmp = heap[heap_len--];
 
	for (i = 1; i < heap_len && (j = i * 2) <= heap_len; i = j) {
		if (j < heap_len && heap[j]->cost > heap[j+1]->cost) j++;
 
		if (heap[j]->cost >= tmp->cost) break;
		(heap[i] = heap[j])->heap_idx = i;
	}
 
	heap[i] = tmp;
	tmp->heap_idx = i;
 
	return nd;
}


/* Dijkstra's shortest paths algorithm; 
	unreachable nodes will never make it into the queue */
void dijkstra(node_t *nodes) {
	node_t *lead, *neighbor;
	long int target_name = (finishx*W)+finishy;
	long int i;

	set_dist(&nodes[(startx*W)+starty], &nodes[(startx*W)+starty], 0);

	/* Note: from the starting node we search both horizontally and vertically */
	while ((lead = pop_queue())) {
		if (lead->name == target_name) {  // target reached
			set_dist(neighbor, lead, lead->cost + neighbor->cost);
			break;
		} else {
			if (lead->horizontal == 0) {  // Keep searching horizontally
				for (i = (lead->name % W)-1; i >= 0; i--) {	// Go left
					neighbor = &nodes[((lead->name / W)*W)+ i];
					if (neighbor->cost == 4000)  // Obstacle reached
						break;
					set_dist(neighbor, lead, lead->cost + neighbor->cost);
					neighbor->horizontal = 1;
				}
				for (i = (lead->name % W)+1; i < W; i++) {		// Go right
					neighbor = &nodes[((lead->name / W)*W)+ i];
					if (neighbor->cost == 4000)  // Obstacle reached
						break;
					set_dist(neighbor, lead, lead->cost + neighbor->cost);
					neighbor->horizontal = 1;
				}
			}
			if ((lead->horizontal == 1) || ((lead->name / W == startx) && (lead->name % W == starty))) {  // Keep searching vertically
				for (i = (lead->name / W)-1; i >= 0; i--) {  // Go up
					neighbor = &nodes[(W*i)+(lead->name % W)];
					if (neighbor->cost == 4000) 	// Obstacle reached
						break;
					set_dist(neighbor, lead, lead->cost + neighbor->cost);
					neighbor->horizontal = 0;
				}
				for (i = (lead->name / W)+1; i < L; i++) {  // Go down
					neighbor = &nodes[(W*i)+(lead->name % W)];
					if (neighbor->cost == 4000) 	// Obstacle reached
						break;
					set_dist(neighbor, lead, lead->cost + neighbor->cost);
					neighbor->horizontal = 0;
				}
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int i, j, point, foundOne = 0;
	long int id;

	// Read board dimensions: L W 
	if(scanf("%lld %lld", &L, &W) != 2) {
		printf("\nError while reading file.\n");
		exit(1);
	}

	node_t *nodes = (struct node_t*) malloc(L*W * sizeof(struct node_t));

	// Read the points of the board
	id = 0;
	for (i = 0; i < L; i++) {  // for every line
		for (j = 0; j < W; j++) {
			scanf("%d", &point);
			if (point == 0) {  // We found a 0 point
				if (foundOne == 0) {
					foundOne = 1;
					startx = i;
					starty = j;
				} else {
					finishx = i;
					finishy = j;
				}
			}
			nodes[id].name = id;
			nodes[id].cost = point;
			id++;
		}
	}

	heap = malloc(((L*W)+1) * sizeof(heap_t));
	heap_len = 0;
 
	/* Dijkstra's shortest paths algorithm */
	dijkstra(nodes);

	/* Result */
	printf("%lld\n", nodes[(finishx*W)+finishy].cost);

	return 0;
}
