#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const double eps = 1e-6;
const double inf = 1e9;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

struct pt{ 
	double x, y;
	pt operator-(const pt&q) { return { x - q.x, y - q.y }; }
	double operator %(const pt&q) { return (x * q.y) - (y * q.x); }
	bool valid() { return x != HUGE_VAL && y != HUGE_VAL && !isnan(x) && !isnan(y); }
	static pt null() { return { HUGE_VAL, HUGE_VAL }; }
	bool operator < (const pt& q) const { //define according to desired function
		return (x < q.x) || (x == q.x && y < q.y);
	}
	friend ostream& operator << (ostream&os, const pt&p){
		os << "(" << p.x << ", " << p.y << ")\n";
		return os;
	}
};

struct line { 
	double a, b, c;
	double f(const pt &p){ return (a * p.x) + (b * p.y) + c; }
	static line llim() { return { -1.0, 0.0, -inf}; }
	static line rlim() { return { 1.0, 0.0, -inf}; }
	static line ulim() { return { 0.0, 1.0, -inf}; }
	static line dlim() { return { 0.0, -1.0, -inf}; }
	friend ostream& operator << (ostream&os, const line&p){
		os << "(" << p.a << ", " << p.b << ", " << p.c << ")\n";
		return os;
	}
};

pt intersect(const line&p, const line&q){
	double den = (p.a * q.b) - (p.b * q.a);
	if (fabs(den) <= eps) return pt::null();
	return { ((p.b * q.c) - (q.b * p.c)) / den, ((p.c * q.a) - (q.c * p.a)) / den };
}


struct halfplane { //assume all constraints are given in the form ax + by + c <= 0
	vector<line> lines;
	void add_constraint(const line& t) { lines.push_back(t); }
	halfplane() : lines({ line::dlim(), line::llim(), line::rlim(), line::ulim() }) {}
	pt solve_brute() {
		int n = lines.size();
		vector<pt> pts;
		for(int i = 0; i<n; i++){
			for(int j = i+1; j<n; j++){
				cerr << "TESTING " << i << " " << j << endl;
				cerr << lines[i] << lines[j] << endl;
				pt cur = intersect(lines[i], lines[j]);
				if (!cur.valid()) continue; 
				bool can = true;
				int k;
				for(k = 0; k<n && can; k++) {
					can &= (lines[k].f(cur) < eps);
					if (!can){
						cerr << "CANT:\n";
						cerr << cur << lines[k] << lines[k].f(cur) << endl;
					}
				}
				if (can) pts.push_back(cur);
			}
		}
		if (pts.empty()) return pt::null();
		cerr << "NOT EMPTY" << endl;
		sort(pts.begin(), pts.end()); //convex hull of points
		function<bool(pt, pt)> equ = [&](pt p, pt q) { 
			return fabs(p.x-q.x) <= eps && fabs(p.y-q.y) <= eps; 
		};
		pts.erase(unique(pts.begin(), pts.end(), equ), pts.end());
		vector<pt> up = { pts[0] }, low = { pts[0] };
		int lu = 1, lo = 1;
		for(int i = 1; i<pts.size(); i++) {
			while (lu > 1 && (up[lu-1]-up[lu-2])%(pts[i]-up[lu-2]) > -eps) {
				up.pop_back();
				lu--;
			}
			up.emplace_back(pts[i]);
			lu++;
			while (lo > 1 && (low[lo-1]-low[lo-2])%(pts[i]-low[lo-2]) < eps) {
				low.pop_back();
				lo--;
			}
			low.emplace_back(pts[i]);
			lo++;
		}
		up.pop_back();
		up.insert(up.end(), low.rbegin(), low.rend());
		ofstream out("chull.txt");
		out << up.size() << '\n';
		for(pt p: up) out << p.x << ' ' << p.y << '\n';
		out.close();
		return up.front();
	}
};




int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(0); cout.tie(0);

	int n; cin >> n;
	halfplane hp;
	for(int i = 0; i<n; i++){
		line t;
		cin >> t.a >> t.b >> t.c;
		hp.add_constraint(t);
	}
	pt ans = hp.solve_brute();
	ofstream fout("points.txt");
	if (ans.valid()) fout << "1\n" <<  ans.x << ' ' << ans.y << '\n';
	else cout << "0\n";
	return 0;
}

