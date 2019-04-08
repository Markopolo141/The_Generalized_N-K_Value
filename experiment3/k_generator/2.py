
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

def plot(data,xaxis=None,yaxis=None,zaxis=None):
	fig = plt.figure()
	ax = fig.gca(projection='3d')
	x = [a[0] for a in data]
	y = [a[1] for a in data]
	z = [a[2] for a in data]
	if xaxis is not None:
		ax.set_xlabel(xaxis)
	if yaxis is not None:
		ax.set_ylabel(yaxis)
	if zaxis is not None:
		ax.set_zlabel(zaxis)
	ax.scatter(x, y, z,s=2.7)
	plt.show()

plot([
[0,0,1],
[0.83,0,1.08],
[0,0.67,1.07],
[0.67,0.33,1.1],
[0,1.09,0.86],
[1.07,1.07,0.65],
[1.08,0.75,0.81],
[1.1,0,0.95],

[0,0,0],
[1,0,0],
[0,1,0],
[1,1,0]

],xaxis='x',yaxis='y',zaxis='z')
