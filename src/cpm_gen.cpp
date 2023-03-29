#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <map>

using namespace std;

multimap<string, int> load_model(string input_file) {
	multimap<string, int> sequences_lookahead;

	ifstream infile(input_file);
	string line;

	while (getline(infile, line))
	{
		int c = 0;
		string key;

		stringstream ss(line);
		string word;
		while (ss >> word) {
			if (c == 0) {
				key = word;
			} else {
				sequences_lookahead.insert({key, stoi(word)});
			}

			c++;
    	}
	}

	return sequences_lookahead;
}

char get_next_character_prediction(string file_buffer, pair<multimap<string, int>::iterator, multimap<string, int>::iterator> iterator)
{
	char chosen_ch = file_buffer[iterator.first->second+1];				
	return chosen_ch;

}

int main(int argc, char **argv)
{
	cout << "Loading model '" << argv[1] << "'" << endl;
	multimap<string, int> sequences_lookahead = load_model(argv[1]);
	cout << "Model is loaded" << endl;

	// Ler ficheiro original - o modelo tem pointers pra ele
	std::ifstream t(argv[2]);
	std::stringstream buffer;
	buffer << t.rdbuf();
	string original_file = buffer.str();

	// Outras settings - seq é o fim da string que vamos continuar, size é o length a gerar e k o tamanho da window igual a do modelo
	string seq = argv[3];
	int generation_size = atoi(argv[4]);
	int k = atoi(argv[5]);

	cout << "Settings loaded" << endl;

	for (int i=0; i < generation_size; i++) {
		char next_character_prevision = get_next_character_prediction(original_file, sequences_lookahead.equal_range(seq.substr(seq.size()-k, seq.size())));
		seq += next_character_prevision;
	}

	cout << seq << endl;

}
