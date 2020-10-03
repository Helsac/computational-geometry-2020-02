#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <deque>
#include <fstream>
#include <random>
#include <chrono>
using namespace std;
typedef long long ll;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int genrand(int l, int r) { return uniform_int_distribution<int>(l, r)(rng); }

const double EPS = 1e-9, DINF = 1e6, PI = acos(-1);

template <typename T> int sgn2(T x) { return x < 0 ? -1 : 1; }
struct pt {  
	double x, y;
	pt(){}
	pt(double x, double y) : x(x), y(y) {}
	double norm() { return sqrt(norm2()); }
	double norm2() { return (x*x) + (y*y); }
	pt operator + (const pt& p) { return pt(x + p.x, y + p.y); }
	pt operator - (const pt& p) { return pt(x - p.x, y - p.y); }
	pt operator * (const double& t) { return pt(x * t, y * t); }
	pt operator / (const double& t) { return pt(x / t, y / t); }
	pt unit() { return *this / norm(); }
	pt rot(pt r) { return pt(*this % r, *this * r); }
	double operator * (const pt& p) { return (x * p.x) + (y * p.y); }
	double operator % (const pt& p) { return (x * p.y) - (y * p.x); }
	bool operator == (const pt& p) { return abs(x - p.x) <= EPS && abs(y - p.y) <= EPS; }
	bool operator < (const pt& p) const { return (x < p.x-EPS) || (fabs(x-p.x) <= EPS && y < p.y-EPS); }
};
const pt ccw90(1, 0), cw90(-1, 0);

struct line {
	pt p, pq, q;
	line() {}
	line(pt p, pt q) : p(p), q(p), pq(q - p) {}
	bool has(pt& r) { return dist(r) <= EPS; }
	bool operator / (line& l) { return abs(pq.unit() % l.pq.unit()) <= EPS; } //parallel
	bool operator == (line& l) { return *this / l && has(l.p); }
	pt operator ^ (line& l) { 
		if (*this / l) return pt(DINF,DINF);
		pt r = l.p + l.pq * ((p - l.p) % pq / (l.pq % pq));
		return r;
	}
	int side(pt& r){ return has(r) ? 0 : sgn2(pq % (r-p)); }
	pt proj(pt& r){ return p + pq * ((r - p) * pq / pq.norm2()); }
	double dist(pt& r){ return (r - proj(r)).norm(); }
};

struct halfplane : public line {
	double angle;
	halfplane(){}
	halfplane(pt& a, pt& b) { p = a; q = b; pq = b - a; angle = atan2(pq.y, pq.x); }
	bool operator < (const halfplane& b) const { return angle < b.angle; }
};

struct poly {
	int n;
	vector<pt> pts;
	poly() {}
	poly(const vector<pt>& _p) : n(int(_p.size())), pts(_p) {}
	double area() { // signed area
		double t = 0.0;
		for(int i = 0; i < n; i++) t += pts[i] % pts[(i+1) % n];
		return t / 2.0;
	}
};

vector<pt> hp_intersection(vector<halfplane> &v){
	deque<pt> p;
	deque<halfplane> dq;
	dq.push_back(v[0]);
	int len = 1;
	for(int i = 1; i<v.size(); i++){
		while(len > 1 && v[i].side(p.back()) < 0 ) p.pop_back(), dq.pop_back(), len--;
		while(len > 1 && v[i].side(p.front()) < 0) p.pop_front(), dq.pop_front(), len--;
		dq.push_back(v[i]), len++;
		if (abs(dq[len-1].pq % dq[len-2].pq) < EPS){
			dq.pop_back(), len--;
			if (p.size()) p.pop_back();
			if (dq[len-1].side(v[i].p) >= 0) dq.back() = v[i];
		}
		if (len > 1) p.push_back(dq[len-2] ^ dq[len-1]);
	}
	while(len > 1 && dq.front().side(p.back()) < 0) dq.pop_back(), p.pop_back(), len--;
	if (len <= 2) return vector<pt>();
	p.push_back(dq.back() ^ dq.front());
	return vector<pt>(p.begin(), p.end());
}

poly generate_convex_polygon(){
	ofstream fout("polysize.txt");
	fout << genrand(3, 6) << endl; fout.close();
	system("java convex.java < polysize.txt");
	ifstream fin("poly.txt");
	int sz; fin >> sz;
	vector<pt> pts(sz);
	for(int i = 0; i < sz; i++) fin >> pts[i].x >> pts[i].y; 
	fin.close();
	return poly(pts);
}

pair<bool, pt> pred(double mid, vector<halfplane> v){ 
	for(int i = 0;i<v.size(); i++){
		pt aux = v[i].pq.unit().rot(ccw90) * mid;
		v[i].p = v[i].p + aux;
		v[i].q = v[i].q + aux;
	}
	vector<pt> ans = hp_intersection(v);
	pt ptaux = ans.size() > 0 ? ans[0] : pt(-1,-1);
	return { ans.size() > 0 , ptaux };
}

struct timer{
	clock_t _START;
	timer(){_START = clock();}
	double time(){return (clock()-_START)/double(CLOCKS_PER_SEC);}
};

int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(0); cout.tie(0);
	int tc;
	cin >> tc;
	double total_time = 0;
	while(tc--) {
		cout << "Rem: " << tc << endl;
		poly p = generate_convex_polygon();
		timer tt;
		if (p.area() < 0.0) reverse(p.pts.begin(), p.pts.end());
		vector<halfplane> v;
		for(int i = 0; i<p.n; i++) v.push_back(halfplane(p.pts[i], p.pts[(i+1) % p.n]));
		rotate(v.begin(), min_element(v.begin(), v.end()), v.end());
		// pt distante
		double l = 0.00, r = 1000.00;
		for(int i = 0; i < 100; ++i) {
			double mid = (l+r)/2.0;
			if(pred(mid, v).first) l = mid;
			else r = mid-EPS;
		}
		pair<bool, pt> pt_ans = pred(l, v);
		total_time += tt.time();
		cout << tt.time() << '\n';
		ofstream fout("inter.txt");
		if(pt_ans.first) {
			cout << "Distancia = " << l << "\n";
			cout << "Punto = {" << pt_ans.second.x << ", " << pt_ans.second.y << "}\n";
			fout << "1\n" << pt_ans.second.x << ' ' << pt_ans.second.y << '\n';
			fout << l << '\n';
		} else cout << "Sin Respuesta\n";
	}
	cout << total_time << '\n';
}
