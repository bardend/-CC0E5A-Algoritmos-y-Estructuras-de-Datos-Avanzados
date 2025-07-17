##### Is Instance

```py
if isinstance(value, Point):
```

##### ss_tree_1.py

```py
import random
return [Point(tuple(round(random.random(), 1) for _ in range(dim))) for _ in range(n)]
```

#### Test

```py
import tracemalloc


# Iniciar el rastreo de asignaciones de memoria
tracemalloc.start()

# Código a monitorear
a = [i for i in range(2000000)]  # Crea una lista grande

# Tomar un snapshot de la memoria actual
snapshot = tracemalloc.take_snapshot()

# Obtener las 5 líneas con más consumo de memoria
top_stats = snapshot.statistics('lineno')

print("[ Top 5 líneas con más consumo de memoria ]")
for stat in top_stats[:5]:
    print(stat)
```

```py
from random import randrange, random, randint
num_points = 10
seen = set()
rects = []

for _ in range(num_points):
  min_x = randint(0, 20)
  min_y = randint(0, 20)
  max_x = randint(min_x + 20, 40)
  max_y = randint(min_y + 20, 40)

  key = (min_x, min_y, max_x, max_y)
  if key in seen:
    continue

  seen.add(key)
  rects.append(Rectangle(min_x, min_y, max_x, max_y))
```

#### Unittest

```py
def core_distance(distances, min_points):
    if len(distances) < min_points:
        return None
    return sorted(distances)[min_points - 1]

import unittest
import random

class TestOPTICS(unittest.TestCase):
    def test_core_distance_none(self):
        self.assertIsNone(core_distance([0.1, 0.2], 3))
    def test_core_distance_value(self):
        self.assertEqual(core_distance([0.3, 0.1, 0.5, 0.2, 0.4], 3), 0.3)

# Ejecuta las pruebas
suite = unittest.TestLoader().loadTestsFromTestCase(TestOPTICS)
unittest.TextTestRunner(verbosity=2).run(suite) 

```



#### Ploteo

```py

import math
import matplotlib.colors as mcolors
from matplotlib import pyplot as plt

def grafica_2d(vec_x, vec_y, xlabel, ylabel, title):
  plt.plot(vec_x, vec_y)
  plt.xlabel(xlabel)
  plt.ylabel(ylabel)
  plt.title(title)
  plt.show()

grafica_2d(x2, y2, "Numero de opercaiones Query", "Tiempo (s)", "Knn-k-Nearest")
```

```py
x_data = [[] for _ in range(2)]
y_data = [[] for _ in range(2)]

for i, metric in enumerate(metrics):
    plt.plot(x_data[i], y_data[i], label=metric.__name__)
    
plt.legend()
plt.show()  
```



#### Sort

```py
candidates.sort(key = lambda pair: pair[1])

```

#### Dsu

```py
maxn = 10000
parent_cluster = [i for i in range(maxn)]

def find(x):
    if parent_cluster[x] == x:
        return x
    parent_cluster[x] = find(parent_cluster[x])
    return parent_cluster[x]

def union(x, y):
    root_x = find(x)
    root_y = find(y)
    
    if root_x != root_y:
        parent_cluster[root_y] = root_x

```

#### Unique

```py
import numpy as np
parent_cluster_np = np.array(parent_cluster)
from numpy import unique    
print(unique(parent_cluster_np))
```

