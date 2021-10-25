import os

for i in range(100):
	os.system("curl localhost:8081/index.html & >/dev/null");
	print(i)
