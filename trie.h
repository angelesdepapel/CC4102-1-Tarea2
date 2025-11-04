#pragma once
#include "nodo.h"
#include <string>
#include <vector>

//Tipos de prioridad
enum class Prioridad{
    TIEMPO,
    ACCESOS
};

//Clase que representa un trie
class Trie{
private:
    Nodo* raiz;
    Prioridad prio;
    long long cantidad_accesos;
    int cantidad_nodos;
    std::vector<Nodo*> nodos;


    /** Dado un caracter
     * retorna su índice correspondiente en el arreglo next
     */
    int getIndex(char c){
        if(c == '$') return 26;
        else return c - 'a';
    }


    /** Dado un índice del arreglo next
     * retorna su caracter correspondiente
     */
    char getChar(int i){
        if(i == 26) return '$';
        else return 'a' + i;
    }


    /** Dado un nodo, actualiza best_terminal y best_priority
     * recursivamente hacia arriba, hasta llegar a la raiz
     */
    void update(Nodo* v){
        Nodo* actual = v->parent;
        long long newPriority = v->priority;
        Nodo* newTerminal = v;

        while(actual != nullptr){
            if(actual->best_priority < newPriority){
                actual->best_priority = newPriority;
                actual->best_terminal = newTerminal;
                actual = actual->parent;
            }else{
                break;
            }
        }
    }

    public:
        /** Constructor del trie
         */
        Trie(Prioridad tipo = Prioridad::TIEMPO): prio(tipo), cantidad_accesos(0), cantidad_nodos(0){
            raiz = new Nodo();
            raiz->parent = nullptr;
            raiz->priority = -1;
            raiz->str = nullptr;
            raiz->best_terminal = nullptr;
            raiz->best_priority = -1;

            for(int i = 0; i<sigma; i++){
                raiz->next[i] = nullptr;
            }

            cantidad_nodos++;
            nodos.push_back(raiz);
        }


        /** Inserta una nueva palabra en el trie
         */
        void insert(const std::string& w){
            Nodo* actual = raiz;

            // Insertamos la palabra caracter a caracter
            for(char c : w){
                int id = getIndex(c);

                if(actual->next[id] == nullptr){
                    Nodo* nuevo = new Nodo();
                    nuevo->parent = actual;
                    nuevo->priority = -1;
                    nuevo->str = nullptr;
                    nuevo->best_terminal = nullptr;
                    nuevo->best_priority = -1;

                    for(int i = 0; i < sigma; i++){
                        nuevo->next[i] = nullptr;
                    }

                    actual->next[id] = nuevo;
                    cantidad_nodos++;
                    nodos.push_back(nuevo);
                }
                actual = actual->next[id];
            }

            // Insertamos el caracter especial al final
            int id = 26; //índice de '$'
            if(actual->next[id] == nullptr){
                Nodo* terminal = new Nodo();
                terminal->parent = actual;
                terminal->priority = 0;
                terminal->str = new std::string(w + "$");
                terminal->best_terminal = terminal;
                terminal->best_priority = 0;

                for(int i = 0; i < sigma; i++){
                    terminal->next[id] = nullptr;
                }

                actual->next[id] = terminal;
                cantidad_nodos++;
                nodos.push_back(terminal);

                if(actual->best_terminal == nullptr || actual->best_priority < 0){
                    actual->best_terminal = terminal;
                    actual->best_priority = 0;
                    update(terminal);
                }
            }
        }


        /** Dado un puntero a un nodo,
         * retorna el puntero resultante de descender por el caracter c.
         */
        Nodo* descend(Nodo* v, char c){
            if(v == nullptr) return nullptr;
            int id = getIndex(c);
            return v->next[id];
        }

        
        /** Dado un subarbol,
         * retorna un puntero a su mejor autocompletado.
         */
        Nodo* autocomplete(Nodo* v){
            if(v == nullptr) return nullptr;
            return v->best_terminal;
        }


        void update_priority(Nodo* v){
            if(v == nullptr) return;
            
            if(prio == Prioridad::ACCESOS){
                v->priority++;
                v->best_priority = v->priority;
            }else if(prio == Prioridad::TIEMPO){
                cantidad_accesos++;
                v->priority = cantidad_accesos;
                v->best_priority = cantidad_accesos;
            }

            update(v);
        }


        int get_cantidad_nodos() const{
            return cantidad_nodos;
        }
};