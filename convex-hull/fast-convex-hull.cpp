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
	reverse(lower.begin(), lower.end()); //revierto el orden del L_lower
	upper.pop_back(); //Ultimo punto de upper es el primer punto de lower
	upper.insert(upper.end(), lower.begin(), lower.end()); //meto todo el L_lower a L_upper
	upper.pop_back(); //Ultimo punto de lower es el primer punto de upper
	return upper; //retorno el hull completo ahora almacenado en L_upper
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
	input(0);
	vector<pair<int,int>> chull = fast_chull();
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
