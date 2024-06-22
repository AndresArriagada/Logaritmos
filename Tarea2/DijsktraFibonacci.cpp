//incluir imports

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <list>
#include <climits>
#include <unordered_map>
using namespace std;
using namespace chrono;




//definiciones de estructuras y funciones
//Nodo: Nodo del grafo base. Contiene un entero que representa su llave, y un vector
//con las aristas que lo llevan a otros nodos
class Nodo {
public:
    
    int valor;
    //Cada valor del vector representa una arista, la arista es un par en el cual el primer
    //valor es un puntero al nodo que lo conecta, y el segundo valor es un entero que representa
    //el peso de la arista
    vector<pair<Nodo*, int>> aristas;

    Nodo(int v) : valor(v) {} // Constructor
};

//Grafo: estructura base de entrada compuesta por nodos y conectados entre si por aristas con 
//pesos. 
class Grafo {
public:
    //vector que contiene los nodos
    vector<Nodo*> nodos;


    //Metodo para agregar un nodo. Recibe el valor del nodo y lo crea sin aristas. Finalmente lo agrega 
    //a la lista de nodos
    void agregarNodo(int valor) {
        Nodo* nuevoNodo = new Nodo(valor);
        nodos.push_back(nuevoNodo);
    };
    
    
    //Metodo para agregar una arista. Recibe los dos nodos que se van a conectar y el peso
    void agregarArista(Nodo* nodoOrigen, Nodo* nodoDestino, int peso) {
        //Solo lo hacemos en una direccion, ya que el no hacerlo dirigido se maneja en la 
        //seccion del experimento
        nodoOrigen->aristas.push_back(make_pair(nodoDestino, peso));
    };
    
    //Metodo que retorna el vector de los nodos del grafo
    vector<Nodo*> obtenerNodos() {
        return nodos;
    }
};



//NodoFib: Nodo de la cola de fibonacci
//Para el nodo de la cola de fibonacci, debemos contener:
    //Distancia: es el Key por el que se ordena el arbol
    //Nodo: puntero al nodo inicial
    //Punteros de la estructura: Estos conectan al padre, a un hijo, al nodo a la derecha y al nodo a la izquierda
 struct FibNode{
    int distancia;   //Representa la distancia al nodo inicial. A la vez es la llave.
    Nodo* nodo;      //Nodo al que se esta representando con FibNode
    FibNode* parent;   // Puntero al nodo padre
    FibNode* child;    // Puntero a uno de los hijos del nodo
    FibNode* left;     // Puntero al nodo izquierdo en la lista de hermanos
    FibNode* right;    // Puntero al nodo derecho en la lista de hermanos
    int degree;     // Grado del nodo (número de hijos que tiene)
    bool mark;      // Indica si el nodo ha perdido un hijo desde la última vez que fue hecho hijo de otro nodo

    //Constructor de un nodo. Recibe un par que tiene la distancia y el nodo. Los guarda en donde corresponde y asigna los 
    //valores de inicializacion correspondientes.
    FibNode(pair<int, Nodo*> p): 
    distancia(p.first), 
    nodo(p.second), 
    parent(nullptr), 
    child(nullptr), 
    left(this), 
    right(this), 
    degree(0), 
    mark(false) {}
};    



//ColaFibonacci: Cola de fibonacci y todas sus funciones
//La cola de fibonacci debe tener:
class FibHeap {
    public:
    //Make: constructor de una cola vacia
    FibHeap() : minNode(nullptr), nodeCount(0) {};

    //insert: agrega un nodo a la lista de raices. Ademas retorna la direccion del nodo
    FibNode* insert(pair<int,Nodo*> p) {
        //Recibimos el nodo y el peso, para partir debemos crear el nodo de fibonacci que representa este nodo
        FibNode* nodo = new FibNode(p);

        //Vemos si existe al menos un nodo, verificando si existe el minimo
        if (minNode==nullptr){//en caso que no, definimos este como minimo y retornamos
            minNode=nodo;
            nodeCount++;
            return nodo;
        } else {//Si entra aca, es porque hay nodos en la cola
            //Para agregarlo, lo uniremos a la izquierda del nodo minimo
            minNode -> left -> right = nodo;
            nodo -> left = minNode ->left;
            nodo -> right = minNode;
            minNode -> left = nodo;

            //Ya en este punto aumentamos la lista, por lo que procedemos a aumentar el contador
            nodeCount++;

            //Y para terminar, vemos si el nodo nuevo es menor que el nodo minimo, y en caso de ser asi redefinimos nuestro nodo minimo

            if (nodo -> distancia < minNode -> distancia) {
                minNode = nodo;
            }

            return nodo;
        }
    }
    
