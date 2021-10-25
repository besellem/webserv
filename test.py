import os
import sys

for i in range(100):
	os.system(f"curl localhost:{int(sys.argv[1])} &");
	# os.system(f"curl localhost:{int(sys.argv[1])}/index.html &");
	# print(i)
