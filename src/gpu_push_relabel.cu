#include <stdio.h>
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true) {
   if (code != cudaSuccess) {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}
#include <iostream>
#include "flow.h"
using namespace std;

__device__ void relabel_gpu(node & u, int h) {
	u.height = h+1;
}

__device__ void push_gpu(node & u, node & v, edge & e, edge *E) {
	edge & back  = E[e.mate];
	int d = min(u.excess, e.capacity);
	atomicSub(&e.capacity,d);
	atomicAdd(&back.capacity,d);
	atomicSub(&u.excess,d);
	atomicAdd(&v.excess,d);
	// u.excess -= d;
	// v.excess += d;
	// e.capacity -= d;
	// back.capacity += d;

}

__global__ void push_relabel_kernel(int n, int s, int t, node *V, edge *E) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i == 0 || i > n || i == s || i == t) {
		return;
	}
	node &u = V[i];
	
	int CYCLE = 500;
	for (int z = 0; z < CYCLE; z++) {
		if (u.excess == 0 || u.height >= n) {
			continue;
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
			push_gpu(u,v,e,E);
		} else {
			relabel_gpu(u,v.height);
		}
	}
	
}

void global_relabeling_cpu(int s, int t, vector<node> &V, vector<edge> &E, int & excess_sum) {
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

void cancel_bad_flow_cpu(int n, int s, int t, vector<node> &V, vector<edge> &E) {
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

void compute_flow_gpu(int n, int s, int t, vector<node> &V, vector<edge> &E) {
	node *dev_V;
	edge *dev_E;
	gpuErrchk(cudaMalloc(&dev_V, sizeof(node)*(n+1)));
	gpuErrchk(cudaMalloc(&dev_E, sizeof(edge)*E.size()));

	gpuErrchk(cudaMemcpy(dev_V, V.data(), sizeof(node)*(n+1), cudaMemcpyHostToDevice));
	gpuErrchk(cudaMemcpy(dev_E, E.data(), sizeof(edge)*E.size(), cudaMemcpyHostToDevice));


	int excess_sum = -1;
	while (excess_sum != 0) {
		gpuErrchk(cudaMemcpy(dev_V, V.data(), sizeof(node)*(n+1), cudaMemcpyHostToDevice));

		push_relabel_kernel<<<1,1024>>>(n,s,t,dev_V,dev_E);

		gpuErrchk(cudaMemcpy(V.data(), dev_V, sizeof(node)*(n+1), cudaMemcpyDeviceToHost));
		gpuErrchk(cudaMemcpy(E.data(), dev_E, sizeof(edge)*E.size(), cudaMemcpyDeviceToHost));

		for (int i = 1; i <= n; i++) {
			V[i].height = 2*n+1;
		}
		cancel_bad_flow_cpu(n,s,t,V,E);
		global_relabeling_cpu(s,t,V,E,excess_sum);
		//cout << excess_sum << endl;
	}
}	

void compute_flow_gpu(int n, int m, int s, int t, vector<vector<edge>> &G) {
	vector<node> V;
	vector<edge> E;
	init_graph(n,s,t,V,E,G);
	init_flow(n,s,t,V,E);
	compute_flow_gpu(n,s,t,V,E);
	cout << "Flow : " << V[t].excess << " gpu" << endl;
}