    //getMin: retorna el minimo
    FibNode* getMin() {
        return minNode;
    }

    //Metodo para eliminar el minimo de la cola
    void deleteMin() {
        //Caso de cola vacia
        if (nodeCount==0) return;

        //Si tiene hijos, los movemos todos a la lista de raices
        if (minNode->child) {
            //tomamos el hijo al que esta apuntando
            FibNode* child = minNode->child;
            child->parent = nullptr;
            int valorNodo = child -> nodo -> valor;
            cout << valorNodo << endl;
            child= child->right;

            //Mientras no estemos en nodo a la izquierda del hijo
            while(valorNodo != child -> nodo -> valor){
                //quitamos las referencias a los padres
                child->parent = nullptr;
                //pasamos al siguiente
                child = child->right;
                cout<<child->nodo->valor << " " << valorNodo <<endl;
            }

            //Unimos las dos listas
            minNode->left->right = minNode->child->left;
            minNode->child->left->right = minNode->left;
            minNode->child->left = minNode;
            minNode->left = minNode->child;
        }
        //Luego de haber sacado todos los hijos, eliminamos el nodo de la lista
        minNode->left->right = minNode->right;
        minNode->right->left = minNode->left;

        //Y finalmente asignamos el nuevo minimo
        if (minNode == minNode->right) {//En caso de ser el unico nodo, entonces apuntamos al nullptr
            minNode = nullptr;
        } else { //En caso que no sea el unico nodo, definimos un nuevo minimo arbitrario y consolidamos el arbol para asegurar que se mantenga la estructura y poder obtener el verdadero minimo
            FibNode* oldMin = minNode; 
            minNode = oldMin->right;
            consolidate();
        }
        --nodeCount;//finalmente, disminuimos en 1 el conteo total de nodos
    }

    //union: une dos colas
    void unirColas(FibHeap& nueva) {
        //Para unir dos filas usaremos la misma logica de unir que se uso en deleteMin.

        //Lo primero es ver si la nueva cola esta vacia, si es asi no se hace nada
        if(nueva.nodeCount == 0) {return;}

        //Si no lo esta, vemos si nuestra cola esta vacia, de ser asi tomamos la nueva como la cola final
        if(nodeCount == 0) {
            minNode = nueva.minNode;
            nodeCount += nueva.nodeCount;
            nueva.minNode = nullptr;
            nueva.nodeCount = 0;
            return;
            }

        //Ya habiendo cubierto los dos casos base, procedemos a la logica de dos colas no vacias

        else{
            //Hacemos las modificaciones de los punteros de la misma forma que en delMin
            minNode -> left -> right = nueva.minNode -> left;
            nueva.minNode -> left -> right = minNode -> left;
            nueva.minNode -> left = minNode;
            minNode -> left = nueva.minNode;
            
            //verificamos el nuevo minimo
            //Si el minimo de la nueva lista es menor, lo definimos como minimo global de la lista unida
            if(nueva.minNode -> distancia < minNode -> distancia) {
                minNode = nueva.minNode;
            }
            
            //sumamos la cantidad de nodos
            nodeCount += nueva.nodeCount;

            //eliminamos el puntero al nodo minimo y el total de nodos de la lista unida para eliminarla y retornamos
            nueva.minNode = nullptr;
            nueva.nodeCount = 0;
            return;
        }
    };



