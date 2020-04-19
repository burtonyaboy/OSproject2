import random

with open("instructions.txt","w") as f:
    for i in range(0,1000):
        f.write(" " + str(int(random.normalvariate(5,2))))
