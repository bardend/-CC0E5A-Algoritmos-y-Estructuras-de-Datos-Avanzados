import heapq
import math
import matplotlib.colors as mcolors
from itertools import groupby
from matplotlib import pyplot as plt
from random import randrange, random
from scipy.spatial import KDTree
from typing import List, Optional, Tuple
NOISE = -1
def optics(points: List[Tuple[float, float]], eps: float, min_points: int) -> Tuple[List[Optional[float]], List[int]]:
    """
    Ejecuta OPTICS sobre los puntos dados.

    Parámetros:
        points: lista de tuplas (x, y) con las coordenadas de los puntos.
        eps: radio máximo para considerar vecinos.
        min_points: número mínimo de puntos para definir un punto core.

    Retorna:
        reachability_distances: lista con la distancia de alcanzabilidad de cada punto (None si es noise o border).
        ordering: orden en que los puntos fueron procesados.
    """
    
    def core_distance(distances: List[float]) -> Optional[float]:
        # Si hay menos vecinos que min_points, no hay distancia core.
        if len(distances) < min_points:
            return None
        # La distancia core es la (min_points-ésima) menor distancia.
        return sorted(distances)[min_points - 1]

    
    def update(neighbors: List[int], distances: List[float], seeds: List[Tuple[float, int]]):
        # Calcula la distancia core del punto actual.
        core_dist = core_distance(distances)
        if core_dist is None:
            # Sin distancia core, no se extiende el reachability.
            return seeds

        # Para cada vecino, actualiza su distancia de alcanzabilidad.
        for q, dist in zip(neighbors, distances):
            if not processed[q]:
                # La nueva reachability es el máximo entre core_dist y la distancia al vecino.
                new_r = max(core_dist, dist)
                old_r = reachability_distances[q]
                if old_r is None:
                    # Si no tenía valor previo, se asigna y se agrega a la semilla.
                    reachability_distances[q] = new_r
                    heapq.heappush(seeds, (new_r, q))
                elif new_r < old_r:
                    # Si la nueva reachability es mejor, se actualiza en el heap.
                    reachability_distances[q] = new_r
                    for i, (_, idx) in enumerate(seeds):
                        if idx == q:
                            seeds[i] = (new_r, q)
                            break
                    heapq.heapify(seeds)
        return seeds


    # Inicializa estructuras de datos.
    n = len(points)
    processed = [False] * n
    reachability_distances: List[Optional[float]] = [None] * n
    tree = KDTree(points)
    ordering: List[int] = []

    # Recorre todos los puntos.
    for p in range(n):
        if processed[p]:
            continue
        processed[p] = True

        # Encuentra todos los vecinos dentro del radio eps.
        nbrs = tree.query_ball_point(points[p], r=eps)
        # Calcula distancias reales a cada vecino.
        nbr_dists = [math.dist(points[p], points[q]) for q in nbrs]

        # Si el punto es core (tiene core_distance), inicia expansión.
        if core_distance(nbr_dists) is not None:
            ordering.append(p)
            seeds: List[Tuple[float, int]] = []
            seeds = update(nbrs, nbr_dists, seeds)
            
            while seeds:
                _, q = heapq.heappop(seeds)
                if processed[q]:
                    continue
                processed[q] = True
                ordering.append(q)

                # Expande desde el nuevo punto q.
                q_nbrs = tree.query_ball_point(points[q], r=eps)
                q_dists = [math.dist(points[q], points[i]) for i in q_nbrs]
                if core_distance(q_dists) is not None:
                    seeds = update(q_nbrs, q_dists, seeds)
        else:
            # Punto border o noise: distancia de alcanzabilidad indefinida.
            reachability_distances[p] = None

    return reachability_distances, ordering
def optics_to_clusters(ordering: List[int], reachability_distances: List[Optional[float]], eps: float) -> List[int]:
    n = len(ordering)
    labels = [NOISE] * len(ordering)
    cluster_id = 0

    for idx in range(n):
        point = ordering[idx]
        r = reachability_distances[point]
        if r is None or r > eps:
            # Iniciar un nuevo clúster solo si el punto anterior pertenecía a un clúster
            if idx > 0:
                prev = ordering[idx - 1]
                if reachability_distances[prev] is not None and reachability_distances[prev] <= eps:
                    cluster_id += 1
            labels[point] = NOISE
        else:
            labels[point] = cluster_id

    return labels
def create_spherical_cluster(centroid, radius, n):
    pts = []
    for _ in range(n):
        a = random() * 2*math.pi
        r = radius * math.sqrt(random())
        pts.append((centroid[0] + r*math.cos(a), centroid[1] + r*math.sin(a)))
    return pts
def plot_clusters(points: List[Tuple[float, float]], labels: List[int], ax):
    clusters = groupby(sorted(range(len(points)), key=lambda i: labels[i]), key=lambda i: labels[i])
    unique = sorted(set(labels))
    colors = ['k'] + list(mcolors.TABLEAU_COLORS.keys())
    markers = ['x', 'o', 's', 'p', '*', 'h', 'H', 'X', 'D', 'd']
    for label, group in clusters:
        pts = [points[i] for i in group]
        xs, ys = zip(*pts)
        color = colors[unique.index(label) % len(colors)]
        marker = markers[unique.index(label) % len(markers)]
        ax.plot(xs, ys, linestyle='none', marker=marker, markersize=8,
                markerfacecolor=color, markeredgecolor='k')
def plot_reachability(ax, ordering: List[int], reachability_distances: List[Optional[float]], labels: List[int], eps: float):
    rd = [d if d is not None else 2*eps for d in reachability_distances]
    xs = list(range(len(ordering)))
    # fill per-cluster
    for lbl in sorted(set(labels)):
        ys = [rd[ordering[i]] if labels[ordering[i]] == lbl else 0 for i in xs]
        color = mcolors.TABLEAU_COLORS[list(mcolors.TABLEAU_COLORS.keys())[lbl % len(mcolors.TABLEAU_COLORS)]]
        ax.fill_between(xs, 0, ys, facecolor=color, step='pre', alpha=0.6)
    ax.axhline(eps, color='r', linestyle='--')
    ax.set_ylabel('Reachability')
    ax.set_xlabel('Ordering')
def reachability_plot(points: List[Tuple[float, float]], ordering: List[int],
                      reachability_distances: List[Optional[float]], eps: float):
    labels = optics_to_clusters(ordering, reachability_distances, eps)
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
    fig.suptitle(f'OPTICS (eps={eps})')
    plot_clusters(points, labels, ax1)
    plot_reachability(ax2, ordering, reachability_distances, labels, eps)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
    