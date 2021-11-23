
import random

INSERT_NUM = 100
# <= INSERT_NUM
DELETE_NUM = 20

elems = list (range (1, INSERT_NUM))
random.shuffle (elems)

toDelete = random.sample (elems, DELETE_NUM)

print (INSERT_NUM)

for i in range (0, INSERT_NUM - 1) :
    print (elems[i], end = ' ')

print ()
print (DELETE_NUM)

for i in range (0, DELETE_NUM - 1) :
    print (toDelete[i], end = ' ')

print ()
