/*
	Inclusión de punto en polígono
	Oscar Burga 20181A324

	Algoritmo para polígono convexo en O(N) preprocesamiento + O(logN) por consulta
	Algoritmo para polígono simple general en O(N) por consulta.
*/
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct pt {
	int x, y;
	pt operator-(const pt&q) const { return { x - q.x, y - q.y }; }
	int operator*(const pt&q) const { return (x * q.x) + (y * q.y); }
	int operator%(const pt&q) const { return (x * q.y) - (y * q.x); }
	bool operator<(const pt&q) { return (x < q.x) or (x == q.x && y < q.y); }
	bool on_segment(const pt&s, const pt&e) {
		return ((e-s) % (*this-s) == 0) && ((s-*this) * (e-*this) <= 0);
	}
};
template<typename T> int sign(T x) { return int(x > 0) - int(x < 0); }
struct poly{ 
	int n;
	vector<pt> pts;
	poly(int n) : n(n), pts(n) {};
	void print(){
		printf("%zu vertices\n", pts.size());
		for(pt&p: pts) printf("(%d, %d)\n", p.x, p.y);
		puts("");
		ofstream out("poly.txt");
		out << n << '\n';
		for(pt&p: pts) out << p.x << ' ' << p.y << '\n';
		out.close();
	}
	void normalize(){ //remove collinear points and set lower leftmost point as pt[0]
		int len = 1;
		vector<pt> ret = { pts[0] };
		ret.reserve(pts.size());
		for(int i = 1; i<=n; i++) {
			while (len > 1 && ((ret[len-1] - ret[len-2]) % (pts[i%n]-ret[len-1]) == 0)){
				ret.pop_back();
				len--;
			}
			ret.emplace_back(pts[i%n]), len++;
		}
		ret.pop_back(), len--;
		pts = move(ret);
		rotate(pts.begin(), min_element(pts.begin(), pts.end()), pts.end());
		n = pts.size();
	}
	bool contains(const pt&t){//convex only, assumes CLOCKWISE order
		pt vpt = t-pts[0];
		if (((pts[1]-pts[0]) % vpt > 0) || ((pts.back()-pts[0]) % vpt < 0)) return false;
		int l = 1, r = n-2;
		while (l < r) {
			int mid = l + (r-l+1)/2;
			pt pq = pts[mid] - pts[0];
			if (pq % vpt <= 0) l = mid;
			else r = mid-1;
		}
		return ((pts[l]-pts[0]) % vpt <= 0) && 
			((pts[(l+1)%n]-pts[l]) % (t-pts[l]) <= 0) &&
			((pts[(l+1)%n]-pts[0]) % (vpt) >= 0);
	}
	bool nccontains(pt t, bool strict = false){ //any polygon, can be CW or CCW
		bool inside = false;
		for(int i = 0; i<n; i++){
			pt& p = pts[i], q = pts[(i+1)%n];
			if (t.on_segment(p, q)) return !strict;
			inside ^= ((q.y>=t.y) - (p.y>=t.y)) * sign((p-t) % (q-t)) > 0;
		}
		return inside;
	}
};

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int genrand(int l, int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

int main(){
	int n;
	scanf("%d", &n);
	poly pol(n);
	for(int i = 0; i<n; i++) scanf("%d %d", &pol.pts[i].x, &pol.pts[i].y);
	pol.normalize();
	pol.print();
	int q = 500;
	ofstream out("pts.txt");
	out << q << '\n';
	while(q--){
		int x, y;
		x = genrand(0, 500), y = genrand(0, 500);
		out <<  x << ' ' << y << ' ' << int(pol.nccontains({x,y})) << '\n';
	}
	out.close();
	return 0;
}

