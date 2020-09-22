#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template<typename T> struct pt{ 
	T x, y;
	pt operator+(const pt& q){ return { x + q.x, y + q.y }; }
	pt operator-(const pt& q){ return { x - q.x, y - q.y }; }
	pt operator*(const T k){ return { x * k, y * k }; }
	T operator%(const pt& q) { return (x * q.y) - (y * q.x); }
	T len2() { return x*x + y*y; }
	T len() { return sqrt(len2()); }
	bool operator<(const pt&q) { return (x < q.x) || (x == q.x && y < q.y); }
	bool operator==(const pt&q) { return x == q.x && y == q.y; }
};

template<typename T> 
vector<pt<T>> convex_hull(vector<pt<T>>& pts){ 
	sort(pts.begin(), pts.end());
	pts.erase(unique(pts.begin(), pts.end()), pts.end());
	int m = pts.size();
	if (m == 1) return { pts[0] };
	vector<pt<T>> upper, lower;
	int lu = 0, lo = 0; 
	for(int i = 0; i<m; i++){
		while (lu >= 2 && (upper[lu-2] - upper[lu-1]) % (pts[i] - upper[lu-1]) <= 0) {
			upper.pop_back(); 
			lu--;
		}
		upper.emplace_back(pts[i]), lu++;
		while (lo >= 2 && (lower[lo-2] - lower[lo-1]) % (pts[i] - lower[lo-1]) >= 0){
			lower.pop_back(); 
			lo--;
		}
		lower.emplace_back(pts[i]), lo++;
	}
	reverse(upper.begin(), upper.end()); 
	lower.pop_back();
	lower.insert(lower.end(), upper.begin(), upper.end()); 
	lower.pop_back(); 
	return lower; 
}

int n;
vector<pt<int>> pts;
int main(){
	int tc;
	scanf("%d", &tc);
	for(int t = 1; t<=tc; t++){
		scanf("%d", &n);
		pts.resize(n);
		for(int i = 0; i<n; i++){
			scanf("%d %d", &pts[i].x, &pts[i].y);
		}
		vector<pt<int>> chull = convex_hull(pts);
		printf("Case #%d\n", t);
		for(auto&p: chull) printf("%d %d\n", p.x, p.y);
	}
	return 0;
}
