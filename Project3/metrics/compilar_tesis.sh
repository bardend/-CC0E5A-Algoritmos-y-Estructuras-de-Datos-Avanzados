#!/bin/bash

echo "=== COMPILACIÓN DE TESIS EN LATEX ==="

# Verificar si pdflatex está instalado
if ! command -v pdflatex &> /dev/null; then
    echo "Error: pdflatex no está instalado"
    echo "Instala con: sudo apt-get install texlive-latex-base texlive-latex-recommended texlive-latex-extra"
    exit 1
fi

# Compilar el documento LaTeX
echo "Compilando tesis.tex..."

# Primera pasada
pdflatex -interaction=nonstopmode tesis.tex

# Segunda pasada para referencias
pdflatex -interaction=nonstopmode tesis.tex

# Limpiar archivos auxiliares
rm -f *.aux *.log *.toc *.out *.synctex.gz

echo "=== COMPILACIÓN COMPLETADA ==="
echo "Archivo generado: tesis.pdf"

if [ -f "tesis.pdf" ]; then
    echo "Tamaño del archivo: $(du -h tesis.pdf | cut -f1)"
    echo "Páginas: $(pdfinfo tesis.pdf 2>/dev/null | grep Pages | awk '{print $2}')"
fi