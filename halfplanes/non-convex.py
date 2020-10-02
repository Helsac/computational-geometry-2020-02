import resource, sys
resource.setrlimit(resource.RLIMIT_STACK, (2**29, -1))
sys.setrecursionlimit(10**7)
#https://github.com/luogu-dev/cyaron
from cyaron import *

pts = Polygon.simple_polygon(Vector.random(10, [500, 500], 0)).points

f = open("poly.txt", "w")
f.write(str(len(pts)) + '\n')
for pt in pts:
	f.write(str(pt[0])+' '+str(pt[1])+'\n')
f.close()

