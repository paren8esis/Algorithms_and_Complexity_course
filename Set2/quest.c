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

#define MAX_NM 1000
#define MAX_K 200

long long int M;
long long int N;
long long int K;

struct tuple {
	long long int score1;
	long long int score2;
	int visited;
};

long long int map[MAX_NM][MAX_NM];
struct tuple scores[MAX_NM+1][MAX_NM+1];

/* Takes as input two integers and returns the minimum */
long long int min_int(long long int x1, long long int x2) {
	if (x1 < x2)
		return x1;
	else
		return x2;
}

/* Takes as input the number of one player and returns the number of the other*/
int switch_player(int pl) {
	if (pl == 1)
		return 2;
	else
		return 1;
}

/* Function of the recursive algorithm that computes the best strategy */
struct tuple findSolution(long long int x, long long int y, int player) {
	long long int p, q, maximum = -1, nextPlayer, minimum;
	struct tuple newScore, maxScore;

	if (scores[x][y].visited == 0) {  /* We haven't calculated the scores */
		nextPlayer = switch_player(player);
		for (p = 0; p < min_int(K+1, N-x+1); p++) {
			for (q = 0; q < min_int(K+1, M-y+1); q++) {
				if ((p == 0) && (q == 0))
					continue;
				newScore = findSolution(x+p, y+q, nextPlayer);		
				if (newScore.score2 == maximum) {  /* Tie */
					if (newScore.score1 < minimum) {
						minimum = newScore.score1;
						maxScore.score1 = newScore.score2;
						maxScore.score2 = (newScore.score1) + map[x][y]; 
					}
				}		
				if (newScore.score2 > maximum) {  /* Not a tie */
					maxScore.score1 = newScore.score2;
					maxScore.score2 = (newScore.score1) + map[x][y];
					maximum = newScore.score2;
					minimum = newScore.score1;
				}
			}
		}
		maxScore.visited = 1;
		scores[x][y] = maxScore;
	}

	return scores[x][y];
} 

int main(int argc, char *argv[]) {
	long long int i, j, currX, currY, p, coins;
	struct coords *nextMove;

	/* Read size of map and step: N M K */
	if(scanf("%lld %lld %lld", &N, &M, &K) != 3) {
		printf("\nError while reading file.\n");
		exit(1);
	}

	/* Read the map */
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			scanf("%lld", &coins);
			map[i][j] = coins;
		}
   }

	map[0][0] = 0;

	/* Play the game! */
	struct tuple sol;

	for (i = 0; i <= N; i++) {
		for (j = 0; j <= M; j++) {
			scores[i][j].score1 = 0;
			scores[i][j].score2 = 0;
			if ((j == M) || (i == N))
				scores[i][j].visited = 1;
			else
				scores[i][j].visited = 0;
		}
	}

	sol = findSolution(0, 0, 1);

	printf("%lld %lld\n", sol.score1, sol.score2);

	return 0;
}
