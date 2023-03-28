#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <map>
#include <iterator>
#include <random>
#include <vector>

using namespace std;

struct symbol_t {
	int numHits;
	int numFails;
	int numBits;
};

char get_next_character_prediction(pair<multimap<string, char>::iterator, multimap<string, char>::iterator> iterator, string key, int size)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distr(0, size);

	int randIdx = distr(gen);

	while (randIdx > 1) {
		advance(iterator.first, 1);
		if ((*iterator.first).first == key) randIdx--;
	}

	return (*iterator.first).second;
}

int main(int argc, char **argv) {

	// Command line arguments
	const char *filename = argv[1];						// file to predict and compare
	const int k = atoi(argv[2]);						// size of the sliding window


	int file_lenght;	

	ifstream file (filename);

	if (!file.is_open()) {
		cout << "[ERROR] Can't open file '" << filename << "'" << endl;
		exit(EXIT_FAILURE);
	}

	file.seekg(0, ios::end);
	file_lenght = file.tellg();

	cout << "file has size" << file_lenght << "\n" << endl;

	file.seekg(0, ios::beg);

    char ch;
	int Nh = 0;											// number of hits
	int Nf = 0;											// number of fails		
	int pointer = 0;						

	multimap<string, char> sequences;	
	//multimap<string, struct symbol_t> symbols;
		

    // Create a vector to store the characters
    vector<char> characters;

    // Read and store each character in the vector

    while (true) {

		window = buffer.substr(i, k);

		n_previous_encounters = sequences.count(window);

		next_character = buffer.at(i + k);

		cout << window;
		if (n_previous_encounters > 0) {
			next_character_prevision = get_next_character_prediction(sequences.equal_range(window), window, n_previous_encounters);
			
			if (next_character_prevision == next_character) Nh++;
			else Nf++;

			cout << "(" << next_character_prevision << ")";
		}
		cout << endl;

		sequences.insert({window, next_character});

		

        streampos pos = file.tellg();

        if (file.get(ch)) {
			cout << "[" << pos << "] char: " << ch << "\n" << endl;
	            //characters.insert(characters.begin() + pos, c);
        } else {
            break;
        }

		if (file.eof()) { // check if end-of-file has been reached
            break;
        }
    }

    file.close();

	return 0;
	
}