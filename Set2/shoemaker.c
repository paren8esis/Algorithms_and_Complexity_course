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

#define MAX_LEN 100

struct nlist {
	struct nlist *next;
	long long int x;
	long long int y;
	long long int sum;
};

long long int L;
long long int R;

long long int rights[MAX_LEN+1][2];
long long int lefts[MAX_LEN+1][2];
struct nlist *LCS[MAX_LEN+1][MAX_LEN+1];


/* Takes two integers as input and returns the maximum */
long long int max_sum(long long int x1, long long int x2) {
	if (x1 > x2) 
		return x1;
	else
		return x2;
}

/* Searches LCS[i][j] for a given struct node.
Returns the node if found, else returns NULL. */
struct nlist *lookup_node(long long int x, long long int y, struct nlist *arr) {

	while (arr != NULL) {
		if ((x == arr->x) && (y == arr->y)) {
			return arr;		/* found */
		}
		arr = arr->next;
	} 
	
	return NULL;		/* not found */
}

/* Inserts given struct node in LCS[i][j] */
void install_node(long long int inx, long long int iny, long long int insum, long long int i, long long int j) {
	struct nlist *np;
	
	if (LCS[i][j] == NULL) {
		np = (struct nlist *) malloc(sizeof(*np));
		np->x = inx;
		np->y = iny;
		np->sum = insum;
		LCS[i][j] = np;
	} else {
		np = LCS[i][j];
		np = (struct nlist *) malloc(sizeof(*np));
		np->x = inx;
		np->y = iny;
		np->sum = insum;
		np->next = LCS[i][j]->next;
		LCS[i][j]->next = np;
		
	}
}

/* The optimized LCS algorithm */
long long int LCS_algo(long long int i, long long int x, long long int j, long long int y) {
	struct nlist *foo1;
	struct nlist *foo2;
	long long int foo1sum, foo2sum;

	if ((i >= L) || (j >= R)) {
		return 0;
	} else {
		if (lefts[i][1] == rights[j][1]) {  /* Same type */
			if (x > y) {
				if ((foo1 = lookup_node(x-y, rights[j+1][0], LCS[i][j+1])) == NULL) {
					foo1sum = LCS_algo(i, x-y, j+1, rights[j+1][0]);
					install_node(x-y, rights[j+1][0], foo1sum, i, j+1);
					return y + foo1sum;
				} else
					return y + foo1->sum;
			} else if (x < y) {
				if ((foo1 = lookup_node(lefts[i+1][0], y-x, LCS[i+1][j])) == NULL) {
					foo1sum = LCS_algo(i+1, lefts[i+1][0], j, y-x);
					install_node(lefts[i+1][0], y-x, foo1sum, i+1, j);
					return x + foo1sum;
				} else
					return x + foo1->sum;
			} else {
				if ((foo1 = lookup_node(lefts[i+1][0], rights[j+1][0], LCS[i+1][j+1])) == NULL) {
					foo1sum = LCS_algo(i+1, lefts[i+1][0], j+1, rights[j+1][0]);
					install_node(lefts[i+1][0], rights[j+1][0], foo1sum, i+1, j+1);
					return x + foo1sum;
				} else
					return x + foo1->sum;
			}
		} else {  /* Different type */
			if ((foo1 = lookup_node(lefts[i+1][0], y, LCS[i+1][j])) == NULL) {
				foo1sum = LCS_algo(i+1, lefts[i+1][0], j, y);
				install_node(lefts[i+1][0], y, foo1sum, i+1, j);
				if ((foo2 = lookup_node(x, rights[j+1][0], LCS[i][j+1])) == NULL) {
					foo2sum = LCS_algo(i, x, j+1, rights[j+1][0]);
					install_node(x, rights[j+1][0], foo2sum, i, j+1);
					return max_sum(foo1sum, foo2sum);	
				} else 
					return max_sum(foo1sum, foo2->sum);
			} else {
					if ((foo2 = lookup_node(x, rights[j+1][0], LCS[i][j+1])) == NULL) {
						foo2sum = LCS_algo(i, x, j+1, rights[j+1][0]);
						install_node(x, rights[j+1][0], foo2sum, i, j+1);
						return max_sum(foo1->sum, foo2sum);
					} else
						return max_sum(foo1->sum, foo2->sum);
			}
		} 
				
	}
}

int main (int argc, char *argv[]) {
	long long int i, j, num, type, final;

	struct nlist *np;

	/* Read legth of both sequences: L R */
	if (scanf("%lld %lld", &L, &R) != 2) {
		printf("\nError while reading input.\n");
		exit(1);
	}

	/* Read the sequences */
	for (i = 0; i < L; i++) {
		scanf("%lld %lld", &num, &type);
		lefts[i][0] = num;
		lefts[i][1] = type;
    }
	lefts[L][0] = 0;
	lefts[L][1] = -1;

	for (i = 0; i < R; i++) {
		scanf("%lld %lld", &num, &type);
		rights[i][0] = num;
		rights[i][1] = type;
	}
	rights[R][0] = 0;
	rights[R][1] = -1;

	/* LCS algorithm */
	final = LCS_algo(0, lefts[0][0], 0, rights[0][0]);

	/* Print result */
	printf("%lld\n", final);

	return 0;
}
