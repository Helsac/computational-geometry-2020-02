/*
	Oscar Burga - 20181A324
	Algoritmo de Bentley-Ottmann para intersección de líneas en O((N+K)logN)
	La complejidad espacial es O(N+K). Puede reducirse a O(N) si no se desean guardar
	los puntos de intersección.
	Suposiciones:
	* No hay 2 segmentos que compartan algún extremo
	* No hay intersecciones de 3 o más líneas en un mismo punto.
	* No hay líneas verticales
	* No hay segmentos que se solapan (ie: todo par de segmentos paralelos no se intersecan).
	* No hay intersecciones en los extremos de los segmentos
	
*/

#include <cmath>
#include <algorithm>
#include <functional>
#include <vector>
#include <set>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <random>
#include <chrono>
using namespace std;
using ll = long long;

const double EPS = 1E-9;

struct pt { //Punto 
	double x, y;
	bool operator<(const pt& p) const {
		return x < p.x - EPS || (abs(x - p.x) < EPS && y < p.y - EPS);
	}
	bool operator ==(const pt &p) const {
		return fabsl(x - p.x) <= EPS && fabsl(y - p.y) <= EPS;
	}
};

struct SegmentIntersector{ //Estructura para intersecar segmentos
	struct line { 
		double a, b, c;
		line() {}
		line(pt p, pt q) {
			a = p.y - q.y;
			b = q.x - p.x;
			c = -a * p.x - b * p.y;
			norm();
		}
		void norm() {
			double z = sqrt(a * a + b * b);
			if (fabs(z) > EPS) a /= z, b /= z, c /= z;
		}
		double dist(pt p) const { return a * p.x + b * p.y + c; }
	};
	double det(double a, double b, double c, double d) { return a * d - b * c; }
	inline bool betw(double l, double r, double x) { 
		return min(l, r) <= x + EPS && x <= max(l, r) + EPS;
	} 
	inline bool intersect_1d(double a, double b, double c, double d) {
		if (a > b) swap(a, b);
		if (c > d) swap(c, d);
		return max(a, c) <= min(b, d) + EPS;
	}
	bool intersect(pt a, pt b, pt c, pt d, pt& left, pt& right) {
		if (!intersect_1d(a.x, b.x, c.x, d.x) || !intersect_1d(a.y, b.y, c.y, d.y)) return false;
		line m(a, b); line n(c, d);
		double zn = det(m.a, m.b, n.a, n.b);
		if (fabs(zn) < EPS) {
			if (abs(m.dist(c)) > EPS || abs(n.dist(a)) > EPS) return false;
			if (b < a) swap(a, b);
			if (d < c) swap(c, d);
			left = max(a, c);
			right = min(b, d);
			return true;
		} else {
			left.x = right.x = -det(m.c, m.b, n.c, n.b) / zn;
			left.y = right.y = -det(m.a, m.c, n.a, n.c) / zn;
			return betw(a.x, b.x, left.x) && betw(a.y, b.y, left.y) &&
				betw(c.x, d.x, left.x) && betw(c.y, d.y, left.y);
		}
	}
} utils;

struct seg{ //Segmento dentro del status
	pt p, q;
	int id;
	double left() { return min(p.x, q.x); }
	double right() { return max(p.x, q.x); }
	double get_y(double x) const {
		if (abs(p.x - q.x) < EPS) return p.y;
		return p.y + (q.y - p.y) * (x - p.x) / (q.x - p.x);
	}
	bool operator<(const seg& b) const {
		double x = max(min(p.x, q.x), min(b.p.x, b.q.x));
		return get_y(x) < b.get_y(x) - EPS;
	}
	void update_left(double x, double y){
		if (p.x < q.x - EPS) p.x = x, p.y = y;
		else q.x = x, q.y = y;
	}
};

struct event { //evento en la cola
	double x;
	int tp, id1, id2; //type {0: inter, 1:open, -1:end} , seg_ids
	event(){}
	event(double x, int tp, int id1, int id2 = -1) : x(x), tp(tp), id1(id1), id2(id2) {}
	bool operator<(const event& e) const {
		if (abs(x - e.x) > EPS) return x < e.x;
		return tp > e.tp;
	}
};

vector<seg> lines; //conjunto de segmentos
set<seg> s; //arbol binario del status
multiset<event> pq; //arbol binario para los eventos
vector<set<seg>::iterator> where; 

multiset<event>::iterator find_inter_event(double x){ 
	multiset<event>::iterator it = pq.lower_bound(event(x-EPS, 0, -1));
	if (it != pq.end() && (fabs(x - it->x) > EPS or it->tp != 0)) return pq.end();
	return it;
}

set<seg>::iterator previous(set<seg>::iterator it) {
	return it == s.begin() ? s.end() : prev(it);
}

