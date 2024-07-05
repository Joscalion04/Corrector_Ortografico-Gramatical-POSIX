# Corrector_Ortografico-Gramatical-POSIX
Este proyecto es un editor de texto avanzado implementado en C, diseñado para realizar múltiples funciones sobre archivos de texto utilizando hilos y forks para operaciones concurrentes y paralelas. Permite crear, leer, editar y modificar archivos de texto, con capacidades integradas de análisis ortográfico y semántico.

## Características

- **Crear Archivo**: Permite al usuario crear un archivo de texto, ingresando un nombre y contenido.
- **Leer Archivo**: Lee y muestra el contenido de un archivo de texto existente.
- **Editar Contenido**: Agrega texto al final de un archivo existente.
- **Modificar Archivo**: Busca y reemplaza palabras específicas en un archivo de texto.
- **Análisis Ortográfico**: Utiliza un analizador ortográfico para corregir palabras mal escritas, sugiriendo palabras cercanas válidas.
- **Análisis Semántico**: Realiza un análisis semántico básico para verificar la estructura gramatical de las oraciones ingresadas.

## Tecnologías y Funcionalidades

- **Lenguaje**: C
- **Librerías**: pthread (POSIX Threads)
- **Operaciones Seguras**: Utiliza mutex para asegurar la exclusión mutua y evitar condiciones de carrera.
- **Interfaz de Usuario**: Menús interactivos para seleccionar las operaciones deseadas y ingresar datos.

## Integración de Funcionalidades

El programa integra múltiples hilos para manejar de manera eficiente las operaciones de entrada y salida de archivos, así como los análisis ortográficos y semánticos. Cada operación se ejecuta de manera independiente mediante el uso de hilos, asegurando que el programa sea capaz de realizar múltiples tareas concurrentemente.

## Uso

Para usar el programa, clona este repositorio y compila el código en un entorno compatible con POSIX Threads. Asegúrate de tener instalado el compilador de C y las herramientas necesarias para compilar y ejecutar programas en C.

## Creditos

Este repositorio fue realizado como proyecto universitario.
Integrantes:
 - Raquel Hernández Campos
 - David Serrano Medrano
 - Joseph Leon Cabezas
