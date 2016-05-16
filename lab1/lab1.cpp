#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <getopt.h>

using namespace std;

string ReadFile(string q)
{
	string word="";
	string w="";
	ifstream in(q.c_str());
	while(!in.eof())
	{
		getline(in,w);
		word=word+w;
	}
	in.close();
	return word;
}

void WriteFile(string data, string fileName)
{
	ofstream of(fileName.c_str());
	of<<data;
	of.close();
}


string Encryption(string word, string key)
{
	int size;
	string shifr="";
	if (key.size() > word.size())
		size = key.size();
	else
		size = word.size();
	for(int i=0; i<size; i++)
		if(word[i % word.size()]>=97 && word[i % word.size()]<=122)
		{
			shifr+=(word[i % word.size()]%97+key[i % key.size()]%97)%26+97;
		}
		else
			shifr+=word[i];
	return shifr;
}

string Decryption(string word, string key)
{
	int size;
	string shifr="";
	if (key.size() > word.size())
		size = key.size();
	else
		size = word.size();
	for(int i=0; i<size; i++)
		if(word[i % word.size()]>=97 && word[i % word.size()]<=122)
		{
			shifr+=(word[i % word.size()]%97+26-key[i % key.size()]%97)%26+97;
		}
		else
			shifr+=word[i];
	return shifr;
}




int main (int argc, char **argv)
{
	string key_namefile="";
	string key="";
	string file="";
	string fileName="";
	string word="";
	string shifr="";
	bool e=0;
	bool d=0;
	int c = 0;
    while ((c = getopt(argc, argv, "k:f:o:ed"))!= -1) 
    {
        switch (c) 
        {
            case 'k':
                key_namefile = optarg;
                key = ReadFile(key_namefile);
                break;
            case 'f':
                file = optarg;
                word = ReadFile(file);
                break;
            case 'o':
            	fileName=optarg;
            	break;
            case 'e':
            {
            	e=1;
                shifr=Encryption(word,key);
                WriteFile(shifr,fileName);
                break;
            }
            case 'd':
            {
            	d=1;
                shifr=Decryption(word,key);
                WriteFile(shifr,fileName);
                break;
            }
            case '?':
                if (optopt == 'k' || optopt == 'f' || optopt == 'o')
                    fprintf (stderr,"Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr,"Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                printf("Неизвестная опция");
                break;
        }
        if (argc < 5)
        {
        	cout<<"Введены не все аргументы.\nВведите:\n./program_name -k key_fileName -f word_fileName -o output_fileName -e/-d\n";
        	return 1;
        }
        if(e && d)
        {
        	cout<<"Введите либо -e, либо -d\n";
        	return 1;
        }
        if (!e && !d)
        {
        	cout<<"Введите -e или -d\n";
        	return -1;
        }
    }
    return 0;
}
