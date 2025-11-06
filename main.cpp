#include "trie.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>

using namespace std;
using namespace chrono;

/** Lee un archivo, palabra por palabra.
 * Retorna un vector con las palabras leídas en el archivo. */
vector<string> leer_archivo(const string& filename) {
  vector<string> palabras;
  ifstream f(filename);
  string palabra;
  while (f >> palabra) palabras.push_back(palabra);
  f.close();
  return palabras;
}

/** Cuenta todos los caracteres de un vector de palabras, hasta el límite designado. */
long long contar_caracteres(const vector<string>& palabras, int hasta) {
  long long total = 0;
  for (int i = 0; i < hasta; i++) total += palabras[i].length();
  return total;
}

int main(int argc, char* argv[]) {
  for (int i = 0; i < 4; i++) {
    string words_file;
    if(i == 0) words_file = "words.txt";
    else if(i == 1) words_file = "wikipedia.txt";
    else if(i == 2) words_file = "random.txt";
    else if(i == 3) words_file = "random_with_distribution.txt";
    auto palabras = leer_archivo(words_file);
    int N = palabras.size();

    cout << "====================================" << endl;
    cout << " EXPERIMENTO TRIES" << endl;
    cout << "====================================" << endl;
    cout << " Archivo de palabras a procesar: " << words_file << endl;
    cout << "====================================" << endl;
    cout << "Leyendo dataset..." << flush;
    cout << " OK (" << N << " palabras)" << endl;

    cout << "\n====================================" << endl;
    cout << " Experimento 1: Consumo de memoria" << endl;
    cout << "====================================" << endl;

    ofstream csv_memoria(std::string(words_file + "Memoria.csv"));
    csv_memoria << "NumPalabras,NumNodos,NumCaracteres,NodosPorCaracter\n";

    Trie trie;
    vector<int> puntos_medicion;
    for (int exp = 0; exp <= 17; exp++) puntos_medicion.push_back(1 << exp);
    puntos_medicion.push_back(N);
    int id_medicion = 0;

    cout << "Insertando palabras y midiendo memoria..." << endl;
    for (int i = 0; i < N; i++) {
      trie.insert(palabras[i]);

      if (id_medicion < (int)puntos_medicion.size() && i + 1 == puntos_medicion[id_medicion]) {
        long long total_chars = contar_caracteres(palabras, i + 1);
        double nodos_por_char = (double)trie.getCantidadNodos() / total_chars;

        csv_memoria << (i + 1) << "," << trie.getCantidadNodos() << ","
                << total_chars << "," << nodos_por_char << "\n";

        cout << "  N = 2^" << id_medicion << " (" << (i+1) << " palabras): "
            << trie.getCantidadNodos() << " nodos, "
            << fixed << setprecision(4) << nodos_por_char << " nodos/char" << endl;

        id_medicion++;
      }
    }
    csv_memoria.close();
    cout << "OK - Resultados guardados en memoria.csv" << endl;


    cout << "\n====================================" << endl;
    cout << " Experimento 2: Tiempo de inserción" << endl;
    cout << "====================================" << endl;

    ofstream csv_tiempo(std::string(words_file + "Tiempo.csv"));
    csv_tiempo << "NumPalabras,TiempoGrupo,CaracteresGrupo,TiempoPorCaracter\n";

    Trie trie2;

    int M = 16;
    int grupo_size = N / M;

    cout << "Insertando " << N << " palabras en " << M << " grupos..." << endl;
    cout << "Tamaño de cada grupo: " << grupo_size << " palabras" << endl << endl;

    auto inicio_grupo = high_resolution_clock::now();
    long long chars_grupo = 0;

    for (int i = 0; i < N; i++) {
      trie2.insert(palabras[i]);
      chars_grupo += palabras[i].length();

      if ((i + 1) % grupo_size == 0 || i + 1 == N) {
        auto fin_grupo = high_resolution_clock::now();
        duration<double> tiempo_grupo = fin_grupo - inicio_grupo;
        double tiempo_por_char = tiempo_grupo.count() / chars_grupo;

        csv_tiempo << (i + 1) << "," << tiempo_grupo.count() << ","
                << chars_grupo << "," << tiempo_por_char << "\n";

        int grupo_num = (i + 1) / grupo_size;
        if ((i + 1) % grupo_size != 0) grupo_num++;

        cout << "  Grupo " << grupo_num << " (palabras " << (i + 1 - chars_grupo/6) << "-" << (i+1) << "): "
            << fixed << setprecision(6) << tiempo_grupo.count() << " s, "
            << tiempo_por_char << " s/char" << endl;

        inicio_grupo = high_resolution_clock::now();
        chars_grupo = 0;
      }
    }

    csv_tiempo.close();
    cout << "\nOK - Resultados guardados en tiempo_insercion.csv" << endl;

    cout << "\n====================================" << endl;
    cout << " EXPERIMENTO COMPLETADO" << endl;
    cout << "======================================" << endl;
  }
  return 0;
}
