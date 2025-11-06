#pragma once
#include <array>
#include <string>

using namespace std;

/** Para esta tarea se utilizarán datasets en inglés, por lo que
 * el tamaño del alfabeto es de 27 (26 caracteres, más el carácter especial $) */
const int sigma = 27;

/** Estructura de datos que representa un nodo.
 * - parent: una referencia a su nodo padre (nulo si el nodo es la raíz).
 * - next: estructura que mapea caracteres Σ a hijos.
 * - priority: dependiendo del criterio a utilizar, es el tiempo de acceso o
 * la cantidad de accesos a este nodo terminal.
 * - str: si el nodo es terminal esto debe contener un puntero al string asociado.
 * - bestTerminal: un puntero al nodo terminal del subárbol con mayor prioridad.
 * - bestPriority: la prioridad del nodo con mayor prioridad del subárbol. */
struct Nodo {
  Nodo* parent = nullptr;
  array<Nodo*, sigma> next = {};
  long long priority = -1;
  string* str = nullptr;
  Nodo* bestTerminal = nullptr;
  long long bestPriority = -1;
};
