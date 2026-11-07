//Grupo 5 
//Ahí lo corrigen si me equivoqué 

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>

using namespace std;

//Se define una estructura de tipo NodoC para el arbol. La "C" indica que sera para compresion.
struct NodoC
{
	unsigned char caracter;		//Variable positiva que corresponde al byte del caracter ASCII
	long long int repeticion;	//Variable que guarda la aparicion del cada caracter
	NodoC* izquierdo;
	NodoC* derecho;

	/* Se define una funcion que permita crear nodos, esta alberga un caracter,
	la repeticion de ese caracter e inicializa los hijos, inicialmente en NULL.*/

	NodoC(unsigned char c, long long int r, NodoC* i = NULL, NodoC* d = NULL)
	{
		caracter = c;
		repeticion = r;
		izquierdo = i;
		derecho = d;
	}
};

/*Se define una estructura de tipo NodoD para el arbol.La "D" indica que sera para descompresion.
Tras esta definicion se realizan las mismas operaciones que en la struc anterior.*/

struct NodoD
{
	unsigned char caracter;
	NodoD* izquierdo;
	NodoD* derecho;
	NodoD(char c, NodoD* i = NULL, NodoD* d = NULL)
	{
		caracter = c;
		izquierdo = i;
		derecho = d;
	}
};


//----------------------------------------------- ° --------------------------------------------------------
//										COMPRESION DE ARCHIVOS
//----------------------------------------------- ° --------------------------------------------------------

/*Se crea la funcion que permitira acomodar los datos, y en esta se define un int para acomodar estos.
Ademas, el "if" permite acomodar segun la repeticion de caracteres.*/

void AcomodadoDat(vector<NodoC*>& A, int i, int longitud)
{
	int menor = i;

	if (2 * i + 1 <= longitud && A[2 * i + 1]->repeticion < A[i]->repeticion)
	{
		longitud = 2 * i + 1;
		if (2 * i + 2 <= longitud && A[2 * i + 2]->repeticion < A[2 * i + 1]->repeticion)
			menor = 2 * i + 2;
	}
	else if (2 * i + 2 <= longitud && A[2 * i + 2]->repeticion < A[i]->repeticion)
		menor = 2 * i + 2;

	if (menor != i)
	{
		swap(A[i], A[menor]);
		AcomodadoDat(A, menor, longitud);
	}
}

//Saca el codigo binario minimo, verifica que el valor recibido, si este es menor a 1, retorna NULL. Sino, se iguala a su valor de frecuencia.

NodoC* sacar_cb_min(vector<NodoC*>& A)
{
	if (A.size() < 1)
		return NULL;

	NodoC* minimo = A[0];
	A[0] = A.back();

	A.pop_back();
	AcomodadoDat(A, 0, A.size() - 1);
	return minimo;
}


