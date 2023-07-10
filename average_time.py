import time
import os

n = int(input("How many iterations: "))
c = input("Command to run: ")
    
total_time = 0.0
for _ in range(n):
    before = time.time_ns()
    os.system(c)
    after = time.time_ns()
    total_time += float(after-before)/(10 ** 9)
print("Average time: " + str(total_time/float(n)))

