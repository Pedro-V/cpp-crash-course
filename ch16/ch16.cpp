// ---------------------------------------------------------------------------------
// Streams

// ---------------------------------------------------------------------------------
// Stream classes

// Streams can be of 3 types: basic_{o,i,io}stream, which are STL classes that
// can be specialized by its underlying type, such as char or wchar_t. Pre-built
// specializations, such as for char types, are available in these headers:
// - wistream is basic_istream specialized for wchar_t
// - istream is basic_istream specialized for char
// Such types can be accessed through headers like <istream>, <iostream> and etc.

// Global stream objects are implementation-defined objects that wrap standard
// streams such as stdin, stdout and stderr:
// - cin and cout for input and output, respectively
// - cerr and clog for error output, unbuffered and buffered, respectively

#include <iostream>
#include <string>
#include <bitset>

// Stream classes support 2 types of operations: formatted and unformatted.

// Formatted IO passes through the standard stream operators << (output)
// and >> (input), which are oveloaded by the basic stream templates.
void example_output_operator() {
  std::bitset<8> s{ "01110011" };
  std::string str("Crying zeros and I'm hearing ");
  size_t num{ 111 };
  std::cout << s;
  std::cout << '\n';
  std::cout << str;
  std::cout << num;
  std::cout << "s\n";
}

// Since the output of a stream operator is a stream reference, these can be
// chained together.
void example_chained_output_operator() {
  std::bitset<8> s{ "01110011" };
  std::string str("Crying zeros and I'm hearing ");
  size_t num{ 111 };
  std::cout << s << '\n' << str << num << "s\n";
}

// Useful for conciser stream-handling code.
using namespace std;

void example_input_operator() {
  double x, y;
  cout << "X: ";
  cin >> x;
  cout << "Y: ";
  cin >> y;

  string op;
  cout << "Operation: ";
  cin >> op;
  if (op == "+") {
    cout << x + y;
  } else if (op == "-") {
    cout << x - y;
  } else if (op == "*") {
    cout << x * y;
  } else if (op == "/") {
    cout << x / y;
  } else {
    cout << "Unknown operation " << op;
  }
}

// Unformatted IO are usually used with binary data or when low-level access to
// streams is necessary. Some of the unformatted operations offered by
// istream and ostream are get, put, write, read, ignore, etc..

void example_unformatted() {
  cout.write("Give me a char\n", 16);
  char c;
  cin.get(c);
  cout.put(c);
  cout.put('\n');
  cin.get(); // Gets the \n char from previous get
  char *s = new char[10];
  cout.write("Give me a char*\n", 17);
  // Since it's unformatted, whitespace is not ignored by the operation.
  cin.getline(s, 9);
  cout.write(s, 10);
}

// Some types are formatted in a special way by the operators.
void example_special_formatting() {
  // Bools are formatted as integers.
  bool b{ true };
  // Pointers are formatted as hexadecimal numbers, either 32- or 64-bit,
  // depending on the system.
  void *p;
  cout << "true: " << b << " pointer: " << p << "\n\n";

  cout << "Give me a word starting with whitespace\n";
  string str;
  // The whitespace is stripped by the input operator.
  cin >> str;
  cout << str << "\n\n";

  cout << "Give me text, followed by an int.\n";
  // If input doesn't begin with at least one digit, yields 0.
  int i;
  cin >> i;
  cout << "i: " << i << '\n';
}

// ---------------------------------------------------------------------------------
// Stream state: indicate whether IO failed.

void example_stream_state() {
  // Stream's bits expose the state of the stream: good, eof, fail and bad bits.
  cout << "Experiment giving non-good signals to the stream, like EOF "
          "or anything but a number.\n";
  int x;
  cin >> x;
  for (size_t i{}; i < 2; i++) {
    if (!cin.good()) {
      cout << "Entered bad state because of ";
      if (cin.eof()) {
        cout << "EOF";
      } else if (cin.fail()) {
        cout << "Failed operation";
      } else if (cin.bad()) {
        cout << "Catastrophic error";
      }
      cout << '\n';
      // Clears its state, going back to good;
      cin.clear();
    }
    else {
      cout << "In good state.\n";
    }
  }
  // Since EOF stays on the stream, even clearing it and trying to read
  // again will cause another EOF to be caught.
}

void example_boolean_streams_conversion() {
  string word;
  size_t count{};
  cout << "Write words for me to discover!\n";
  cout << "Will stop when reach a non-good state, such as EOF.\n";
  // Bad state converts to false, and is emitted by stream itself.
  while (cin >> word)
    count++;
  cout << "Discovered " << count << " words.\n";
}

