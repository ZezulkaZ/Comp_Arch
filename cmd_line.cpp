#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<vector>

using namespace std;

// Include the Cache class definition here
// (In practice, you'd have a separate header file)

int main(int argc, char* argv[]) {
  /* check if the correct number of arguments have been passed */
  if (argc < 4) {
    cout << "Usage: " << endl;
    cout << "   ./cache_sim num_entries associativity input_filename" << endl;
    return 0;
  }

  /* get command-line arguments */
  unsigned num_entries = atoi(argv[1]);
  unsigned associativity = atoi(argv[2]);
  string input_filename = argv[3];

  /* print configuration */
  cout << "Number of entries: " << num_entries << endl;
  cout << "Associativity: " << associativity << endl;
  cout << "Input File Name: " << input_filename << endl;

  /* create the cache */
  Cache cache(associativity, num_entries);

  /* open input file */
  ifstream input_file(input_filename);
  if (!input_file.is_open()) {
    cerr << "Error: Could not open input file " << input_filename << endl;
    return 1;
  }

  /* read memory addresses from input file */
  vector<unsigned long> addresses;
  unsigned long addr;
  
  while (input_file >> addr) {
    addresses.push_back(addr);
  }
  input_file.close();

  /* open output file */
  string output_filename = "cache_sim_output";
  ofstream output_file(output_filename);
  
  if (!output_file.is_open()) {
    cerr << "Error: Could not create output file" << endl;
    return 1;
  }

  /* simulate cache behavior */
  for (size_t i = 0; i < addresses.size(); i++) {
    unsigned long current_addr = addresses[i];
    
    // Check if it's a hit or miss
    if (cache.hit(output_file, current_addr)) {
      output_file << current_addr << " : HIT" << endl;
      // Update cache (for LRU purposes)
      cache.update(output_file, current_addr);
    } else {
      output_file << current_addr << " : MISS" << endl;
      // Update cache with new entry
      cache.update(output_file, current_addr);
    }
  }

  output_file.close();
  
  cout << "Simulation complete. Results written to " << output_filename << endl;
  
  return 0;
}