#include "flow.h"
#include <iostream>
#include <cassert>
#include <queue>
using namespace std;

void add_edge(int a, int b, int capacity, vector<vector<edge>> &G) {
	assert(a != b);
	G[a].emplace_back(b,capacity);
	G[b].emplace_back(a,0);
	G[a].back().mate = G[b].size() - 1;
	G[b].back().mate = G[a].size() - 1;
}

void init_graph(int n, int s, int t, vector<node> &V, vector<edge> &E, vector<vector<edge>> &G) {
	V.resize(n+1);
	for (int i = 1; i <= n; i++) {
		V[i].excess = 0;
		V[i].height = 0;
		V[i].edges_begin = V[i].edges_cur = E.size();
		for (auto &e: G[i]) {
			E.push_back(e);
			if (i < e.vertex) {
				e.mate = E.size() - 1; 
			}else {
				int mate_pos = G[e.vertex][e.mate].mate;
				E.back().mate = mate_pos;
				E[mate_pos].mate = E.size() - 1;
			}
		}
		V[i].edges_end = E.size();
	}
}

void init_flow(int n, int s, int t, vector<node> &V, vector<edge> &E, queue<int> &Q) {
	V[s].height = n;
	for (int i = V[s].edges_begin; i < V[s].edges_end; i++) {
		edge & e = E[i];
		edge & back = E[E[i].mate];
		V[e.vertex].excess = back.capacity = e.capacity;
		e.capacity = 0;
		if (e.vertex != t) {
			Q.push(e.vertex);
		}
	}
}

void init_flow(int n, int s, int t, vector<node> &V, vector<edge> &E) {
	queue<int> Q;
	init_flow(n,s,t,V,E,Q);
}