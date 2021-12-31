import matplotlib.pyplot as plt
#case 1
N1 = [5,6,7,8,9,10]
K1 = 10
variances1 = [196, 1500, 2750, 4900, 5800, 9675]

#case 2
K2 = [5,6,7,8,9,10]
N2 = 10
variances2 = [35372, 31244, 32532, 24532, 20324, 16243]

plt.plot(N1,variances1)
plt.xlabel("Number of processes (At K=10)")
plt.ylabel("Variance (Seconds)")
plt.title("Graph 1")
plt.show()

plt.plot(K2,variances2)
plt.xlabel("Number of rounds (At N=10)")
plt.ylabel("Variance (Seconds)")
plt.title("Graph 2")
plt.show()