void input(bool isrand = false){ 
	int n;
	if (isrand){ 
		mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
		function<double(double, double)> genrand_real = [&rng](double l, double r) {
			return uniform_real_distribution<double>(l, r)(rng);
		};
		double mxlen, mxc;
		cin >> n >> mxlen >> mxc;
		lines.resize(n);
		for(int i = 0; i<n; i++){
			seg &t = lines[i];
			double len = genrand_real(1.00, mxlen);
			do {
				#ifndef SAMEX
				t.p.x = genrand_real(0.00, mxc);
				t.p.y = genrand_real(0.00, mxc);
				t.q.x = genrand_real(t.p.x - len, t.p.x + len);
				t.q.y = genrand_real(t.p.y - len, t.p.y + len);
				#else
				t.p.x = 1.00;
				t.p.y = genrand_real(0.00, mxc);
				t.q.x = mxc;
				t.q.y = genrand_real(0.00, mxc);
				#endif
				t.id = i;
			} while (fabs(t.p.x - t.q.x) < 1.0 || fabs(t.p.y - t.q.y) < 1.0);
		}
	} else { 
		cin >> n;
		lines.resize(n);
		for(int i = 0; i<n; i++){
			seg &t = lines[i];
			cin >> t.p.x >> t.p.y >> t.q.x >> t.q.y;
			t.id = i;
		}
	}
	#ifdef FOUT
	ofstream out("segments.txt");
	out.tie(0);
	out << lines.size() << '\n';
	out << setprecision(9) << fixed;
	for(auto &t: lines) {
		out << t.p.x << ' ' << t.p.y << ' ';
		out << t.q.x << ' ' << t.q.y << '\n';
	}
	out.close();
	#endif
}

void initialize(){
	where.resize(lines.size());
	for (int i = 0; i < lines.size(); i++){
		seg &t = lines[i];
		pq.insert(event(t.left(), 1, i));
		pq.insert(event(t.right(), -1, i));
	}
}

bool inter(const seg&a, const seg&b, pt& l, pt&r){
	return utils.intersect(a.p, a.q, b.p, b.q, l, r);
}

//función principal, retorna un vector con los puntos de intersección
vector<pt> intersection_points(){
	vector<pt> out;
	int iters = 0;
	while(pq.size()){
		iters++;
		assert(iters < int(1e7));
		event e = *pq.begin(); pq.erase(pq.begin());		
		pt auxl, auxr;
		if (e.tp == 1){
			seg cur = lines[e.id1];
			auto it = where[e.id1] = s.insert(cur).first;
			auto below = previous(it), above = next(it);
			if (below != s.end() && above != s.end() && inter(*below, *above, auxl, auxr)){
				auto rem_p = find_inter_event(auxl.x);
				if (rem_p != pq.end()) pq.erase(rem_p);
			}
			if (above != s.end() && inter(*it, *above, auxl, auxr)) {
				pq.insert(event(auxl.x, 0, it->id, above->id));
			}
			if (below != s.end() && inter(*below, *it, auxl, auxr)){
				assert(below->id != it->id);
				pq.insert(event(auxl.x, 0, below->id, it->id));
			}
		
		} else if (e.tp == -1) {
			seg cur = lines[e.id1];
			auto it = where[e.id1];
			auto below = previous(it), above = next(it);
			s.erase(it);
			where[e.id1] = s.end();
			if (below != s.end() && above != s.end() && inter(*below, *above, auxl, auxr)){
				pq.insert(event(auxl.x, 0, below->id, above->id));
			}
		} else { //intersection
			auto it1 = where[e.id1], it2 = where[e.id2]; // cur1 below cur2, have to swap them
			seg cur1 = *it1, cur2 = *it2, copy_cur1 = cur1, copy_cur2 = cur2;
			inter(cur1, cur2, auxl, auxr); out.push_back(auxl);
			s.erase(it1), s.erase(it2);
			cur1.update_left(auxl.x+EPS, auxl.y+EPS);
			cur2.update_left(auxl.x+EPS, auxl.y-EPS);
			assert(cur2<cur1); //make sure cur2 is now below cur1
			it1 = where[cur1.id] = s.insert(cur1).first;
			it2 = where[cur2.id] = s.insert(cur2).first;
			it1 = next(it1); //highest
			it2 = previous(it2); //lowest
			// if highest intersects with old_above, remove cuz no longer neighbours
			if (it1 != s.end() && inter(copy_cur2, *it1, auxl, auxr)) { 
				auto rem_p = find_inter_event(auxl.x);
				if (rem_p != pq.end()) pq.erase(rem_p);
			}
			//if lowest intersects with old_below, remove cuz no longer neighbours
			if (it2 != s.end() && inter(*it2, copy_cur1, auxl, auxr)){
				auto rem_p = find_inter_event(auxl.x);
				if (rem_p != pq.end()) pq.erase(rem_p);
			}
			//if highest intersects with new_above, insert into pq
			if (it1 != s.end() && inter(cur1, *it1, auxl, auxr)){
				pq.insert(event(auxl.x, 0, cur1.id, it1->id));
			}
			//if lowest intersects with new_below, insert into pq
			if (it2 != s.end() && inter(*it2, cur2, auxl, auxr)){
				pq.insert(event(auxl.x, 0, it2->id, cur2.id));
			}
			assert(out.size() < int(1e6));
		}
	}
	return out;
}

