### Oscar Burga - 20181A324
### Graficador para convex hull
### Necesita 2 archivos "points.txt" y "chull.txt"
### Formato de archivos: primera linea N (cantidad de puntos), seguido de N lineas con los puntos
import matplotlib.pyplot as plt

		
fig, ax = plt.subplots()
		
def get_poly(): 
	f = open("poly.txt", "r")
	n = int(f.readline().strip().split()[0])
	x = []
	y = []
	for i in range(n):
		a, b = [float(x) for x in f.readline().strip().split()]
		x.append(a)
		y.append(b)
	f.close()
	x.append(x[0])
	y.append(y[0])
	return x, y

def get_inter():
	f = open("inter.txt", "r")
	n = int(f.readline().strip().split()[0])
	if n == 0: return
	x = []
	y = []
	for i in range(n):
		a, b = [float(x) for x in f.readline().strip().split()]
		x.append(a)
		y.append(b)
	if n == 1:
		ax.add_artist(plt.Circle((x[0], y[0]), float(f.readline().strip())))
		ax.scatter(x, y, c='r')
		return
	if n == 2:
		plt.plot(x, y)
		return
	f.close()
	x.append(x[0])
	y.append(y[0])
	if n >= 3:
		plt.fill(x, y, 'b')
		return
polyx, polyy = get_poly()

plt.plot(polyx, polyy, color='r') #graficar lineas del hull
get_inter()
plt.show() #mostrar grafico
