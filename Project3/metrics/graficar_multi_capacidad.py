#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Configurar estilo de las gráficas
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

def main():
    # Leer datos
    try:
        data = pd.read_csv('resultados_multi_capacidad.csv')
        print(f"Datos cargados: {len(data)} registros")
        print(f"Capacidades encontradas: {sorted(data['capacity'].unique())}")
        print(f"Tamaños encontrados: {sorted(data['tree_size'].unique())}")
    except FileNotFoundError:
        print("Error: No se encontró el archivo 'resultados_multi_capacidad.csv'")
        print("Ejecuta primero: ./ejecutar_multi_capacidad.sh")
        return
    
    # Crear figura con 2 subgráficos (1x2)
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    fig.suptitle('Análisis Multi-Capacidad: Tiempo y Anomalías vs Tamaño del Árbol', fontsize=16, fontweight='bold')
    
    # 1. Tiempo promedio vs Tamaño del árbol por capacidad
    tiempo_por_tamano = data.groupby(['tree_size', 'capacity'])['insertion_time_ms'].mean().reset_index()
    capacities = sorted(data['capacity'].unique())
    
    for capacity in capacities:
        subset = tiempo_por_tamano[tiempo_por_tamano['capacity'] == capacity]
        axes[0].plot(subset['tree_size'], subset['insertion_time_ms'], 
                    marker='o', linewidth=2, markersize=8, label=f'Capacidad {capacity}')
    
    axes[0].set_xlabel('Número de Elementos del Árbol')
    axes[0].set_ylabel('Tiempo Promedio de Inserción (ms)')
    axes[0].set_title('Tiempo vs Tamaño del Árbol por Capacidad')
    axes[0].legend()
    axes[0].grid(True, alpha=0.3)
    axes[0].set_xscale('log')  # Escala logarítmica para mejor visualización
    
    # 2. Tasa de anomalías vs Tamaño del árbol por capacidad
    anomaly_rate = data.groupby(['tree_size', 'capacity'])['is_anomaly'].mean() * 100
    anomaly_rate_df = anomaly_rate.reset_index()
    
    for capacity in capacities:
        subset = anomaly_rate_df[anomaly_rate_df['capacity'] == capacity]
        axes[1].plot(subset['tree_size'], subset['is_anomaly'], 
                    marker='^', linewidth=2, markersize=8, label=f'Capacidad {capacity}')
    
    axes[1].set_xlabel('Número de Elementos del Árbol')
    axes[1].set_ylabel('Tasa de Anomalías (%)')
    axes[1].set_title('Tasa de Anomalías vs Tamaño del Árbol por Capacidad')
    axes[1].legend()
    axes[1].grid(True, alpha=0.3)
    axes[1].set_xscale('log')  # Escala logarítmica para mejor visualización
    

    
    # Ajustar espaciado
    plt.tight_layout()
    
    # Guardar gráficas
    plt.savefig('analisis_multi_capacidad.png', dpi=300, bbox_inches='tight')
    print("Gráficas guardadas como 'analisis_multi_capacidad.png'")
    
    # Mostrar estadísticas resumen
    print("\n=== ESTADÍSTICAS RESUMEN ===")
    print(f"Total de configuraciones probadas: {len(data.groupby(['capacity', 'tree_size']))}")
    
    print("\nTiempo promedio por capacidad:")
    tiempo_summary = data.groupby('capacity')['insertion_time_ms'].agg(['mean', 'std']).round(4)
    print(tiempo_summary)
    
    print("\nTasa de anomalías por capacidad:")
    anomaly_summary = data.groupby('capacity')['is_anomaly'].mean() * 100
    print(anomaly_summary.round(2))
    
    # Mostrar gráficas
    plt.show()

if __name__ == "__main__":
    main()