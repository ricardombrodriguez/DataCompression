#include "datastream.h"

class DataStream 
{
	private:
		string filename;
		char c;
		ifstream stream;
	
	public:
		DataStream(string filename)
		{
			this->filename = filename;
			stream(this->filename);
		}

		char next()
		{
			stream.get(c);
			cout << c;
		}
}
