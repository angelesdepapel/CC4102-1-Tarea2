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



int main(int argc, char* argv[]) {
  // Experimentacion memoria y tiempo 
  auto palabras = leerArchivo("datasets/words.txt");
  int N = palabras.size();

  cout << "====================================" << endl;
  cout << " EXPERIMENTO TRIES" << endl;
  cout << "====================================" << endl;
  cout << " Leyendo dataset..." << flush;
  cout << " OK (" << N << " palabras)" << endl;

  filesystem::path current = filesystem::current_path();
  filesystem::create_directories("words");

  cout << "\n====================================" << endl;
  cout << " Experimento 1: Consumo de memoria" << endl;
  cout << "====================================" << endl;

  ofstream csvMemoria(string("words/Memoria.csv"));
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
  cout << "\nOK - Resultados guardados en: words/Memoria.csv" << endl;



  cout << "\n====================================" << endl;
  cout << " Experimento 2: Tiempo de inserción" << endl;
  cout << "====================================" << endl;

  ofstream csvTiempo("words/Tiempo.csv");
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
  cout << "\nOK - Resultados guardados en words/Tiempo.csv" << endl;

  cout << "\n====================================" << endl;
  cout << " Experimento 3: Análisis de autocompletado " << endl;
  cout << "====================================" << endl;

  /** Arreglo de archivos del dataset a procesar */
  array<string, 3> archivos = {"wikipedia", "random", "random_with_distribution"};

  //por cada archivo en los datasets
  for (string archivo : archivos) {

    //leemos el archivo a analizar
    auto palabras = leerArchivo(archivo);

    //creamos el csv donde guardaremos los datos
    ofstream csvAutoCompleteTiempo(string("autocompletes/tiempo_" + archivo + ".csv"));
    csvAutoCompleteTiempo << "NumPalabras,NumChar,UserChar,TiempoTotal,TiempoPorPalabra,TiempoPorChar\n";

    //nuestro trie
    Trie trie_tiempo = Trie(Prioridad::TIEMPO);

    //poblamos nuestros tries con las palabras de words
    for (string w : leerArchivo("datasets/words.txt")) {
      trie_tiempo.insert(w);
    }

    long long L = 1 << 22; //palabras totales
    long long char_usuario = 0; //chars escritos por el usuario
    long long char_totales = 0; //chars totales
    
    for (int i ; i<L ; i++) { 
      //para cada palabra w
      std::string w = palabras[i];
      //partimos por la raiz
      Nodo *nodo_tiempo = trie_tiempo.getRaiz();

      //guardamos cuantas letras contamos
      int letras_contadas = 0;

      //por cada caracter en w
      for (char c : w) {
        letras_contadas++; //acabamos de leer una

        //bajamos por el trie
        auto check_tiempo = trie_tiempo.descend(nodo_tiempo, c); 

        //si la palabra no está en el trie
        if (check_tiempo == nullptr) { 
          //el usuario escribió toda la palabra
          char_usuario += w.length();

        //si la palabra está en el trie
        } else {
          //buscamos el mejor autocomplete
          auto result = trie_tiempo.autocomplete(check_tiempo);

          //si es la palabra que buscaba el usuario
          if (*result->str == (w + "$")) {
            //añadimos solo las que escribió el usuario
            char_usuario += letras_contadas; 
            //actualizamos la prio de esa palabra
            trie_tiempo.update_priority(result);

          } else {
            //si no era la que buscaba, seguimos bajando
            nodo_tiempo = check_tiempo;
          }
        }
      }
    }

    /*Hay que hacer los rescates de datos para i y repetir para el arbol de accesos*/


    

  }
  
  //Trie trie_accesos = Trie(Prioridad::ACCESOS);


  cout << "\n====================================" << endl;
  cout << " EXPERIMENTOS COMPLETADOS " << endl;
  cout << "====================================\n" << endl;
  return 0;

}