void example_exceptions() {
  // Streams can throw exceptions when certain state bits occur.
  // We set an exception to occur whenever EOF or failed operations occur.
  cin.exceptions(istream::eofbit | istream::failbit);
  int x;
  size_t count{};
  cout << "Give me numbers to count.\n";
  try {
    while (cin >> x) 
      count++;
    cout << "Read " << count << " numbers\n";
  } catch (const exception& e) {
    cerr << "Error ocurred reading from stdin: " << e.what() << '\n';
  }
}

// ---------------------------------------------------------------------------------
// Manipulators: Manual control over buffering, flushing and formatting.

// Since slower system calls are made for writing to streams, a technique called
// buffering is used to wait for multiple output elements (by queueing them)
// and send them together (by flushing them).

void example_buf_flush_manip() {
  // <istream> provides the ws manipulator for skipping whitespace.
  
  // <ostream> provides flush for flushing, ends for sending a null byte and
  // endl for sending a newline and flushing.
  string word;
  size_t count{};
  cout << "Write words for me to discover!\n";
  cout << "Will stop when reach a non-good state, such as EOF.\n";
  while (cin >> word)
    count++;
  cout << "Discovered " << count << " words." << endl;
  // Use \n when you know there'll be more output soon, and std::endl when the
  // program has finished outputting text to the stream for a while.
}

#include <iomanip>
void example_format_manip() {
  // <iomanip> provides many manipulators for controlling the formatting output.
  // I use the scope resolution operator :: on the manipulators for better highlight.
  cout << "Gotham needs its " << std::boolalpha << true << " hero.\n";
  cout << "Mark it " << std::noboolalpha << false << "!\n";
  cout << "There are " << 69 << "," << std::oct << 7 << " leaves in here.";
  cout << "Yabba " << std::hex << 3'669'732'608 << "!\n";
  cout << "Avogadro's number: " << std::scientific << 6.0221415e-23 << "\n";
  cout << "The Hogwarts platform: " << std::fixed << std::setprecision(2) << 9.750123 << "\n";
  // Manipulators apply to all subsequent objects you insert into a stream.
  cout << "Always eliminate " << 3'735'929'054 << "\n"; 
  // Back to decimal.
  cout << std::dec;
  cout << "setw(6), several elements: [" << 89 << std::setw(6) << 12 << 34 << "]\n";
}

// ---------------------------------------------------------------------------------
// User-defined types: Making user-defined types work with streams by
// overloading input/output operators.

#include <vector>

template <typename T>
ostream& operator<<(ostream& s, const vector<T> v) {
  s << "Size: " << v.size()
    << "\nCapacity: " << v.capacity()
    << "\nElements:\n";
  for (const auto& element : v) {
    s << "\t" << element << "\n";
  }
  return s;
}

void example_output_vec() {
  const vector<string> characters {
    "Bobby Shaftoe",
    "Lawrence Waterhouse",
    "Gunter Bischoff",
    "Earl Comstock"
  };
  cout << characters << endl;

  const vector<bool> bits { true, false, true, false };
  cout << boolalpha << bits << endl;
}

#include <deque>
template <typename T>
istream& operator>>(istream& s, deque<T>& t) {
  T element;
  while (s >> element) {
    t.emplace_back(move(element));
  }
  return s;
}

void example_input_deque() {
  cout << "Give me numbers: ";
  deque<int> numbers;
  cin >> numbers;
  int sum{};
  cout << "Cumulative sum:\n";
  for (const auto& element : numbers) {
    sum += element;
    cout << sum << "\n";
  }
}

// ---------------------------------------------------------------------------------
// String stream: Facilitate reading from and writing to character sequences.

#include <sstream>

void example_ostringstream() {
  // ostringstream produces strings with str.
  int sun_amount = 2;
  ostringstream ss;
  ss << "By Grabthar's hammer, "
     << "by the " << sun_amount << " suns of Worvan. "
     << "You shall be avenged.";
  // str() return a copy of the internal buffer as a string.
  const auto lazarus = ss.str();

  // str(s) sets the internal buffer to s.
  ss.str("I am Groot.");
  const auto groot =  ss.str();
  
  cout << lazarus << "\n";
  cout << groot << endl;
}

void example_istringstream() {
  // istringstream supports construction from a string.
  string numbers("1 2.23606 2");
  istringstream ss{ numbers };
  int a;
  float b, c, d;
  ss >> a >> b >> c;
  cout << "a: " << a << " b: " << b << " c: " << c << endl;
  if (ss >> d) {
    cout << "Exhausted string streams return false\n";
  }
}

void example_stringstream() {
  // stringstream supports all stream operations.
  std::stringstream ss;
  ss << "Zed's DEAD";

  std::string who;
  ss >> who;
  int what;
  ss >> std::hex >> what;
  cout << who << " " << std::hex << what << endl;
}

// ---------------------------------------------------------------------------------
// File streams: reading from and writing to files.
// Benefits from stream operations, RAII and move semantics.

