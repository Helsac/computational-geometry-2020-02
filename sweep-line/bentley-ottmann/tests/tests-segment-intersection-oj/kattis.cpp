#include <bits/stdc++.h>
using namespace std;
using ll = long long;

const double EPS = 1E-9;

struct pt {
    double x, y;
    bool operator<(const pt& p) const {
        return x < p.x - EPS || (abs(x - p.x) < EPS && y < p.y - EPS);
    }
    bool operator ==(const pt &p) const {
	return fabsl(x - p.x) <= EPS && fabsl(y - p.y) <= EPS;
    }
};

struct SegmentIntersection{
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
		if (fabs(z) > EPS)
		    a /= z, b /= z, c /= z;
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
};

double fixSign(double d) {
    ostringstream ss;
    ss << fixed << setprecision(2) << d;
    string str = ss.str();
    if (str == "-0.00") return 0.0;
    return d;
}

int main(){
	ios_base::sync_with_stdio(false);
	cin.tie(0); cout.tie(0);
	int n;
	cin >> n;
	SegmentIntersection inter;
	cout << setprecision(2) << fixed;
	while(n--){
		pt a, b, c, d;
		cin >> a.x >> a.y >> b.x >> b.y;
		cin >> c.x >> c.y >> d.x >> d.y;
		pt L, R;
		if (inter.intersect(a, b, c, d, L, R)){
			L.x = round(L.x * 100.0)/100.0;
			L.y = round(L.y * 100.0)/100.0;
			R.x = round(R.x * 100.0)/100.0;
			R.y = round(R.y * 100.0)/100.0;
			cout << fixSign(L.x) << ' ' << fixSign(L.y);
			if (!(L == R)) cout << ' ' << fixSign(R.x) << ' ' << fixSign(R.y);
			cout << '\n';
		} else cout << "none" << endl;
	}

	return 0;
}

