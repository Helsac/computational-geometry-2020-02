/*
	Oscar Burga - 20181A324
	Lunes 14 de Septiembre - Actividad de clase
	Implementación del algoritmo ConvexHull en O(NlogN)
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

//Producto cruz de vectores
int cross(pair<int,int> u, pair<int,int> v){ 
	return (u.first * v.second) - (u.second * v.first);
}

//Comparador para determinar si tres puntos giran a la izquierda
bool uppercmp(pair<int,int> l, pair<int,int> mid, pair<int,int> r){
	pair<int,int> u = getvec(mid, l), v = getvec(mid, r);
	return cross(u, v) <= 0;
}
//Comparador para determinar si tres puntos giran a la derecha
bool lowercmp(pair<int,int> l, pair<int,int> mid, pair<int,int> r){
	pair<int,int> u = getvec(mid, l), v = getvec(mid, r);
	return cross(u, v) >= 0;
}

//Funcion principal, practicamente identico al algoritmo presentado en clase
//La única variación que hice es evaluar el L_lower en la misma pasada con un comparador distinto
//Me pareció más sencillo de esta forma
vector<pair<int,int>> fast_chull(){
	sort(pts.begin(), pts.end());
	pts.erase(unique(pts.begin(), pts.end()), pts.end());
	n = pts.size();
	if (n == 1) return { pts[0] };
	vector<pair<int,int>> upper = { pts[0] }; //L_upper
	vector<pair<int,int>> lower = { pts[0] }; //L_lower
	int lu = 1, lo = 1; //lu y lo son el tamaño actual del L_upper y L_lower respectivamente
	for(int i = 1; i<n; i++){
		//Mientras tengo por lo menos 2 puntos en mi hull superior y tengo un giro a la izq
		while (lu >= 2 && uppercmp(upper[lu-2], upper[lu-1], pts[i])){
			upper.pop_back(); //elimino el punto del medio
			lu--;
		}
		upper.emplace_back(pts[i]);
		lu++;
		//Mientras tengo por lo menos 2 puntos en mi hull inferior y tengo un giro a la der
		while (lo >= 2 && lowercmp(lower[lo-2], lower[lo-1], pts[i])){
			lower.pop_back(); //elimino el punto del medio
			lo--;
		}
		lower.emplace_back(pts[i]);
		lo++;
	}
	reverse(upper.begin(), upper.end()); 
	upper.pop_back(); 
	upper.insert(upper.end(), lower.begin(), lower.end()); 
	upper.pop_back(); 
	return upper; 
}


int main(){
	while(true){
		scanf("%d", &n);
		if (n == 0) return 0;
		pts.resize(n);
		for(int i = 0; i<n; i++){
			scanf("%d %d", &pts[i].first, &pts[i].second);
		}
		vector<pair<int,int>> chull = fast_chull();
		printf("%zu\n", chull.size());
		for(pair<int,int>&p: chull){
			printf("%d %d\n", p.first, p.second);
		}
	}
	return 0;
}