//Igual que intersection_points, pero solo cuenta las intersecciones, no las guarda.
int intersection_points_count(){
	int out = 0;
	int iters = 0;
	int mxsize = 0;
	while(pq.size()){
		mxsize = max(int(pq.size()), mxsize);
		iters++;
		assert(iters < int(1e7));
		event e = *pq.begin(); pq.erase(pq.begin());		
		pt auxl, auxr;
		if (e.tp == 1){
			seg cur = lines[e.id1];
			auto it = where[e.id1] = s.insert(cur).first;
			auto below = previous(it), above = next(it);
			if (below != s.end() && above != s.end() && inter(*below, *above, auxl, auxr)){
				auto rem_p = find_inter_event(auxl.x);
				if (rem_p != pq.end()) pq.erase(rem_p);
			}
			if (above != s.end() && inter(*it, *above, auxl, auxr)) {
				pq.insert(event(auxl.x, 0, it->id, above->id));
			}
			if (below != s.end() && inter(*below, *it, auxl, auxr)){
				assert(below->id != it->id);
				pq.insert(event(auxl.x, 0, below->id, it->id));
			}
		
		} else if (e.tp == -1) {
			seg cur = lines[e.id1];
			auto it = where[e.id1];
			auto below = previous(it), above = next(it);
			s.erase(it);
			where[e.id1] = s.end();
			if (below != s.end() && above != s.end() && inter(*below, *above, auxl, auxr)){
				pq.insert(event(auxl.x, 0, below->id, above->id));
			}
		} else { //intersection
			auto it1 = where[e.id1], it2 = where[e.id2]; // cur1 below cur2, have to swap them
			seg cur1 = *it1, cur2 = *it2, copy_cur1 = cur1, copy_cur2 = cur2;
			inter(cur1, cur2, auxl, auxr); 
			out++;
			s.erase(it1), s.erase(it2);
			cur1.update_left(auxl.x+EPS, auxl.y+EPS);
			cur2.update_left(auxl.x+EPS, auxl.y-EPS);
			assert(cur2<cur1); //make sure cur2 is now below cur1
			it1 = where[cur1.id] = s.insert(cur1).first;
			it2 = where[cur2.id] = s.insert(cur2).first;
			it1 = next(it1); //highest
			it2 = previous(it2); //lowest
			// if highest intersects with old_above, remove cuz no longer neighbours
			if (it1 != s.end() && inter(copy_cur2, *it1, auxl, auxr)) { 
				auto rem_p = find_inter_event(auxl.x);
				if (rem_p != pq.end()) pq.erase(rem_p);
			}
			//if lowest intersects with old_below, remove cuz no longer neighbours
			if (it2 != s.end() && inter(*it2, copy_cur1, auxl, auxr)){
				auto rem_p = find_inter_event(auxl.x);
				if (rem_p != pq.end()) pq.erase(rem_p);
			}
			//if highest intersects with new_above, insert into pq
			if (it1 != s.end() && inter(cur1, *it1, auxl, auxr)){
				pq.insert(event(auxl.x, 0, cur1.id, it1->id));
			}
			//if lowest intersects with new_below, insert into pq
			if (it2 != s.end() && inter(*it2, cur2, auxl, auxr)){
				pq.insert(event(auxl.x, 0, it2->id, cur2.id));
			}
		}
	}
	cout << mxsize << endl;
	return out;
}
int main(){
	ios_base::sync_with_stdio(false);
	input(1);
	initialize();
	//para graficar con python
	#ifdef FOUT
	vector<pt> inters = intersection_points();
	cout << "Found " << inters.size() << " intersections." << endl;
	ofstream out("inters.txt");
	out.tie(0);
	out << inters.size() << '\n';
	out << setprecision(9) << fixed;
	for(auto&p: inters){
		out << p.x << ' ' << p.y << endl;
	}
	out.close();
	#else
	int cnt = intersection_points_count();
	cout << "Found " << cnt << " intersections." << endl;
	#endif
	return 0;
}