void nodo_en_otro(vector<NodoC*>& A, NodoC* element)	//Funcion que inserta un nodo en otro.
{
	A.push_back(element);
	int i = A.size() - 1;
	while (i > 0 && A[(i - 1) / 2]->repeticion > A[i]->repeticion)
	{
		swap(A[i], A[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

void tree_body(vector<NodoC*>& A, int length) //Se crea el cuerpo del arbol, pasando como parametros la coleccion de nodos y su tamaño.
{
	for (int i = (length - 1) / 2; i >= 0; i--) //Variable igualada al tamaño.
	{
		AcomodadoDat(A, i, length);
	}
}

/*Guarda el archivo comprimido, y se crean tres condicionales, la primera en caso de tener un hijo izquierdo, guardarlo con una etiqueta "0",
la segunda, en caso de tener un hijo derecho, guardarlo con la etiqueta "1".*/

void guardar(NodoC* raiz, char codigo[], int indice, vector<long long int>& Huffman_codemap)
{
	if (raiz->izquierdo)
	{
		codigo[indice] = '0';
		guardar(raiz->izquierdo, codigo, indice + 1, Huffman_codemap);

	}

	if (raiz->derecho)
	{
		codigo[indice] = '1';
		guardar(raiz->derecho, codigo, indice + 1, Huffman_codemap);
	}

	/*Finalmente, si no hay hijos, el ciclo reduce "i" por cada vez que el indice aumento. 
	Y si el valor de la posicion es igual al indice se le asigna un valor de 1.*/

	if (!raiz->izquierdo && !raiz->derecho)
	{

		for (int i = indice; i >= 0; i--)
		{
			if (i != indice)
			{
				Huffman_codemap[raiz->caracter] *= 10;
				Huffman_codemap[raiz->caracter] += codigo[i] - '0';
			}
			else
				Huffman_codemap[raiz->caracter] = 1;
		}
	}
}

void store_tree(ofstream& ingreso, NodoC* raiz) //Creacion de una funcion de tipo void para escribir codigo binario en un arch ext.
{
	if (!raiz->izquierdo && !raiz->derecho)
	{
		ingreso << '1';
		ingreso << raiz->caracter;
	}
	else
	{
		ingreso << '0';
		store_tree(ingreso, raiz->izquierdo);
		store_tree(ingreso, raiz->derecho);
	}
}

NodoC* Huffman(long long int contar[]) //Asignacion de frecuencias, mediante inclusion y extraccion de nodos.
{
	vector<NodoC*>minheap;
	int i;

	for (i = 0; i < 256; i++)
	{
		if (contar[i] != 0)
		{
			minheap.push_back(new NodoC(i, contar[i]));
		}

	}

	tree_body(minheap, minheap.size() - 1);	//Cola para coleccion de nodos

	while (minheap.size() != 1)
	{
		NodoC* Z = new NodoC(-1, 0, sacar_cb_min(minheap), sacar_cb_min(minheap));
		Z->repeticion = Z->izquierdo->repeticion + Z->derecho->repeticion;
		nodo_en_otro(minheap, Z);
	}
	return(minheap[0]);	//Retorno de arbol de Huffman
}

void Write_compressed(ifstream& ingreso, ofstream& salida, vector<long long int >& Huffman_codemap)    //Funcion para escribir el archivo comprimido.
{
	char ch;
	unsigned char bits = 0;
	long long int counter = 0;

	while (ingreso.get(ch))
	{
		long long int temp = Huffman_codemap[static_cast<unsigned char>(ch)];	//Contador estatico

		while (temp != 1) //Ciclo que permite convertir los bits por medio de frecuencia
		{

			bits <<= 1;

			if ((temp % 10) != 0)
				bits |= 1;

			temp /= 10;

			counter++;

			if (counter == 8)
			{
				salida << bits;
				counter = bits = 0;
			}
		}
	}
	while (counter != 8)
	{
		bits <<= 1;
		counter++;
	}

	salida << bits;	//Guardado de bits en el nuevo archivo comprimido
	salida.close();
}

//----------------------------------------------- ° --------------------------------------------------------
//										DESCOMPRESION DE ARCHIVOS
//----------------------------------------------- ° --------------------------------------------------------

//Se crea el arbol usando el "NodoD" que anteriormente se señalo que era el asociado a la descompresion

NodoD* CrearArbol(ifstream& cual)
{
	char caracter;
	cual.get(caracter);
	if (caracter == '1')
	{
		cual.get(caracter);
		return (new NodoD(caracter));
	}
	else
	{
		NodoD* izquierdo = CrearArbol(cual);
		NodoD* derecho = CrearArbol(cual);
		return(new NodoD(-1, izquierdo, derecho));
	}
}

//Se crea una funcion "Decodificar" para como su nombre indica, decodificar los bytes.

void Decodificar(ifstream& ingreso, string archivo, NodoD* raiz, long long int t_repeticiones)
{
	ofstream salida((archivo.erase(archivo.size() - 4)).c_str(), ios::binary);	//Definicion de nuevo archivo.
	if (!salida.good())
	{
		perror("Error al ejecutar el arcivo, intente nuevamente.\t");
		exit(-1);
	}

//----------------------------------------------- ° --------------------------------------------------------
//									MANEJO BITS - DESCOMPRESION
//----------------------------------------------- ° --------------------------------------------------------

	bool error = false;
	char bits;
	NodoD* auxiliar = raiz;
	while (ingreso.get(bits))
	{
		int contador = 7;	//Contador para recorrer byte.

		while (contador >= 0)
		{

			//Si los hijos no son nulos, se comprime el archivo y se reduce la frecuencia a 1.

			if (!auxiliar->izquierdo && !auxiliar->derecho)
			{
				salida << auxiliar->caracter;
				t_repeticiones--;
				if (!t_repeticiones)
				{
					error = true;
					break;
				}
				auxiliar = raiz;
				continue;
			}

			/*Condicional que compara el bit del caracter y el movido, si es true, apunta al nodo de la derecha,
			y el contador se reduce, sino, apunta al nodo de la izquiera.*/

			if ((bits & (1 << contador)))
			{
				auxiliar = auxiliar->derecho;
				contador--;
			}
			else
			{
				auxiliar = auxiliar->izquierdo;
				contador--;
			}
		}
		if (error)
		{
			break;
		}
	}
	salida.close();
}

//----------------------------------------------- ° --------------------------------------------------------
//										FUNCION MAIN - CODIGO FUENTE
//----------------------------------------------- ° --------------------------------------------------------

int main(int argc, char* argv[])
{
	system("cls");

	int c;
	bool menu = true;

	while (menu)
	{
		system("CLS");
		system("color f3");
		cout << "\n\n\n\n\n\n\n\n\t\t\t\t\t  1. Opciones para comprimir" << endl;
		cout << "\t\t\t\t\t  2. Opciones para descomprimir" << endl;
		cout << "\t\t\t\t\t  3. Cerrar menu" << endl;
		cout << endl << "\t\t\t\t\t Seleccione la opcion que desea realizar: ";
		cin >> c;

		switch (c)
		{

//----------------------------------------------- ° --------------------------------------------------------
		/* En caso de seleccionar comprimir, se creara una variable "vector" que almacena
		los codigos de Huffman, define el tamaño maximo y crea un arreglo.*/
//----------------------------------------------- ° --------------------------------------------------------

		case 1:
		{
			system("cls");

			vector<long long int > Huffman_codemap;

			Huffman_codemap.resize(256);

			long long int Count[256] = { 0 };

			string filename;

			
			if (argc > 1) {

				filename = argv[1];
				cout << "El archivo recibido por consola es: " << filename << endl << endl;
			}
			else {
				cout << "Escriba el nombre del archivo a comprimir (y su extension .txt): ";
				cin >> filename;
			}

			ifstream input_file(filename.c_str(), ios::binary);

			//Si el archivo no existe se muestra un error y se cierra la consola.

			if (!input_file.good())
			{
				perror("Error 404, el archivo no existe.\t");
				exit(-1);
			}

			cout << "\nEl archivo se esta comprimiendo...";

			clock_t start_time = clock();
			char ch;

			while (input_file.get(ch))
				Count[static_cast<unsigned char>(ch)]++;

			//Limpieza de punteros

			input_file.clear();
			input_file.seekg(0);

			NodoC* tree = Huffman(Count);

			ofstream output_file((filename + ".cdar").c_str(), ios::binary);

			if (!output_file.good())
			{
				perror("Error.\t");
				exit(-1);
			}

			output_file << tree->repeticion;

			output_file << ',';

			store_tree(output_file, tree);

			output_file << ' '; 

			char single_code[16];	//Arreglo que guarda un codigo unico para los codigos de la coleccion del vector

			guardar(tree, single_code, 0, Huffman_codemap);

			Write_compressed(input_file, output_file, Huffman_codemap);

			//Inicia el cerrado de archivos, primero el archivo de ingreso, luego el de salida y luego se detiene el tiempo.

			input_file.close();

			output_file.close();

			clock_t stop_time = clock();

			//Si se borra el archivo original, muestra un error al usuario.

			if (remove(filename.c_str()) != 0)
				perror("Error 404, archivo original eliminado.\t");

			cout << "\n\n--------------------------------- El archivo fue comprimido con exito ---------------------------------\n\n";
			cout << "Se demoraron:\t" << double(stop_time - start_time) / CLOCKS_PER_SEC << " segundos en la compresion\n\n";
			system("pause");
			break;
		}
//----------------------------------------------- ° --------------------------------------------------------
		// En caso de seleccionar descomprimir, se creara una variable segun el archivo por descomprimir.
//----------------------------------------------- ° --------------------------------------------------------
		case 2:
		{
			system("cls");

			string filename;
			if (argc > 1) {
				filename = argv[1];
				cout << "El archivo recibido por consola es: " << filename << endl << endl;
			}
			else {
				cout << "Ingrese el nombre del archivo a descomprimir (y su extension .txt.cdar): ";
				cin >> filename;
			}

			ifstream input_file(filename.c_str(), ios::binary);

			//Si el archivo no se abre se le indica al usuario y se cierra la consola.

			if (!input_file.good())
			{
				perror("Error, no fue posible abrir el archivo seleccionado.\t");
				exit(-1);
			}

			if (filename.find(".cdar") == string::npos) 
			{
				cout << "INVALID_ACTION - El archivo ya fue descomprimido.\n\n";
				exit(-1);
			}

			cout << "\nEl archivo seleccionado esta en proceso de descompresion...";

			clock_t start_time = clock();

			long long int Total_freq = 0;

			char ch;

			while (input_file.get(ch))
			{
				if (ch == ',')
					break;

				Total_freq *= 10;
				Total_freq += ch - '0';
			}

			NodoD* Huffman_tree = CrearArbol(input_file);

			input_file.get(ch);

	//Llamado a "Decodificar", y se le pasan como parametros el arch. de entrada, el nombre de este, el arbol de huffman y las frecuencias totales.

			Decodificar(input_file, filename, Huffman_tree, Total_freq);

			input_file.close();

			clock_t stop_time = clock();

			if (remove(filename.c_str()) != 0)
				perror("Error, el archivo fue eliminado.\t");

			cout << "\n\n--------------------------------- El archivo fue descomprimido con exito ---------------------------------\n\n";
			cout << "Se demoraron:\t" << double(stop_time - start_time) / CLOCKS_PER_SEC << " segundos en la descompresion\n\n";
			system("pause");
			break;
		}

		case 3:
		{
			menu = false;
			break;
		}
		}
	}

}