    //Esta funcion recibe un nodo y un valor. Este valor es un intento de disminuir la llave. En caso que no sea menor que la llave actual, no hace nada. En caso que sea menor: lo cambia, en caso que no sea parte de la lista de raices y su raiz pasa a ser menor que la del padre lo corta para agregarlo, y verifica si es el nuevo minimo
    void decreaseKey(FibNode* node, int newKey) {
        //si la nueva llave es mayor que la distancia guardada, no hacemos nada
        if (newKey > node->distancia) {
            return;
        }
        //En caso que sea menor, lo cambiamos
        node->distancia = newKey;

        //Si tiene un padre, y al actualizar la distancia pasa a ser menor que la de este, entonces entra aca
        if (node->parent !=nullptr) {//Si tiene padre
            if (node->distancia < node -> parent-> distancia) {
                FibNode* parent = node->parent;
                cut(node, parent);
                cascadingCut(parent);

                //Si ahora es menor que el padre, entonces existe la posibilidad que sea el nuevo minimo. De ser asi, entonces verificamos para definirlo como el nuevo

                //Lo guarde como nueva variable porque me estaba dando segmentation fault y queria ver en que acceso eras
                int nueva = node->distancia;
                if(nueva < minNode->distancia) {
                    minNode = node;
                }
                }
        }
        
        
    };


    //Verifica si esta vacia:
    bool isEmpty() {
        return minNode == nullptr;
    }

    
    //Puntero al nodo minimo
    FibNode* minNode;
    //Entero que representa cuantos nodos hay
    int nodeCount;

     //Metodo para cortar un nodo de un arbol y lo mueve a la lista de raices
    void cut(FibNode* node, FibNode* parent) {
        //Verifica si es el unico hijo
        if (node->right == node) {
            parent->child = nullptr;
        } else {//Si no lo es, entonces reconecta entre si los nodos a sus lados
            node->left->right = node->right;
            node->right->left = node->left;
            if (parent->child == node) {//En caso que el parent este apuntando a este, lo apunta al siguiente
                parent->child = node->right;
            }
        }
        parent->degree--;//disminuye en 1 el contador de hijos del padre

        //Lo agregamos a la lista de raices
        minNode->left->right = node;
        node->left = minNode->left;
        node->right = minNode;
        minNode->left = node;
        //Quitamos el puntero al padre
        node->parent = nullptr;
        //Quitamos la marca
        node->mark = false;
    }

    //A la vez, tenemos el cascadingCut que usaremos como metodo para balancear el arbol. Si un nodo le cortan 2 hijos lo tomaremos como que esta mal balanceado y lo agregaremos a la lista de raices como nuevo arbol. Para eso usaremos el mark para señalar si este nodo ya tuvo un corte en uno de sus hijos
    void cascadingCut(FibNode* node) {
        if(node->parent != nullptr){
            FibNode* parent = node->parent;
            
            if (!node->mark) {
                node->mark = true;
            } else { //si esta marcado, lo cortamos y procedemos a aplicar cascadingCut al padre
                cut(node, parent);
                cascadingCut(parent);
            }
        }
    }
    

