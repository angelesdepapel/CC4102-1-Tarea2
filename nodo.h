#pragma once
#include <array>
#include <string>
using namespace std;

/**
 * Para esta tarea se utilizarán datasets en inglés,
 * por lo que el tamaño del alfabeto es 27 (26 letras
 * más el carácter especial $)
 */
const int sigma = 27;

struct Nodo{
    Nodo* parent;
    std::array<Nodo*, sigma> next;
    long long priority;
    std::string* str;
    Nodo* best_terminal;
    long long best_priority;
};