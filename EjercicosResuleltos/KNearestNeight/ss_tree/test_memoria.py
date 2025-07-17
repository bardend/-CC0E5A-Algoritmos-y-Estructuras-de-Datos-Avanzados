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
