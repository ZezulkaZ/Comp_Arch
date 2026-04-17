#include<iostream>
#include<fstream>

using namespace std;

class Entry {
public:
  Entry() {
    valid = false;
    tag = 0;
    ref = 0;
  }

  void display(ofstream& outfile) {
    outfile << "  Entry: valid=" << valid << ", tag=" << tag << ", ref=" << ref << endl;
  }

  void set_tag(int _tag) { 
    tag = _tag; 
  }

  int get_tag() { 
    return tag; 
  }

  void set_valid(bool _valid) { 
    valid = _valid; 
  }

  bool get_valid() { 
    return valid; 
  }

  void set_ref(int _ref) { 
    ref = _ref; 
  }

  int get_ref() { 
    return ref; 
  }

private:  
  bool valid;
  unsigned tag;
  int ref;
};

class Cache {
public:
  Cache(int _assoc, unsigned _num_entries) {
    assoc = _assoc;
    num_entries = _num_entries;
    num_sets = num_entries / assoc;
    
    // Allocate 2D array: num_sets x assoc
    entries = new Entry*[num_sets];
    for (int i = 0; i < num_sets; i++) {
      entries[i] = new Entry[assoc];
    }
  }
  
  ~Cache() {
    for (int i = 0; i < num_sets; i++) {
      delete[] entries[i];
    }
    delete[] entries;
  }

  void display(ofstream& outfile) {
    outfile << "Cache Configuration:" << endl;
    outfile << "  Total entries: " << num_entries << endl;
    outfile << "  Associativity: " << assoc << endl;
    outfile << "  Number of sets: " << num_sets << endl;
    
    for (int i = 0; i < num_sets; i++) {
      outfile << "Set " << i << ":" << endl;
      for (int j = 0; j < assoc; j++) {
        outfile << "  Way " << j << ": ";
        entries[i][j].display(outfile);
      }
    }
  }


  int get_index(unsigned long addr) {
    // Index bits: log2(num_sets) bits above the offset (offset=0 since 1 word per block)
    // Since each block is 1 word, no offset bits needed
    unsigned num_set_bits = 0;
    unsigned temp = num_sets;
    while (temp > 1) {
      temp >>= 1;
      num_set_bits++;
    }
    
    // Extract index bits (the bits above the offset)
    return addr & (num_sets - 1);
  }

  unsigned get_tag(unsigned long addr) {
    // Tag is the remaining bits after removing index bits
    unsigned num_set_bits = 0;
    unsigned temp = num_sets;
    while (temp > 1) {
      temp >>= 1;
      num_set_bits++;
    }
    
    // Shift right by index bits to get tag
    return addr >> num_set_bits;
  }

  unsigned long retrieve_addr(int way, int index);
  
  bool hit(ofstream& outfile, unsigned long addr) {
    int index = get_index(addr);
    unsigned tag = get_tag(addr);
    
    // Check each way in the set for a hit
    for (int i = 0; i < assoc; i++) {
      if (entries[index][i].get_valid() && entries[index][i].get_tag() == tag) {
        return true;  // HIT
      }
    }
    return false;  // MISS
  }

  void update(ofstream& outfile, unsigned long addr) {
    int index = get_index(addr);
    unsigned tag = get_tag(addr);
    
    // Check if it's a hit first
    for (int i = 0; i < assoc; i++) {
      if (entries[index][i].get_valid() && entries[index][i].get_tag() == tag) {
        // Update reference count (LRU replacement policy)
        entries[index][i].set_ref(0);
        // Increment ref counts for other entries in the set
        for (int j = 0; j < assoc; j++) {
          if (j != i && entries[index][j].get_valid()) {
            entries[index][j].set_ref(entries[index][j].get_ref() + 1);
          }
        }
        return;
      }
    }
    
    // MISS - need to find a place to insert
    int replace_way = -1;
    
    // First, look for invalid entry
    for (int i = 0; i < assoc; i++) {
      if (!entries[index][i].get_valid()) {
        replace_way = i;
        break;
      }
    }
    
    // If no invalid entry, use LRU replacement (highest ref count)
    if (replace_way == -1) {
      int max_ref = -1;
      for (int i = 0; i < assoc; i++) {
        if (entries[index][i].get_ref() > max_ref) {
          max_ref = entries[index][i].get_ref();
          replace_way = i;
        }
      }
    }
    
    // Insert the new entry
    entries[index][replace_way].set_valid(true);
    entries[index][replace_way].set_tag(tag);
    entries[index][replace_way].set_ref(0);
    
    // Increment ref counts for other entries in the set
    for (int i = 0; i < assoc; i++) {
      if (i != replace_way && entries[index][i].get_valid()) {
        entries[index][i].set_ref(entries[index][i].get_ref() + 1);
      }
    }
  }
  
private:
  int assoc;
  unsigned num_entries;
  int num_sets;
  Entry **entries;
};


