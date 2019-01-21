#include <iostream>
#include "flow.h"
#include <chrono>
using namespace std;

int relabel_counter = 0;

void relabel(node & u, int n, vector<node> &V, vector<edge> &E) {
	int h = 2*n+1;
	for (int i = u.edges_begin; i < u.edges_end; i++) {
		edge & e = E[i];
		if (e.capacity > 0) {
			h = min(h, V[e.vertex].height);
		}
	}
	u.height = h+1;
	relabel_counter++;
}

void push(node & u, node & v, edge & e, int s, int t, vector<edge> &E, queue<int> &Q) {
	edge & back  = E[e.mate];
	int d = min(u.excess, e.capacity);
	u.excess -= d;
	v.excess += d;
	e.capacity -= d;
	back.capacity += d;
	if (e.vertex != s && e.vertex != t && v.excess == d) {
		Q.push(e.vertex);
	}
}

void discharge(node & u, int n, int s, int t, vector<node> &V, vector<edge> &E, queue<int> &Q) {
	while (u.excess > 0) {
		if (u.edges_cur == u.edges_end) {
			relabel(u,n,V,E);
			u.edges_cur = u.edges_begin;
			continue;
		}
		edge & e = E[u.edges_cur];
		node & v = V[e.vertex];
		if (e.capacity == 0 || u.height != v.height + 1) {
			u.edges_cur++;
		} else {
			push(u,v,e,s,t,E,Q);
		}
	}
}

void global_relabeling(int start, int start_h, vector<node> &V, vector<edge> &E) {
	queue<int> q;
	V[start].height = start_h;
	q.push(start);
	while (!q.empty()) {
		int i = q.front();
		q.pop();
		node & u = V[i];
		for (int i = u.edges_begin; i < u.edges_end; i++) {
			edge & e = E[i];
			edge & back = E[e.mate];
			node & v = V[e.vertex];
			if (back.capacity > 0 && v.height > u.height + 1) {
				v.height = u.height + 1;
				q.push(e.vertex);
			}
		}
	}
}

void compute_flow(int n, int s, int t, vector<node> &V, vector<edge> &E, queue<int> &Q) {
	while (!Q.empty()) {
		int u = Q.front();
		Q.pop();
		discharge(V[u],n,s,t,V,E,Q);
		if (relabel_counter >= n) {
			for (int i = 1; i <= n; i++) {
				V[i].height = 2*n+1;
			}
			global_relabeling(t,0,V,E);
			global_relabeling(s,n,V,E);
			relabel_counter = 0;
		}
	}	
}	

void compute_flow_cpu(int n, int m, int s, int t, vector<vector<edge>> &G) {
	vector<node> V;
	vector<edge> E;
	queue<int> Q;
	init_graph(n,s,t,V,E,G);
	init_flow(n,s,t,V,E,Q);
	auto start = chrono::steady_clock::now();
	compute_flow(n,s,t,V,E,Q);
	auto stop = chrono::steady_clock::now();
    cout << "Execution time: " << chrono::duration_cast<chrono::microseconds>(stop-start).count() << " us" << endl;
	cout << "Flow : " << V[t].excess << " cpu" << endl;

}

