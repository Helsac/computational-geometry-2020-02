#include "seginter.hpp"
#include <vector>
#include <set>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <random>
#include <chrono>
SegmentIntersector utils;

struct seg{
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

struct event {
	double x;
	int tp, id1, id2; //type {0: inter, 1:open, -1:end} , seg_ids
	event(){}
	event(double x, int tp, int id1, int id2 = -1) : x(x), tp(tp), id1(id1), id2(id2) {}
	bool operator<(const event& e) const {
		if (abs(x - e.x) > EPS) return x < e.x;
		return tp > e.tp;
	}
};

vector<seg> lines;
set<seg> s;
multiset<event> pq;
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
		n = 100;
		lines.resize(n);
		for(int i = 0; i<n; i++){
			seg &t = lines[i];
			do {
			/*
				t.p.x = genrand_real(0.00, 1000.00);
				t.p.y = genrand_real(0.00, 1000.00);
				t.q.x = genrand_real(0.00, 1000.00);
				t.q.y = genrand_real(0.00, 1000.00);
			*/
				t.p.x = 1.00;
				t.p.y = genrand_real(0.00, 1000.00);
				t.q.x = 1000.00;
				t.q.y = genrand_real(0.00, 1000.00);
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
	ofstream out("segments.txt");
	out << lines.size() << '\n';
	for(auto &t: lines) {
		out << t.p.x << ' ' << t.p.y << ' ';
		out << t.q.x << ' ' << t.q.y << '\n';
	}
	out.close();
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
vector<pt> intersection_points(){
	vector<pt> out;
	out.reserve(lines.size() * lines.size());
	while(pq.size()){
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
		}
	}
	return out;
}

int main(){
	input(1);
	initialize();
	vector<pt> inters = intersection_points();
	ofstream out("inters.txt");
	cout << "Found " << inters.size() << " intersections." << endl;
	//para graficar con python
	out << inters.size() << '\n';
	out << setprecision(9) << fixed;
	for(auto&p: inters){
		out << p.x << ' ' << p.y << endl;
	}
	out.close();
	return 0;
}

