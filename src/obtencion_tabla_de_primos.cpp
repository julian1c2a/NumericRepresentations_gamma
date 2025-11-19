#include <iostream>
#include <array>
#include <fstream>
#include <string>

// Define el límite superior del rango
const int LIMITE_SUPERIOR = 65537;
const std::string NOMBRE_ARCHIVO = "primos.csv";

void generar_y_guardar_primos() {
    // 1. Implementación de la Criba de Eratóstenes
    // Se crea un vector booleano "es_primo[0..LIMITE_SUPERIOR]" y se inicializan todas las entradas como verdadero.
    std::array<bool,65540> es_primo{};
    for(bool & ind: es_primo)
        ind = true;
    es_primo[0] = es_primo[1] = false; // 0 y 1 no son números primos.

    for (int p = 2; p * p <= LIMITE_SUPERIOR; p++) {
        // Si es_primo[p] no ha cambiado, entonces es un primo.
        if (es_primo[p] == true) {
            // Marca todos los múltiplos de p como falso (no primos), comenzando desde p*p.
            for (int i = p * p; i <= LIMITE_SUPERIOR; i += p) {
                es_primo[i] = false;
            }
        }
    }

    // 2. Apertura del archivo CSV para escritura
    std::ofstream archivo_csv(NOMBRE_ARCHIVO);

    // Verifica si el archivo se abrió correctamente
    if (!archivo_csv.is_open()) {
        std::cerr << "Error al abrir el archivo " << NOMBRE_ARCHIVO << std::endl;
        return;
    }

    // Opcional: Escribir un encabezado de columna
    archivo_csv << "Primos" << "\n";

    // 3. Escritura de los números primos en el archivo CSV
    int linea = 0;
    for (int p = 2; p <= LIMITE_SUPERIOR; p++) {
        if (es_primo[p]) {
            archivo_csv << p << " , "; // Escribe el primo seguido de un salto de línea
            ++linea;
        }
        if (linea == 31) {
            archivo_csv << "\n";
            linea = 0;
        }
    }

    // 4. Cierre del archivo
    archivo_csv.close();

    std::cout << "Los números primos entre 2 y " << LIMITE_SUPERIOR 
              << " han sido guardados en '" << NOMBRE_ARCHIVO << "'." << std::endl;
}

int main() {
    generar_y_guardar_primos();
    return 0;
}