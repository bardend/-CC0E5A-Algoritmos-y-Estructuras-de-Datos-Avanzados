import subprocess
import os

def run_generate_test():
    print("📦 Compilando y ejecutando generador de test...")
    os.chdir("../GenerateTest")  # Subimos un nivel y entramos a GenerateTest
    subprocess.run(["g++", "-o", "a", "mapa.cpp"], check=True)
    subprocess.run(["./a"], check=True)
    os.chdir("../Proyect2")  # Volvemos a Proyect2

def run_dotnet_tests():
    print("🧪 Ejecutando tests con dotnet...")
    subprocess.run(["dotnet", "test", "ScapegoatTree.Tests/"], check=True)

if __name__ == "__main__":
    try:
        run_generate_test()
        run_dotnet_tests()
        print("\n✅ Todo se ejecutó correctamente.")
    except subprocess.CalledProcessError as e:
        print(f"\n❌ Error durante la ejecución: {e}")