    //Metodo de consolidacion del heap. Con esto nos aseguramos de mantener la estrucutura y preservar la eficiencia de la misma
    void consolidate() {
        //Creamos una tabla de grados que puede recibir hasta 2^15 nodos
        vector<FibNode*> degreeTable(15, nullptr);  
        
        //Creamos una lista de nodos
        list<FibNode*> nodeList;
        //partimos desde el nodo minimo
        FibNode* curr = minNode;
        //En esta parte agregamos todos los nodos a nodeList
        nodeList.push_back(curr);
        int valorNodo = curr -> nodo -> valor;
        cout << valorNodo << endl;
        curr = curr->right;

        while (valorNodo != curr -> nodo -> valor){
            nodeList.push_back(curr);
            curr = curr->right;
             cout<< valorNodo << " " << curr->nodo->valor<<endl;;
        }

        //Usa la degreeTable para asegurar que no hay 2 arboles del mismo grado
        for (auto& node : nodeList) {
            //X es el nodo que vamos a agregar y degree su grado
            FibNode* x = node;
            int degree = x->degree;

            //verificamos si ya se agrego un arbol de este grado a la tabla de grados
            while (degreeTable[degree]) {
                //De ser asi, entonces debemos juntarlos en un nuevo arbol de grado superior
                //tomamos el que esta en la tabla
                FibNode* y = degreeTable[degree];
                //Juntamos los dos arboles, colgando uno de la raiz del otro
                //Si la llave de X es mayor que la de y
                if (x->distancia > y->distancia) {
                    //agregamos a x como hijo de y, manteniendo la llave de y como raiz
                    link(x, y);
                } else {
                    //agrega y como hijo de x
                    link(y, x);
                }
                //Dejamos vacio la celda de ese grado en la tabla
                degreeTable[degree] = nullptr;
                //ya que juntamos dos arboles, aumentamos en 1 el grado
                degree++;
                //Y ahora se reinicia el loop, revisando la siguiente celda, hasta que se consiga una vacia
            }
            //cuando se consigue una vacia, se agrega a la lista y se pasa al siguiente nodo
            degreeTable[degree] = x;
        }


        //Finalmente, ya teniendo la tabla de grados completa, reconstruimos la lista de raices de nuestra cola de fibonacci
        //partimos con un puntero nulo
        minNode = nullptr;
        //recorremos toda la tabla
        for (auto& node : degreeTable) {
            if (node) {
                //vemos si el minnode es nulo
                if (!minNode) {
                    minNode = node;
                } else { //si no, entonces procedemos a agregar este nodo en la lista de raices

                    //lo quitamos de la lista que pertenezca en este momento
                    node->left->right = node->right;
                    node->right->left = node->left;

                    //lo insertamos a la lista de raices
                    minNode->left->right = node;
                    node->right = minNode;
                    node->left = minNode->left;
                    minNode->left = node;

                    //finalmente, si la distancia del nodo nuevo es menor, entonces pasa a ser el nuevo minNode
                    if (node->distancia < minNode->distancia) {
                        minNode = node;
                    }
                }
            }
        }
    }

    
    //conecta Y de la lista de raices como hijo de x, que tambien estaria en la lista de raices
    void link(FibNode* y, FibNode* x) {
        //Parte quitando al nodo y de la lista de raices y agregandole X como padre
        y->left->right = y->right;
        y->right->left = y->left;
        y->parent = x;

        //si X no tiene hijos, entonces es el unico hijo de x y se agrega como tal
        if (!x->child) {
            x->child = y;
            y->right = y;
            y->left = y;
        } else {//en caso que si tenga hijos, agregamos y a la lista de hijos de x
            x -> child -> left -> right = y;
            y -> left = x -> child -> left;
            x -> child -> left = y;
            y -> right = x -> child; 
        }
        //aumentamos el grado de x en 1 y cambiamos la marca y a false
        x->degree++;
        y->mark = false;
    }



};




//Logica de Dijkstra con fibonacci


