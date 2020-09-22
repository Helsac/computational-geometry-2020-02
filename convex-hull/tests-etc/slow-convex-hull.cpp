/*
	Oscar Burga - 20181A324
	Lunes 7 de Septiembre - Ejercicio de clase
	Implementación del algoritmo SlowConvexHull en O(N^3)
*/
#include <vector>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;
using ll = long long;

const int maxn = 105;

// n y el conjunto de puntos son variables globales
int n;
vector<pair<int,int>> pts;

//Funcion para obtener el vector pq dados 2 puntos p y q
pair<int,int> getvec(pair<int,int> p, pair<int,int> q){
	return { q.first - p.first, q.second - p.second };
}

//Producto cruz de vectores pq y pr para ver si r esta a la derecha de pq. 
bool cross(pair<int,int> pq, pair<int,int> pr){ 
	return ((pq.first * pr.second) - (pq.second * pr.first)) <= 0;
}

vector<pair<int,int>> convex_hull_edges(){ // algoritmo visto en clase, obtiene las aristas del chull
	vector<pair<int,int>> edges;
	for(int i = 0; i<n; i++){ 					// para todo punto p
		for(int j = 0; j<n; j++) if (i != j) { 			// para todo punto q, distinto de p
			pair<int,int> pq = getvec(pts[i], pts[j]); 	// obtengo vector pq
			bool edge_of_hull = true; 
			for(int k = 0; k<n && edge_of_hull; k++) {	// para todo punto r distinto de p y q
				if (k != i && k != j) {
					//obtengo vector pr y verifico si r esta a la derecha de pq
					pair<int,int> pr = getvec(pts[i], pts[k]); 
					edge_of_hull &= cross(pq, pr); 
				}
			}
			if (edge_of_hull) edges.push_back({i, j});
		}
	}
	return edges;	//retorno aristas, los puntos los obtengo en una funcion aparte
}

vector<pair<int,int>> get_chull_points(vector<pair<int,int>> edges){
	vector<pair<int,int>> chull;
	chull.push_back(pts[edges.back().first]);
	chull.push_back(pts[edges.back().second]);
	pair<int,int> cur = chull.back();
	//reconstruyo el chull en O(N^2) hasta que el punto inicial vuelva a aparecer.
	while(cur != chull.front()){ 
		for(pair<int,int>&p: edges) if (pts[p.first] == cur) {
			chull.push_back(pts[p.second]);
			cur = chull.back();
			break;
		}
	}
	chull.pop_back(); //eliminar punto repetido (es el mismo que el primer punto)
	return chull;
}

void input(bool isrand = false){ //input, generar aleatorio o leer de stdin
	if (isrand){ //genero 100 puntos aleatorios
		mt19937 rng(chrono::steady_clock::now().time_since_epoch().count()); // seed para random
		n = 100;
		pts.resize(n);
		vector<int> aux1(n), aux2(n);
		// lleno arreglos para X, Y con numeros del 1 al N
		iota(aux1.begin(), aux1.end(), 1); 
		iota(aux2.begin(), aux2.end(), 1);
		// los reordeno aleatoriamente
		shuffle(aux1.begin(), aux1.end(), rng);
		shuffle(aux2.begin(), aux2.end(), rng);
		// genero los puntos de esta forma para evitar puntos colineales
		for(int i = 0; i<n; i++) pts[i] = { aux1[i], aux2[i] };
	} else { //leo puntos del input
		scanf("%d", &n);
		pts.resize(n);
		for(int i = 0; i<n; i++){
			scanf("%d %d", &pts[i].first, &pts[i].second);
		}
	}
	// exportar puntos a un "points.txt" para graficar con python
	ofstream out("points.txt");
	out << pts.size() << '\n';
	for(pair<int,int>&p: pts) out << p.first << ' ' << p.second << '\n';
	out.close();
}


int main(){
	input(1);
	vector<pair<int,int>> chull_edges = convex_hull_edges();
	//Imprimo aristas
	puts("Aristas del Convex Hull:");
	for(auto&p: chull_edges){
		printf("(%d %d) to (%d %d)\n", pts[p.first].first, pts[p.first].second, pts[p.second].first, pts[p.second].second);
	}
	vector<pair<int,int>> chull = get_chull_points(chull_edges);
	//Imprimo puntos del chull
	puts("Puntos del Convex Hull:");
	for(pair<int,int>&p: chull){
		printf("(%d, %d)\n", p.first, p.second);
	}
	// exportar puntos del chull a un "chull.txt" para graficar con python
	ofstream out("chull.txt");
	out << chull.size() << '\n';
	for(pair<int,int> &p: chull) out << p.first << ' ' << p.second << '\n';
	return 0;
}
