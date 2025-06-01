import subprocess
import os
import argparse

def run_generate_test(n):
    print("📦 Compilando y ejecutando generador de test...")
    os.chdir("../GenerateTest")
    subprocess.run(["g++", "-o", "a", "mapa.cpp"], check=True)

    # Ejecutamos y enviamos la entrada
    subprocess.run(["./a"], input=f"{n}\n", text=True, check=True)

    os.chdir("../Proyect2")

def run_dotnet_tests():
    print("🧪 Ejecutando tests con dotnet...")
    subprocess.run(["dotnet", "test", "ScapegoatTree.Tests/"], check=True)

if __name__ == "__main__":
    try:
        parser = argparse.ArgumentParser(description="Generador de test automático")
        parser.add_argument("--numop", type=int, required=True, help="Número a pasar al ejecutable")
        args = parser.parse_args()
        run_generate_test(args.numop)
        run_dotnet_tests()
        print("\n✅ Todo se ejecutó correctamente.")
    except subprocess.CalledProcessError as e:
        print(f"\n❌ Error durante la ejecución: {e}")

