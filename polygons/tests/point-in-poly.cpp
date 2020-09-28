//Testeado en https://codeforces.com/problemsets/acmsguru/problem/99999/253
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct pt {
	ll x, y;
	pt operator-(const pt&q) const { return { x - q.x, y - q.y }; }
	ll operator%(const pt&q) const { return (x * q.y) - (y * q.x); }
	bool operator<(const pt&q) { return (x < q.x) or (x == q.x && y < q.y); }
};

struct poly{ 
	int n;
	vector<pt> pts;
	poly(int n) : n(n), pts(n) {};
	void normalize(){
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
	bool contains(const pt&t){
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
};

int n, m, k;

int main(){
	scanf("%d %d %d", &n, &m, &k);
	poly pol(n);
	for(int i = 0; i<n; i++) scanf("%lld %lld", &pol.pts[i].x, &pol.pts[i].y);
	reverse(pol.pts.begin(), pol.pts.end());
	pol.normalize();
	int cnt = 0;
	while(m--){
		int x, y;
		scanf("%d %d", &x, &y);
		cnt += pol.contains({x, y});
	}
	puts(cnt >= k ? "YES" : "NO");
	return 0;
}

