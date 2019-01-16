#include <iostream>
#include <cassert>
#include <queue>
using namespace std;

struct node {
	int excess;
	int height;
	int edges_begin;
	int edges_end; 
	int edges_cur;

};

struct edge {
	int vertex;
	int capacity; // residual capacity
	int mate;
	edge(int vertex, int capacity) : vertex(vertex), capacity(capacity) {}
};

void add_edge(int a, int b, int capacity, vector<vector<edge>> &G);
void init_graph(int n, int s, int t, vector<node> &V, vector<edge> &E, vector<vector<edge>> &G);
void init_flow(int n, int s, int t, vector<node> &V, vector<edge> &E, queue<int> &Q);
void init_flow(int n, int s, int t, vector<node> &V, vector<edge> &E);

void compute_flow_cpu(int n, int m, int s, int t, vector<vector<edge>> &G);
void compute_flow_gpu_on_cpu(int n, int m, int s, int t, vector<vector<edge>> &G);
void compute_flow_gpu(int n, int m, int s, int t, vector<vector<edge>> &G);

