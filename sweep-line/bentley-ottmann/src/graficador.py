### Oscar Burga - 20181A324
### Graficador para bentley ottmann
### Necesita 2 archivos "segments.txt" y "inters.txt"
### Formato de archivos: primera linea N (cantidad de puntos), seguido de N lineas con los puntos
import matplotlib.pyplot as plt

def draw_lines(): 
	f = open("segments.txt", "r")
	n = int(f.readline().strip().split()[0])
	for i in range(n):
		a, b, x, y = [float(x) for x in f.readline().strip().split()]
		plt.plot([a, x], [b, y], color = 'b')
	f.close()
		
		
def draw_inters(): 
	f = open("inters.txt", "r")
	n = int(f.readline().strip().split()[0])
	x = []
	y = []
	for i in range(n):
		a, b = [float(x) for x in f.readline().strip().split()]
		x.append(a)
		y.append(b)
	f.close()
	plt.scatter(x, y, color='r')
	# agrego copia del primer punto para poder graficar correctamente 


draw_lines()
draw_inters()
plt.show() #mostrar grafico
