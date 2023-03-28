#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <map>
#include <iterator>
#include <random>
#include <vector>
#include <string>

using namespace std;

#define NUM_THREADS 4

// Mutex to protect shared variables
pthread_mutex_t mutex;


class CopyModel 
{
	private:
		/**
		*  params
		*/
		ifstream file;
		int k;
		float alpha;
		float threshold;
		int n_threads;
		int file_lenght;

		/**
		*  structs
		*/
		vector<char> alphabet;
		multimap<string, char> sequences;
		int pointer;

	public:
		CopyModel(ifstream file, int k, float alpha, float threshold, int n_threads, int file_lenght)
		{
			this->file=file;
			this->k = k;
			this->alpha = alpha;
			this->threshold = threshold;
			this->n_threads = n_threads;
			this->file_lenght = file_lenght;
			this->alphabet = {};
			this->pointer = 0;
		}

		void addToAlphabet(char ch)
		{
			this->alphabet.push_back(ch);
		}

		/*
		void processSequence(vector<char> sequence, char ch)
		{
			this->sequences.insert({ sequence, ch });
		}
		*/

		char get_next_character_prediction(pair<multimap<string, char>::iterator, multimap<string, char>::iterator> iterator, string key, int size)
		{
			
			char possible_characters[size];
			char choosen_ch;
			int i = 0;

			cout << "possible characters: ";
			for (auto itr = iterator.first; itr != iterator.second; ++itr) {
				possible_characters[i] = itr->second;
				cout << itr->second << ",";
			}
			cout << "\n" << endl;

			srand(time(0)); // seed the random number generator with the current time
			
			choosen_ch = possible_characters[rand() % size];

			return choosen_ch;

		}

		void start()
		{

			// Creation of working threads and distribute (almost) equal chunks for faster processing of the copy model

			// Initialize mutex
			pthread_mutex_init(&mutex, NULL);

			// Create threads
			vector<pthread_t> threads(this->n_threads);
			vector<int> thread_ids(this->n_threads);

			for (int i = 0; i < this->n_threads; i++) {

				thread_ids[i] = i;

				int rc = pthread_create(&threads[i], NULL, worker, (void*)&thread_ids[i]);
				if (rc) {
					cout << "[ERROR] Could not create a thread, " << rc << endl;
					exit(-1);
				}

			}

			// Wait for threads to finish
			for (int i = 0; i < this->n_threads; i++) {

				pthread_join(threads[i], NULL);

			}
			
			// Destroy mutex
			pthread_mutex_destroy(&mutex);

		}
};

// Function to be executed by each thread
void* worker(void* arg) {

    int thread_id = *(int*)arg;



	char ch;
	vector<char> sliding_window;
	string seq;
	int i;
	char next_character;
	char next_character_prevision;
	int n_previous_encounters;
	
	int Nf = 0;
	int Nh = 0;

	while (!this->file.eof()) {
		
		this->file.get(ch);

		sliding_window.push_back(ch);
		if (sliding_window.size() == k)
		{
			for (char i: sliding_window)
				cout << i;
			sliding_window.erase(sliding_window.begin(), sliding_window.begin() + 1);

			string seq(sliding_window.begin(), sliding_window.end());

			n_previous_encounters = this->sequences.count(seq);

			if (!this->file.get(next_character)) {
				// reached end of file before k characters
				cerr << "Reached EOF" << endl;
				break;
			}

			cout << "(" << next_character << ")\n" << endl;

			//cout << seq;
			if (n_previous_encounters > 0) {

				next_character_prevision = get_next_character_prediction(this->sequences.equal_range(seq), seq, n_previous_encounters);
				
				//cout << "(((((()))))" << next_character_prevision << ")\n" << endl;

				if (next_character_prevision == next_character) Nh++;
				else Nf++;

				//cout << "(" << next_character_prevision << ")";
			}
			cout << endl;

			this->sequences.insert({seq, next_character});

		} 
		
		this->file.seekg(++this->pointer, ios::beg);	// Increments pointer for next iteration (sliding-window)


		
	}

	cout << "Nh = " << Nh << "\n";
	cout << "Nf = " << Nf << "\n";


    pthread_exit(NULL);
	
}


int main(int argc, char **argv) {
	
	// Command line arguments
	char *filename; 								// file to predict and compare
	int k;											// size of the sliding window
	float alpha;									// alpha value for probability
	int n_threads = NUM_THREADS;					// number of threads
	float threshold;								// probability threshold


	int opt;
    while ((opt = getopt(argc, argv, "f:k:t:")) != -1) {
        switch (opt) {
            case 'f':
                filename = string(optarg);
                break;
            case 'k':
                k = atoi(optarg);
				if (k  < 1) {		
					cout << "[ERROR] Sliding window size must be greater than zero.\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
			case 'a':
                alpha = atoi(optarg);
				if (alpha <= 0) {		
					cout << "[ERROR] Alpha must be bigger than zero.\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
            case 'n':
                n_threads = atoi(optarg);
				if (n_threads < 1 || n_threads > 8) {		
					cout << "[ERROR] Number of threads must be [1-8]\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
			case 't':
                threshold = atof(optarg);
				if (threshold < 0 || threshold > 1) {		
					cout << "[ERROR] Probability threshold must be between 0 and 1.\n" << endl;
					exit(EXIT_FAILURE);
				}
                break;
            default:
                cerr << "Usage: " << argv[0] << " -f <filename> -k <window_size> -a <alpha> -n <num_threads> -t <threshold>\n";
                return 1;
        }
    }

	int file_length;

	ifstream file.open(filename);
	if (!file .is_open()) {
		cout << "[ERROR] Can't open file '" << filename << "'" << endl;
		exit(EXIT_FAILURE);
	}
	file.seekg(0, ios::end);
	file_length = file.tellg();
	file.seekg(0, ios::beg);

	CopyModel cp(file, k, alpha, threshold, n_threads, file_lenght);

	cp.start();

	return 0;
}