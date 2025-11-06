#include "trie.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <filesystem>

using namespace std;
using namespace chrono;

/** Lee un archivo, palabra por palabra.
 * Retorna un vector con las palabras leídas en el archivo. */
vector<string> leerArchivo(const string& filename) {
  vector<string> palabras;
  ifstream f(filename);
  string palabra;
  while (f >> palabra) palabras.push_back(palabra);
  f.close();
  return palabras;
}

/** Cuenta todos los caracteres de un vector de palabras, hasta el límite designado.
 * Retorna un long long con la cantidad total de caracteres del vector hasta el límite. */
long long contarCaracteres(const vector<string>& palabras, int hasta) {
  long long total = 0;
  for (int i = 0; i < hasta; i++) total += palabras[i].length();
  return total;
}

/** Arreglo de archivos del dataset a procesar */
array<string, 4> archivos = {"words", "wikipedia", "random", "random_with_distribution"};

int main(int argc, char* argv[]) {
  for (int i = 0; i < 4; i++) {
    string archivo = archivos[i] + ".txt";
    auto palabras = leerArchivo("datasets/" + archivo);
    int N = palabras.size();

    cout << "====================================" << endl;
    cout << " EXPERIMENTO TRIES" << endl;
    cout << "====================================" << endl;
    cout << " Archivo de palabras a procesar: " << archivo << endl;
    cout << " Leyendo dataset..." << flush;
    cout << " OK (" << N << " palabras)" << endl;

    filesystem::path current = filesystem::current_path();
    filesystem::create_directories(archivos[i]);

    cout << "\n====================================" << endl;
    cout << " Experimento 1: Consumo de memoria" << endl;
    cout << "====================================" << endl;

    ofstream csvMemoria(string(archivos[i] + "/Memoria.csv"));
    csvMemoria << "NumPalabras,NumNodos,NumCaracteres,NodosPorCaracter\n";

    Trie trie;
    vector<int> puntosMedicion;
    for (int exp = 0; exp <= 17; exp++) puntosMedicion.push_back(1 << exp);
    puntosMedicion.push_back(N);
    int indiceMedicion = 0;

    cout << "Insertando palabras y midiendo memoria..." << endl;
    for (int i = 0; i < N; i++) {
      trie.insert(palabras[i]);

      if (indiceMedicion < (int)puntosMedicion.size() && i + 1 == puntosMedicion[indiceMedicion]) {
        long long totalChars = contarCaracteres(palabras, i + 1);
        double nodosPorChar = (double)trie.getCantidadNodos() / totalChars;

        csvMemoria << (i + 1) << "," << trie.getCantidadNodos() << ","
          << totalChars << "," << nodosPorChar << "\n";

        cout << "  N = 2^" << indiceMedicion << " (" << (i+1) << " palabras): "
          << trie.getCantidadNodos() << " nodos, "
          << fixed << setprecision(4) << nodosPorChar << " nodos/char" << endl;

        indiceMedicion++;
      }
    }
    csvMemoria.close();
    cout << "\nOK - Resultados guardados en: " << archivos[i] << "/Memoria.csv" << endl;



    cout << "\n====================================" << endl;
    cout << " Experimento 2: Tiempo de inserción" << endl;
    cout << "====================================" << endl;

    ofstream csvTiempo(string(archivos[i] + "/Tiempo.csv"));
    csvTiempo << "NumPalabras,TiempoGrupo,CaracteresGrupo,TiempoPorCaracter\n";

    Trie trie2;

    int M = 16;
    int grupoSize = N / M;

    cout << "Insertando " << N << " palabras en " << M << " grupos..." << endl;
    cout << "Tamaño de cada grupo: " << grupoSize << " palabras" << endl << endl;

    auto inicioGrupo = high_resolution_clock::now();
    long long charsGrupo = 0;

    for (int i = 0; i < N; i++) {
      trie2.insert(palabras[i]);
      charsGrupo += palabras[i].length();

      if ((i + 1) % grupoSize == 0 || i + 1 == N) {
        auto finGrupo = high_resolution_clock::now();
        duration<double> tiempoGrupo = finGrupo - inicioGrupo;
        double tiempoPorChar = tiempoGrupo.count() / charsGrupo;

        csvTiempo << (i + 1) << "," << tiempoGrupo.count() << ","
          << charsGrupo << "," << tiempoPorChar << "\n";

        int grupoNum = (i + 1) / grupoSize;
        if ((i + 1) % grupoSize != 0) grupoNum++;

        cout << "  Grupo " << grupoNum << " (palabras " << (i + 1 - charsGrupo/6)
          << "-" << (i+1) << "): " << fixed << setprecision(6) << tiempoGrupo.count()
          << " s, " << tiempoPorChar << " s/char" << endl;

        inicioGrupo = high_resolution_clock::now();
        charsGrupo = 0;
      }
    }

    csvTiempo.close();
    cout << "\nOK - Resultados guardados en " << archivos[i] << "/Tiempo.csv" << endl;

    cout << "\n====================================" << endl;
    cout << " EXPERIMENTOS COMPLETADOS PARA " << archivo << endl;
    cout << "====================================\n" << endl;
  }
  return 0;
}