#include <fstream>

void example_ofstream() {
  // fstreams accept flags for managing read/write/append access to the files,
  // and also setting special characteristics, such as a binary file.
  ofstream file{ "lunchtime.txt", ios::out|ios::app };
  file << "Time is an illusion.\n";
  file << "Lunch time, " << 2 << "x so." << endl;
}

#include <limits>

void example_ifstream() {
  // Flag defaults to ios::in.
  ifstream file{ "numbers.txt" };
  auto maximum = numeric_limits<int>::min();
  int value;
  while (file >> value)
    maximum = maximum < value ? value : maximum;
  cout << "Maximum found in numbers.txt was " << maximum << endl;
}

// File streams fail silently. This open function uses exceptions for checking
// the open operation occurred fine.
ifstream open(const char* path, ios_base::openmode mode = ios_base::in) {
  ifstream file{ path, mode };
  if (!file.is_open()) {
    string err{ "Unable to open file " };
    err.append(path);
    throw runtime_error{ err };
  }
  file.exceptions(ifstream::badbit);
  return file;
}

void example_open_with_exceptions() {
  try {
    auto file = open("non_existent.txt");
  } catch (std::exception& e) {
    cerr << e.what() << endl;
  }
}

// ---------------------------------------------------------------------------------
// Stream buffers: classes that send or extract characters, and are used by streams.

#include <iterator>

void example_rdbuf() {
  // rdbuf return the pointer to the underlying stream buffer
  ifstream file{ "numbers.txt" };
  cout << file.rdbuf();
}

void example_ostreambuf_itr() {
  // Stream buffer iterators are template classes that expose an input/output iterator
  // interface to the stream.
  ostreambuf_iterator<char> itr{ cout };
  *itr = 'H';
  ++itr;
  *itr = 'i';
}

void example_istreambuf_itr() {
  istreambuf_iterator<char> cin_itr{ cin.rdbuf() }, end{};
  cout << "What's your name? ";
  const string name{ cin_itr, end };
  cout << "\nGoodbye, " << name;
}

// ---------------------------------------------------------------------------------
// Random access

void example_random_access() {
  // Streams keep track of the cursor or position the next byte will be read/written.
  // tell{g,p} return the position, while seek{g,p} sets the position.
  // This is particularly useful for files.
  try {
    auto intro = open("introspection.txt");
    cout << "Contents: " << intro.rdbuf() << endl;
    intro.seekg(0);
    cout << "Contents after seekg(0): " << intro.rdbuf() << endl;
    // Sets the cursor 4 lines before the end of the file.
    intro.seekg(-4, ios_base::end);
    cout << "tellg() after seekg(-4, ios_base::end): "
         << intro.tellg() << endl;
    cout << "Contents after seekg(-4, ios_base::end): "
         << intro.rdbuf() << endl;
  } catch (std::exception& e) {
    cerr << e.what();
  }
}

// ---------------------------------------------------------------------------------
// Exercises

#include <string_view>
#include <cctype>

// Exercise 16-1

struct AutoBrake {
  AutoBrake(double col_thr_s, double speed_mps)
    : collision_threshold_s{ col_thr_s }, speed_mps{ speed_mps } {}
  double collision_threshold_s;
  double speed_mps;
};

ostream& operator<<(ostream& s, const AutoBrake ab) {
  s << "Collision threshold: " << ab.collision_threshold_s << " s\n"
    << "Speed: " << ab.speed_mps << " mps\n";
  return s;
}

// Exercise 16-2

void capitalize(string& str) {
  str[0] = toupper(str[0]);
  for (size_t i{ 1 }; i < str.length(); i++) {
    str[i] = tolower(str[i]);
  }
}
void read_capitalize_write() {
  vector<string> words;
  string word;
  while (cin >> word) {
    capitalize(word);
    words.emplace_back(word);
  }
  cout << words;
}

// Exercise 16-4

#include <map>

struct AlphaHistogram {
  void ingest(string_view str) {
    for (auto c : str) {
      if (isalpha(c)) counts[toupper(c)]++;
    }
  }
  void print() const {
    for (auto [c, n] : counts) {
      cout << c << ": ";
      while (n--) cout << "*";
      cout << endl;
    }
  }
private:
  std::map<char, size_t> counts;
};

void file_summary(const char* file_name) {
  AlphaHistogram hist;
  string word;
  ifstream file{ file_name };

  int wc{}, len{};
  while (file >> word) {
    wc++;
    len += word.length();
    hist.ingest(word);
  }

  cout << "Word count: " << wc
       << "\nAverage word length: " << len / wc
       << "\nHistogram:\n";
  hist.print();
}

// ---------------------------------------------------------------------------------

int main() {
  file_summary("lunchtime.txt");
}
