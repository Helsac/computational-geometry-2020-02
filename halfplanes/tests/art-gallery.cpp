#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <deque>
using namespace std;
typedef long long ll;

const double EPS = 1e-9, DINF = 1e6;

struct pt {  
	double x, y;
	pt(double x, double y) : x(x), y(y) {}
	pt(){}
	bool operator == (pt p) { return abs(x - p.x) <= EPS && abs(y - p.y) <= EPS; }
	pt operator + (pt p) { return pt(x + p.x, y + p.y); }
	pt operator - (pt p) { return pt(x - p.x, y - p.y); }
	pt operator * (double t) { return pt(x * t, y * t); }
	pt operator / (double t) { return pt(x / t, y / t); }
	double operator * (pt p) { return (x * p.x) + (y * p.y); }
	double operator % (pt p) { return (x * p.y) - (y * p.x); }
	double norm() { return sqrt(norm2()); }
	double norm2() { return (x*x) + (y*y); }
	pt unit(){ return *this / norm(); }
};

template <typename T> int sgn2(T x){ return x < 0 ? -1 : 1; }
struct ln {
	pt p,pq;
	ln(pt p, pt q):p(p),pq(q-p){}
	ln(){}
	bool has(pt r){return dist(r)<=EPS;}
	bool operator / (ln l){ return abs(pq.unit() % l.pq.unit()) <= EPS; } //parallel
	bool operator == (ln l){ return *this / l && has(l.p); }
	pt operator ^ (ln l) { 
		if (*this / l) return pt(DINF,DINF);
		pt r = l.p + l.pq * ((p - l.p) % pq / (l.pq % pq));
		return r;
	}
	int side(pt r){ return has(r) ? 0 : sgn2(pq % (r-p)); }
	pt proj(pt r){ return p + pq * ((r - p) * pq / pq.norm2()); }
	double dist(pt r){ return (r - proj(r)).norm(); }
};

struct pol {
	int n;vector<pt> p;
	pol(){}
	pol(vector<pt> _p) { p=_p; n=p.size(); }
	double area() { // signed area
		double t = 0.0;
		for(int i = 0; i < n; i++) t += p[i] % p[(i+1) % n];
		return t / 2.0;
	}
};

struct halfplane:public ln{
	double angle;
	halfplane(){}
	halfplane(pt a, pt b) { p = a; pq = b - a; angle = atan2(pq.y, pq.x); }
	bool operator < (halfplane b) const { return angle < b.angle; }
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

int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(0); cout.tie(0);
	int tc;
	cin >> tc;
	cout << setprecision(2) << fixed;
	while(tc--){
		int n;
		cin >> n;
		vector<pt> pts(n);
		for(int i = 0; i<n; i++) cin >> pts[i].x >> pts[i].y;
		pol p(pts);
		if (p.area() < 0.0) reverse(pts.begin(), pts.end());
		vector<halfplane> v;
		for(int i = 0; i<n; i++) v.push_back(halfplane(pts[i], pts[(i+1)%n]));
		vector<pt> ans = hp_intersection(v);
		p = pol(ans);
		cout << p.area() << '\n';
	}
}
