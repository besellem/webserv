import os
import sys

for i in range(1):
	os.system(f"curl localhost:{int(sys.argv[1])}/www.apple.com.html &");
	# print(i)
