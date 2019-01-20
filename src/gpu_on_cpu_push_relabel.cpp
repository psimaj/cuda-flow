#include <iostream>
#include "flow.h"
using namespace std;

void relabel(node & u, int h) {
	u.height = h+1;
}

void push(node & u, node & v, edge & e, vector<edge> &E) {
	edge & back  = E[e.mate];
	int d = min(u.excess, e.capacity);
	u.excess -= d;
	v.excess += d;
	e.capacity -= d;
	back.capacity += d;

}

void push_relabel(node &u, int n, int s, int t, vector<node> &V, vector<edge> &E) {
	if (u.excess == 0 || u.height >= n) {
		return;
	}
	int h = 2*n;
	int tmp = -1;
	for(int i = u.edges_begin; i < u.edges_end; i++) {
		edge & e = E[i];
		node & v = V[e.vertex];
		if (e.capacity == 0) {
			continue;
		}
		if (v.height < h) {
			h = v.height;
			tmp = i;
		}
	}
	edge & e = E[tmp];
	node & v = V[e.vertex];
	if (u.height == v.height + 1) {
		push(u,v,e,E);
	} else {
		relabel(u,v.height);
	}
}

void global_relabeling(int s, int t, vector<node> &V, vector<edge> &E, int & excess_sum) {
	excess_sum = 0;
	int start = t;
	int start_h = 0;
	queue<int> q;
	V[start].height = start_h;
	q.push(start);
	while (!q.empty()) {
		int i = q.front();
		q.pop();
		node & u = V[i];

		if (i != s && i != t) {
			excess_sum += u.excess;
		}

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

void cancel_bad_flow(int n, int s, int t, vector<node> &V, vector<edge> &E) {
	for (int i = 1; i <= n; i++) {
		node &u = V[i];
		for (int j = u.edges_begin; j < u.edges_end; j++) {
			edge &e = E[j];
			edge &back = E[e.mate];
			node &v = V[e.vertex];
			if (u.height > v.height + 1) {
				u.excess -= e.capacity;
				v.excess += e.capacity;
				back.capacity += e.capacity;
				e.capacity = 0;
			}
		}
	}
}

void compute_flow(int n, int s, int t, vector<node> &V, vector<edge> &E) {
	int excess_sum = -1;
	int CYCLE = 500;
	while (excess_sum != 0) {
		//cout << excess_sum << endl;
		for (int z = 0; z < CYCLE; z++) {
			for (int i = 1; i <= n; i++) {
				if (i != s && i != t)
					push_relabel(V[i],n,s,t,V,E);
			}
		}
		for (int i = 1; i <= n; i++) {
			V[i].height = 2*n+1;
		}
		cancel_bad_flow(n,s,t,V,E);
		global_relabeling(s,t,V,E,excess_sum);
		//cout << excess_sum << endl;
		
	}
}	

void compute_flow_gpu_on_cpu(int n, int m, int s, int t, vector<vector<edge>> &G) {
	vector<node> V;
	vector<edge> E;
	init_graph(n,s,t,V,E,G);
	init_flow(n,s,t,V,E);
	compute_flow(n,s,t,V,E);
	cout << "Flow : " << V[t].excess << " gpu on cpu" << endl;

}
