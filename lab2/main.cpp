#include "DESCoder.h"
#include "string.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "getopt.h"

using namespace std;

bool readFile(char* fileName, vector<unsigned char> &fileContent);
bool writeFile(char* fileName, vector<unsigned char> &fileContent);

vector <unsigned char> encodeVector (vector<unsigned char>& input, DESCoder& coder);
vector <unsigned char> decodeVector (vector<unsigned char>& input, DESCoder& coder);

int main(int argc, char* argv[])
{
	char* inFileName=NULL;
	char* outFileName=NULL;
	char* keyFileName=NULL;
	bool encode;
	int opt = getopt( argc, argv, "i:o:k:de");
	while( opt != -1 ) 
	{
		switch( opt ) 
		{
		case 'i':
			inFileName = optarg;
			break;
		case 'o':
			outFileName = optarg;
			break;
		case 'k':
			keyFileName = optarg;
			break;
		case 'd':
			encode = false;
			break;
		case 'e':
			encode = true;
			break;
		}
		opt = getopt( argc, argv, "i:o:k:de");
	}

	if (inFileName == NULL || outFileName == NULL || keyFileName == NULL)
	{
		cerr << "Wrong arguments" << endl;
		cout << "Usage: " << endl;
		cout << "programName -i <input> -o <output> -k <key> [ -e | -d ]" << endl;
		cout << "Parameters:" << endl;
		cout << "\t<input>  \t file name for input file" << endl;
		cout << "\t<output> \t file name for output file" << endl;
		cout << "\t<key>    \t file name for file with key " << endl;
		cout << "\tUse key \"e\" for encoding, \"d\" for decoding" << endl << endl;
		return false;
	}	
	vector<unsigned char> inputContent;
	if (! readFile(inFileName, inputContent) )
	{
		cerr << "Can't open input file: " << inFileName << endl;
		return -1;
	}
	vector<unsigned char> key;
	if (!readFile(keyFileName, key) )
	{
		cerr << "Can't open key file: " << keyFileName << endl;
		return -1;
	}
	if (key.size()-1 != 8)
	{
		cerr << "Key must be 8 byte value!" << endl;
		return -1;
	}
	key.erase(key.begin() + 8);

	// для шифрования и деширования используется один и тот же ключ
	// поэтому создаём общий "шифратор"
	DESCoder coder(key);

	vector<unsigned char> output; // куда будет записан результат шифрования/дешифрования

	if(encode)
	{
		// шифрование входного фалйа.
		output = encodeVector (inputContent, coder);
	}
	else
	{
		// дешифрование входного файла.
		// первым делом во входном файле записано количество нулей
		// в конце файла для выравнивания длины до кратной 8
		output = decodeVector(inputContent, coder);
	}
	if (!writeFile(outFileName, output) )
	{
		cerr << "Can't open output file: " << outFileName << endl;
		return -1;
	}
	return 0;
}

bool readFile(char* fileName, vector<unsigned char> &fileContent)
{
	ifstream inputStream(fileName, std::ios::binary);
	if (inputStream.fail())
		return false;


	inputStream.seekg(0, std::ios::end);
	int inputSize = inputStream.tellg();
	inputStream.seekg(0, std::ios::beg);

	if (!inputSize)
		return false;

	fileContent.resize(inputSize);
	inputStream.read ( (char*) &fileContent.front(), inputSize);
	inputStream.close();

	return true;
}

bool writeFile(char* fileName, vector<unsigned char> &fileContent)
{
	ofstream outStream(fileName, std::ios::binary);
	outStream.write((char*)&fileContent.front(), fileContent.size());
	outStream.close();
	return true;
}

vector <unsigned char> encodeVector (vector<unsigned char>& input, DESCoder& coder)
{
	// шифрование input шифратором coder
	// для этого необходимо разбить input на блоки по 8 байт
	// а остаток дополнить нулями.
	// количество нулей в конце запишем в начало результата для корректного дешифрования
	vector<unsigned char> output;
	output.push_back (8 - input.size() % 8);

	for (int i = 0; i < input.size() / 8; i++)
	{
		vector<unsigned char> dataPart8; // очередной 8-байтовый кусок
		dataPart8.insert(dataPart8.end(), 
			input.begin() + i * 8, input.begin() + (i + 1) * 8 );

		// шифруем кусочек из 8 байт
		vector<unsigned char> codedPart = coder.encode(dataPart8);

		// записываем его в результат
		output.insert(output.end(), codedPart.begin(), codedPart.end());
	}

	// теперь разберёмся с оставшимся кусочком
	if (input.size() % 8)
	{
		// сформируем кусок из 8 байт,
		// записав незашифрованные байты и дополнив нулями до длины 8
		vector<unsigned char> dataPart;
		dataPart.insert(dataPart.end(),
			input.end() - input.size() % 8, input.end());

		dataPart.insert(dataPart.end(), 8 - input.size() % 8, 0);

		vector<unsigned char> codedPart = coder.encode(dataPart);
		output.insert(output.end(), codedPart.begin(), codedPart.end());
	}

	return output;
}

vector <unsigned char> decodeVector (vector<unsigned char>& input, DESCoder& coder)
{
	// дешифрование вектора input шифратором coder
	// первым элементом в векторе input находится количество
	// нулей, которые пришлось вставить в конец шифруемого сообщения
	// для выравнивания длинны
	// затем идут несколько блоков по 8 байт - зашифрованные данные.
	// их-то и будем дешифровывать

	vector<unsigned char> output;
	for (int i = 0; i < (input.size() - 1) / 8; i++)
	{
		// формируем очередной кусочек из 8 байт
		vector<unsigned char> part8;
		part8.insert(part8.end(),
			input.begin() + i * 8 + 1, input.begin() + (i + 1) * 8 + 1 );

		// дешифруем его
		vector<unsigned char> decodedPart = coder.decode(part8);

		// записываем к результату
		output.insert(output.end(), decodedPart.begin(), decodedPart.end());
	}

	// избавляемся от лишних нулей в конце сообщения
	for (int i = 0; i < input[0]; i++)
		output.pop_back();

	return output;
}
