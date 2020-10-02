### Oscar Burga - 20181A324
### Graficador para convex hull
### Necesita 2 archivos "points.txt" y "chull.txt"
### Formato de archivos: primera linea N (cantidad de puntos), seguido de N lineas con los puntos
import matplotlib.pyplot as plt

def get_points(): #leer puntos 
	f = open("pts.txt", "r")
	n = int(f.readline().strip().split()[0])
	for i in range(n):
		a, b, t = [int(x) for x in f.readline().strip().split()]
		plt.scatter([a], [b], c='rg'[t])
	f.close()
		
def get_chull(): #leer puntos del poligono convexo
	f = open("in.txt", "r")
	n = int(f.readline().strip().split()[0])
	n = int(f.readline().strip().split()[0])
	x = []
	y = []
	for i in range(n):
		a, b = [int(x) for x in f.readline().strip().split()]
		x.append(a)
		y.append(b)
	f.close()
	# agrego copia del primer punto para poder graficar correctamente 
	x.append(x[0])
	y.append(y[0])
	return x, y


chx, chy = get_chull()

plt.plot(chx, chy, color='r') #graficar lineas del hull
plt.show() #mostrar grafico
