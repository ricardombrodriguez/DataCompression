#include <iostream>
#include <math.h>
#include <ifstream>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {

	// Check if program runs with the right number of command line args
	if (argc != 5) {
		cout << "Invalid number of arguments" << endl;
		exit(0);
	}

	// Command line arguments
	const char *filename = argv[1];				// file to predict and compare
	const int k = atoi(argv[2]);					// size of the sliding window
    const float alpha = atof(argv[3]);			// used for estimate_probability
	const float threshold = atof(argv[4]);		// limit the value of n_hits and n_fails
	
	// State variables
	map<char, float> alphabet_probabilities;	// dictionary that stores the alphabet symbols as keys and its probabilities as values
	vector<char> sequence;						// vector that stores the already processed sequences
	unordered_map<string, int[]> hash_table;	// hashtable that stores the position of a sequence of characters in the "sequence" vector
	int pointer = 1;							// points to the current starting position of the sequence we're analyzing
	int num_hits = 0;							
	int num_fails = 0;

	// Open the file
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Can't open file " << filename << endl;
        return 1;
    }

	// File buffer
	string buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	// Iterate through the file buffer using a k-size sliding window
    for (int i = 0; i <= buffer.length() - k; i++) {

        string window = buffer.substr(i, k);
		vector<int> window_positions = get_position_from_hash_table(hash_table, window, pointer);
		char next_character = estimate_next_character(alphabet_probabilities, sequence, window_positions, k);
        
		// Check if predicted character is equal (or not) to true value
		if (next_character == buffer[pointer + k]) {
			++num_hits;
		} else {
			++num_fails;
		}
		
		float probability = estimate_probability(num_hits, num_fails, alpha);
		float amount_information = estimate_amount_information(probability);
		pointer = pointer + 1;

    }

}

float estimate_probability(int num_hits, int num_fails, const float alpha) {
	return (num_hits + alpha) / (num_hits + num_fails + 2 * alpha);
}

float estimate_amount_information(float probability) {
	return -log2(probability);
}

// Inserts the position of the current sliding window in the hash table
// If it exists, it retrieves the previous position(s) of the sequence
int[] get_position_from_hash_table(unordered_map<string,int[]> hash_table, string window, int pointer) {

	if (hash_table.contains(window)) {
		vector<int> old_window_positions = hash_table[window];
		vector<int> new_window_positions = old_window_positions;
		new_window_positions.push_back(pointer);
		hash_table[window] = new_window_positions;
		return old_window_positions;

	} else {
		vector<int> window_positions = { pointer };
		hash_table[window] = window_positions;
		return NULL;
	}

}

/* 
Given a "sequence" of all the processed characters and the starting "window_positions" (ex: [14, 82, 91]) in "sequence" 
Get the list of all characters after each window_position + k. This represents the set of possible next characters
For example, if the list of possible next characters is [" ","a"], return the one which has a higher probability in the alphabet
*/ 
char estimate_next_character(map<char,float> alphabet_probabilities, vector<char> sequence, vector<int> window_positions, int k) {1

	vector<char> next_characters;										// Stores all possible next characters
	for (int pos : window_positions) {
		int next_character_index = pos + k;
		next_characters.push_back( sequence[next_character_index] );
	}	

	char best_char;
	float best_probability;
	for (int ch : next_characters) {
		float ch_probability = alphabet_probabilities[ch];
		if (ch_probability > best_probability || best_probability == NULL) {
			best_char = ch;
			best_probability = ch_probability;
		}
	}

	/*
	todo:
	ter em conta se o caractere existe mais que uma vez no next_characters -> deve ter maior probabilidade de ser escolhido
	caso window_positions seja NULL (a sequencia ainda n tinha aparecido), escolher charactere random tendo em conta distribuição de probabilidades?
	ou ter em conta um k mais pequeno da sequência???
	*/ 

	return best_char;

}