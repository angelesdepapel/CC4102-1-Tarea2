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

  filesystem::create_directories("autocompletes");

  /** Arreglo de archivos del dataset a procesar */
  array<string, 3> archivos = {"wikipedia", "random", "random_with_distribution"};

  cout << "\n====================================" << endl;
  cout << " PRIORIDAD TIEMPO" << endl;
  cout << "====================================" << endl;
  //por cada archivo en los datasets
  for (string archivo : archivos) {

    cout << "\n====================================" << endl;
    //leemos el archivo a analizar
    cout << " Leyendo " << archivo << endl;
    auto palabras = leerArchivo("datasets/" + archivo + ".txt");
    cout << " Listo" << endl;
    cout << "====================================" << endl;

    //creamos el csv donde guardaremos los datos
    ofstream csvAutoCompleteTiempo(string("autocompletes/tiempo_" + archivo + ".csv"));
    csvAutoCompleteTiempo << "NumPalabras,NumChar,UserChar,CharPorTotal,TiempoTotal,TiempoPorPalabra,TiempoPorChar\n";

    //nuestro trie
    Trie trie_tiempo = Trie(Prioridad::TIEMPO);

    //poblamos nuestros tries con las palabras de words
    cout << " Poblando Trie" << endl;
    for (string w : leerArchivo("datasets/words.txt")) {
      trie_tiempo.insert(w);
    }
    cout << " Trie poblado" << endl;

    auto inicioTiempo = high_resolution_clock::now();
    long long L = 1 << 22; //palabras totales
    long long char_usuario = 0; //chars escritos por el usuario
    long long char_totales = 0; //chars totales
    long long exponente = 1; //para comparar con i
    int medicion = 0; //para printear
    
    for (int i=0; i<L ; i++) {
      //para cada palabra w
      std::string w = palabras[i];
      //partimos por la raiz
      Nodo *nodo_tiempo = trie_tiempo.getRaiz();

      //guardamos cuantas letras contamos
      int letras_contadas = 0;

      //por cada caracter en w
      for (char c : w) {
        letras_contadas++; //acabamos de escribir una

        //bajamos por el trie
        auto check_tiempo = trie_tiempo.descend(nodo_tiempo, c); 
        //si la palabra no está en el trie
        if (check_tiempo == nullptr) { 
          //el usuario escribió toda la palabra
          char_usuario += w.length();
          break;

        //si la palabra está en el trie
        } else {
          //buscamos el mejor autocomplete
          Nodo *result = trie_tiempo.autocomplete(check_tiempo);
          //si es la palabra que buscaba el usuario
          if (*result->str == (w + "$")) {
            //añadimos solo las que escribió el usuario
            char_usuario += letras_contadas; 
            //actualizamos la prio de esa palabra
            trie_tiempo.update_priority(result);
            break;
          } else {//si no era la que buscaba
            //si terminamos de escribir la palabra
            if (letras_contadas == w.length()) {
              char_usuario += letras_contadas; //le sumamos toda la palabra
              break;
            } else { //si no seguimos bajando
              nodo_tiempo = check_tiempo;
            }
          }
        }
      }
      //contamos los caracteres totales
      char_totales += w.length();

      //cuando llegamos a uno de los puntos de medicion
      if (i+1 == exponente) {
        auto finTiempo = high_resolution_clock::now();
        duration<double> tiempoTiempo = finTiempo-inicioTiempo;
        double tiempoTiempoTotal = tiempoTiempo.count();
        double tiempoTiempoPorPalabra = tiempoTiempoTotal/(i+1);
        double tiempoTiempoPorChar = tiempoTiempoTotal/char_totales;
        double porcentaje = (double)char_usuario/(double)char_totales;

        cout << " i = 2^" << medicion << ":"<< endl;
        cout << "  N° de palabras: " << i+1 << endl;
        cout << "  N° de caracteres totales: " << char_totales << endl;
        cout << "  N° de caracteres escritos: " << char_usuario << endl;
        cout << "  caracteres escritos / carateres totales: " << porcentaje << endl;
        cout << "  Tiempo total: " << tiempoTiempoTotal << endl;
        cout << "  Tiempo por palabra: " << tiempoTiempoPorPalabra << endl;
        cout << "  Tiempo por caracter: " << tiempoTiempoPorChar << endl;

        csvAutoCompleteTiempo << (i+1) << "," << char_totales << ","
          << char_usuario << "," << porcentaje << "," << tiempoTiempoTotal << ","
          << tiempoTiempoPorPalabra << "," << tiempoTiempoPorChar << "\n";

        exponente <<= 1; //*2
        medicion++;
      }
    }
    csvAutoCompleteTiempo.close();
    cout << "\nOK - Resultados guardados en: " << "autocompletes/tiempo_" << archivo << ".csv" << endl;
  }


  cout << "\n====================================" << endl;
  cout << " PRIORIDAD ACCESOS" << endl;
  cout << "====================================" << endl;
  //por cada archivo en los datasets
  for (string archivo : archivos) {

    cout << "\n====================================" << endl;
    //leemos el archivo a analizar
    cout << " Leyendo " << archivo << endl;
    auto palabras = leerArchivo("datasets/" + archivo + ".txt");
    cout << " Listo" << endl;
    cout << "====================================" << endl;


    //creamos el csv donde guardaremos los datos
    ofstream csvAutoCompleteAccesos(string("autocompletes/accesos_" + archivo + ".csv"));
    csvAutoCompleteAccesos << "NumPalabras,NumChar,UserChar,CharPorTotal,TiempoTotal,TiempoPorPalabra,TiempoPorChar\n";

    //nuestro trie
    Trie trie_accesos = Trie(Prioridad::ACCESOS);

    //poblamos nuestros tries con las palabras de words
    cout << " Poblando Trie" << endl;
    for (string w : leerArchivo("datasets/words.txt")) {
      trie_accesos.insert(w);
    }
    cout << " Trie poblado" << endl;

    auto inicioAccesos = high_resolution_clock::now();
    long long L = 1 << 22; //palabras totales
    long long char_usuario = 0; //chars escritos por el usuario
    long long char_totales = 0; //chars totales
    long long exponente = 1; //para comparar con i
    int medicion = 0; //para printear
    
    for (int i=0; i<L ; i++) {
      //para cada palabra w
      std::string w = palabras[i];
      //partimos por la raiz
      Nodo *nodo_accesos = trie_accesos.getRaiz();

      //guardamos cuantas letras contamos
      int letras_contadas = 0;

      //por cada caracter en w
      for (char c : w) {
        letras_contadas++; //acabamos de escribir una

        //bajamos por el trie
        auto check_accesos = trie_accesos.descend(nodo_accesos, c); 
        //si la palabra no está en el trie
        if (check_accesos == nullptr) { 
          //el usuario escribió toda la palabra
          char_usuario += w.length();
          break;

        //si la palabra está en el trie
        } else {
          //buscamos el mejor autocomplete
          Nodo *result = trie_accesos.autocomplete(check_accesos);
          //si es la palabra que buscaba el usuario
          if (*result->str == (w + "$")) {
            //añadimos solo las que escribió el usuario
            char_usuario += letras_contadas; 
            //actualizamos la prio de esa palabra
            trie_accesos.update_priority(result);
            break;
          } else {//si no era la que buscaba
            //si terminamos de escribir la palabra
            if (letras_contadas == w.length()) {
              char_usuario += letras_contadas; //le sumamos toda la palabra
              break;
            } else { //si no seguimos bajando
              nodo_accesos = check_accesos;
            }
          }
        }
      }
      //contamos los caracteres totales
      char_totales += w.length();

      //cuando llegamos a uno de los puntos de medicion
      if (i+1 == exponente) {
        auto finAccesos = high_resolution_clock::now();
        duration<double> tiempoTiempo = finAccesos-inicioAccesos;
        double tiempoAccesosTotal = tiempoTiempo.count();
        double tiempoAccesosPorPalabra = tiempoAccesosTotal/(i+1);
        double tiempoAccesosPorChar = tiempoAccesosTotal/char_totales;
        double porcentaje = (double)char_usuario/(double)char_totales;

        cout << " i = 2^" << medicion << ":"<< endl;
        cout << "  N° de palabras: " << i+1 << endl;
        cout << "  N° de caracteres totales: " << char_totales << endl;
        cout << "  N° de caracteres escritos: " << char_usuario << endl;
        cout << "  caracteres escritos / carateres totales: " << porcentaje << endl;
        cout << "  Tiempo total: " << tiempoAccesosTotal << endl;
        cout << "  Tiempo por palabra: " << tiempoAccesosPorPalabra << endl;
        cout << "  Tiempo por caracter: " << tiempoAccesosPorChar << endl;

        csvAutoCompleteAccesos << (i+1) << "," << char_totales << ","
          << char_usuario << "," << porcentaje << "," << tiempoAccesosTotal << ","
          << tiempoAccesosPorPalabra << "," << tiempoAccesosPorChar << "\n";

        exponente <<= 1; //*2
        medicion++;
      }
    }
    csvAutoCompleteAccesos.close();
    cout << "\nOK - Resultados guardados en: " << "autocompletes/accesos_" << archivo << ".csv" << endl;
  }


  cout << "\n====================================" << endl;
  cout << " EXPERIMENTOS COMPLETADOS " << endl;
  cout << "====================================\n" << endl;
  return 0;

}