//Esta funcion hace toda la logica del algoritmo de Dijkstra, usando las colas de fibonacci y devuelve las dos listas que se piden en el enunciado
//Recibe la direccion del grafo con el que vamos a trabajar, y la raiz desde donde se parte
pair<vector<int>,vector<int>> DijkstraFibHeap(Grafo& grafo, int raiz){
    //Obtenemos primero el numero de nodos en el grafo
    int n = grafo.nodos.size();
    //creamos el vector de distancias, inicializandolo con todas en el maximo permitido por int
    vector<int> distancias(n, INT_MAX);
    //Creamos el vector de previos, en el que todos partiran con -1 para indicar que aun no se hace dicho recorrido
    vector<int> previos(n, -1);

    //Para poder tener una forma de acceder en tiempo constante a los nodos del arbol, creamos un unordered map, donde la key sera el valor del nodo, y se tendra en este el puntero del nodo

    unordered_map<int, FibNode*> node_map;

    //Creamos el FibHeap
    FibHeap fibHeap;
    
    //Definimos la distancia del nodo raíz como 0, su nodo previo como −1, y agregamos el par (distancia = 0, nodo = raíz) a Q
    distancias[raiz] = 0;
    previos[raiz] = -1;
    node_map[raiz] = fibHeap.insert(make_pair(INT_MAX, grafo.nodos[raiz]));

    //ahora agregamos todos los demas nodos
    for (int i=0; i < n; i++) {
        if (i != raiz) {
            //PAra cada nodo que no es la raiz, su distancia debe ser infinito que representmos con INT_MAX, el nodo previo -1 y se agrega a Q
            //Como ya inicializamos las listas con estos valores, solo falta agregarlo
            node_map[i] = fibHeap.insert(make_pair(INT_MAX, grafo.nodos[i]));
        }
    }

    //Ahora iteraremos sobre la cola de fibonacci hasta que se vacie
    while(!fibHeap.isEmpty()){
        //Extraemos el minimo y lo eliminamos de la cola
        FibNode* minNode = fibHeap.getMin();
        //Extraemos el nodo de adentro del FibNode
        Nodo* actual = minNode -> nodo;
        //eliminamos el minimo
        fibHeap.deleteMin();

        //Ahora iteramos sobre la lista de vecinos del Nodo
        for(auto arista : actual->aristas) {
            //Por casa arista, extraemos el nodo al que apunta y el peso:
            Nodo* vecino = arista.first;
            int peso = arista.second;

            //Ahora debemos ver si la distancia guardada en el arreglo de distancias para el vecino es mayor que la distancia hasta el nodo actual mas el peso. Esto para ver si el camino que estamos actualmente es mas eficiente:
            if (distancias[vecino->valor] > distancias[actual->valor] + peso) {
                //Ya que es mayor, la debemos actualizar a esta nueva distancia
                //Definimos el nodo actual como el previo de su vecino
                previos[vecino->valor] = actual->valor;
                //Definimos la distancia nueva como la distancia hasta el actual mas el peso de la arista con el vecino
                distancias[vecino->valor] = distancias[actual->valor]+ peso;

                //Finalmente, actualizamos la distancia del nodo en la estructura
                fibHeap.decreaseKey(node_map[vecino->valor], distancias[vecino->valor]);
                //y esto se repite para cada arista del nodo actual
            }
        }

        //Este ciclo se repite hasta que se vacie la estructura
    }

    //Ya habiendo recorrido toda la estructura, podemos entregar las listas de distancias y previos como resultado

    return make_pair(previos,distancias);
}





//experimento

//Aca incluiremos la logica del experimento y un main para ejecutarlo


void experimento() {
    vector<int> sizes = {20}; // v = 2^10, 2^12, 2^14
    vector<int> edgesPowers = {2}; // e = 2^16 a 2^22
    int repetitions = 2;

    for (int i : sizes) {
        cout << i << endl;
        for (int exp : edgesPowers) {
            cout << i << exp << endl;
            int e = 1 << exp; // Calcula 2^j aristas

            for (int r = 0; r < repetitions; ++r) {
                Grafo grafo;
                for (int n = 0; n < i; ++n) {
                    grafo.agregarNodo(n);
                }

                // Generar un grafo conexo, empezando con un árbol de expansión mínima
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dist(0, i - 1);

                // Asegurar la conectividad primero
                for (int n = 1; n < i; ++n) {
                    int u = dist(gen) % n; // Conectar con un nodo anterior
                    int peso = dist(gen) % 100 + 1;
                    grafo.agregarArista(grafo.nodos[u], grafo.nodos[n], peso);
                    grafo.agregarArista(grafo.nodos[n], grafo.nodos[u], peso); // Hacerlo no dirigido
                }

                // Añadir aristas adicionales
                for (int j = 0; j < e - (i - 1); ++j) {
                    int u = dist(gen);
                    int w = dist(gen);
                    while (u == w) { w = dist(gen); } // Evitar bucles
                    int peso = dist(gen) % 100 + 1;
                    grafo.agregarArista(grafo.nodos[u], grafo.nodos[w], peso);
                    grafo.agregarArista(grafo.nodos[w], grafo.nodos[u], peso); // Hacerlo no dirigido
                }

                // Medir tiempo de ejecución del algoritmo de Dijkstra
                auto start = high_resolution_clock::now();
                auto resultados = DijkstraFibHeap(grafo, 0); // Ejecutar Dijkstra desde el nodo 0
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);

                cout << "Tiempo para 2^" << log2(i) << " nodos y 2^" << exp << " aristas, rep " << (r + 1) << ": " << duration.count() << " milisegundos" << endl;
            }
        }
    }
}

int main() {
    experimento();
    return 0;
}