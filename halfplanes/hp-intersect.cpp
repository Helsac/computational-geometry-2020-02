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

const double EPS = 1e-9, DINF = 1e6;

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

struct line {
	pt p, pq;
	line() {}
	line(pt p, pt q) : p(p), pq(q - p) {}
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
	halfplane(pt& a, pt& b) { p = a; pq = b - a; angle = atan2(pq.y, pq.x); }
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
	sort(v.begin(), v.end());
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
	fout << genrand(10, 50) << endl; fout.close();
	system("java convex.java < polysize.txt");
	ifstream fin("poly.txt");
	int sz; fin >> sz;
	vector<pt> pts(sz);
	for(int i = 0; i < sz; i++) fin >> pts[i].x >> pts[i].y; 
	fin.close();
	return poly(pts);
}

poly generate_simple_polygon(){
	system("python non-convex.py");
	ifstream fin("poly.txt");
	int sz; fin >> sz;
	vector<pt> pts(sz);
	for(int i = 0; i<sz; i++) fin >> pts[i].x >> pts[i].y;
	fin.close();
	return poly(pts);
}

int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(0); cout.tie(0);
	int tc;
	cin >> tc;
	while(tc--) {
		poly p = generate_simple_polygon();
		if (p.area() < 0.0) reverse(p.pts.begin(), p.pts.end());
		vector<halfplane> v;
		for(int i = 0; i<p.n; i++) v.push_back(halfplane(p.pts[i], p.pts[(i+1) % p.n]));
		vector<pt> ans = hp_intersection(v);
		p = poly(ans);
		ans.erase(unique(ans.begin(), ans.end()), ans.end());
		ofstream fout("inter.txt");
		if (ans.empty()) {
			cout << "Intersection is empty\n";
			fout << "0\n";
		}
		else if (p.area() <= EPS) {
			cout << "Intersection is a ";
			if (ans.size() == 1) {
				cout << "point";
				fout << "1\n" << ans.front().x << ' ' << ans.front().y << '\n';
			} else {
				cout << "segment";
				fout << "2\n" << ans.front().x << ' ' << ans.front().y << '\n';
				fout << ans[1].x << ' ' << ans[1].y << '\n';
			}
			cout << '\n';
		} else {
			cout << "Intersection is a convex region\n";
			fout << ans.size() << '\n';
			for(pt &p: ans) fout << p.x << ' ' << p.y << '\n';
		}
	}
}
