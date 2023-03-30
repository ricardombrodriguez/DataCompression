#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <map>
<<<<<<< HEAD

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

=======
#include <unordered_map>
#include <random>

using namespace std;

unordered_map<string, unordered_map<char, float>> load_model(string input_file)
{
	unordered_map<string, unordered_map<char, float>> sequences_lookahead;

	ifstream infile(input_file);
	string line;

	string seq;
	bool is_key;

	// Iterate over lines
	while (getline(infile, line))
	{
		// If this line is empty, next will be a key (base sequence)
		if (line.empty())
		{
			is_key = true;
			continue;
		}

		// Store key
		if (is_key)
		{
			seq = line;
			is_key = false;
			continue;
		}

		// Read all the elements of a line, separated by white space 
		// accoring to the format `{next_character} {probability}`
		int c = 0;
		char next;

		stringstream ss(line);
		string word;
		while (ss >> word)
		{
			if (c == 0) next = word[0];
			// Store the probability related to `next` character, assuming `seq` has happened
			else sequences_lookahead[seq][next] = stof(word);

			c++;
		}
	}

	return sequences_lookahead;
}

char get_next_character_prediction(unordered_map<char, float> next_characters)
{
	int multiplier = 10000;
	float prob_sum = 0;

	// Get the total value of probabilities
	for (auto const &p : next_characters)
	{
		prob_sum += p.second;
	}

	// We will multiply the floats by this number so as to preserve
	// most of the decimal cases' influence on the result
	int max_rand = prob_sum *= multiplier;

	// Generate random value
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<mt19937::result_type> dist(1, max_rand);

	int r = dist(rng);

	prob_sum = 0;

	for (auto const &p : next_characters)
	{
		prob_sum += p.second;

		if (r <= prob_sum * multiplier)
			return p.first;
	}

	return '?';
}

int main(int argc, char **argv)
{
	cout << "Loading model '" << argv[1] << "'" << endl;
	unordered_map<string, unordered_map<char, float>> sequences_lookahead = load_model(argv[1]);
	cout << "Model is loaded" << endl;

	string buffer = argv[2];
	int k = atoi(argv[3]);
	int gen_size = atoi(argv[4]);

	cout << buffer << " ";

	for (int i = 0; i < gen_size; i++)
	{
		char next_char = get_next_character_prediction(sequences_lookahead[buffer.substr(buffer.size() - k, buffer.size())]);

		cout << next_char;

		buffer += next_char;
	}

	return 0;
>>>>>>> bdef6cc7acbb0acbb8b0aecd37fa4b270a34b636
}
