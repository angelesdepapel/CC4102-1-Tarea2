#pragma once
#include <vector>
#include <string>
#include "nodo.h"

using namespace std;

/** Tipos de prioridad */
enum class Prioridad {
  TIEMPO, ACCESOS
};

class Trie {
private:
  Nodo* raiz;
  Prioridad prioridad;
  long long cantidadAccesos;
  int cantidadNodos;
  //vector<Nodo*> nodos;

  /** Dado un carácter, retorna su índice correspondiente en el arreglo next. */
  int getIndex(char c) {
    if (c == '$') return 26;
    else return c - 'a';
  }

  /** Dado un índice del arreglo next, retorna su carácter correspondiente. */
  char getChar(int i) {
    if (i == 26) return '$';
    else return 'a' + i;
  }

  /** Dado un nodo, actualiza su bestTerminal y bestPriority
   * recursivamente hacia arriba, hasta llegar a la raíz del árbol. */
  void update(Nodo* v) {
    Nodo* actual = v->parent;
    long long nuevaPrioridad = v->priority;

    while (actual != nullptr) {
      if (actual->bestPriority < nuevaPrioridad) {
        actual->bestPriority = nuevaPrioridad;
        actual->bestTerminal = v;
        actual = actual->parent;
      } else break;
    }
  }

  public:
    Trie(Prioridad p = Prioridad::TIEMPO): prioridad(p), cantidadAccesos(0), cantidadNodos(0) {
      raiz = new Nodo();
      cantidadNodos++;
      //nodos.push_back(raiz);
    }

    /** Inserta la palabra w carácter por carácter. creando nodos cuando
     * sea necesario. Al finalizar la palabra, agrega el nodo terminal. */
    void insert(const string& w) {
      Nodo* actual = raiz;

      for (char c : w) {
        int index = getIndex(c);

        if (actual->next[index] == nullptr) {
          Nodo* nuevo = new Nodo();
          nuevo->parent = actual;
          actual->next[index] = nuevo;
          cantidadNodos++;
          //nodos.push_back(nuevo);
        }

        actual = actual->next[index];
      }

      int lastIndex = sigma - 1;
      if (actual->next[lastIndex] == nullptr) {
        Nodo* terminal = new Nodo();
        terminal->parent = actual;
        terminal->priority = 0;
        terminal->str = new string(w + "$");
        terminal->bestTerminal = terminal;
        terminal->bestPriority = 0;

        actual->next[lastIndex] = terminal;
        cantidadNodos++;
        //nodos.push_back(terminal);

        if (actual->bestTerminal == nullptr || actual->bestPriority < 0) {
          actual->bestTerminal = terminal;
          actual->bestPriority = 0;
          update_priority(terminal);
        }
      }
    }

    /** Dado un puntero a un nodo, retorna el puntero resultante
     * de descender por el carácter c. */
    Nodo* descend(Nodo* v, char c) {
      if (v == nullptr) return nullptr;
      int index = getIndex(c);
      return v->next[index];
    }

    /** Dado un sub árbol, retorna un puntero a su mejor autocompletado. */
    Nodo* autocomplete(Nodo* v) {
      if (v == nullptr) return nullptr;
      return v->bestTerminal;
    }                                                                                                                                           

    /** Actualiza la prioridad del nodo terminal v según la variante
     * y actualiza los nodos en el camino a la raíz. Esto último se realiza
     * mediante la función auxiliar update. */
    void update_priority(Nodo* v) {
      if (v == nullptr) return;
      if (prioridad == Prioridad::ACCESOS) {
        v->priority++;
        v->bestPriority = v->priority;
      } else {
        cantidadAccesos++;
        v->priority = cantidadAccesos;
        v->bestPriority = cantidadAccesos;
      }
      update(v);
    }

    const int getCantidadNodos() {
      return cantidadNodos;
    }

    Nodo *getRaiz() {
      return raiz;
    }
/*
    string bestWord(const string& s) {
      Nodo *actual = raiz;
      for (char c : s) {
        int index = getIndex(c);

        if (actual->next[index] == nullptr) {
          Nodo* nuevo = new Nodo();
          actual->next[index] = nuevo;
          cantidadNodos++;
          nodos.push_back(nuevo);
        }

        actual = actual->next[index];
      }

    }*/
};
