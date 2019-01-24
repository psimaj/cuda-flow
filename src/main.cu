/*
Zalozenia do sieci przeplywowej
	brak auto krawedzi
	miedzy kazda para wierzcholkow co najwyzej jedna skierowana krawedz
*/

#include <iostream>
#include <vector>
#include "flow.h"
using namespace std;

vector<vector<edge>> G;

int main() {
	ios_base::sync_with_stdio(false);
	cudaSetDevice(3);
	int n,m,s,t;
	cin >> n >> m >> s >> t;
	G.resize(n+1);
	for (int i = 0; i < m; i++) {
		int a,b,c;
		cin >> a >> b >> c;
		add_edge(a,b,c,G);
	}
	compute_flow_gpu(n,m,s,t,G);
	compute_flow_cpu(n,m,s,t,G);
	compute_flow_gpu_on_cpu(n,m,s,t,G);
	
}
