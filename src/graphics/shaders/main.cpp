#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  stringstream header, source;

  header << "#include <stdint.h>\n\n";
  source << "#include <stdint.h>\n\n";

  for (int i = 1; i < argc; i++) {
    filesystem::path input_file = argv[i];

    cout << "Reading input file: " << input_file << '\n';
    ifstream input;
    input.exceptions(ifstream::failbit | ifstream::badbit);
    input.open(input_file, ios::binary);

    std::vector<uint8_t> buffer((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());

    std::vector<uint32_t> code;
    code.reserve(buffer.size());
    for (size_t i = 0; i < buffer.size(); i += 4) {
      uint32_t word = 0;
      for (int j = 0; j < 4; j++) {
        word |= buffer[i + j] << 8 * j;
      }
      code.push_back(word);
    }
    if (code[0] != 0x07230203) {
      throw std::runtime_error("Invalid magic number");
    }

    string variable_name = input_file.filename();
    replace(variable_name.begin(), variable_name.end(), '.', '_');
    variable_name += "_code";

    stringstream variable_declaration_s;
    variable_declaration_s << "uint32_t " << variable_name << '[' << code.size() << ']';
    string variable_declaration = variable_declaration_s.str();

    header << "extern " << variable_declaration << ";\n";
    source << variable_declaration << " = {\n";

    auto w = 9;
    for (auto it = code.cbegin(); it != code.cend(); it++) {
      auto n = distance(code.cbegin(), it);

      if (n % w == 0) {
        source << "    "
               << "0x" << setfill('0') << setw(8) << right << hex << *it;
      } else {
        source << ", "
               << "0x" << setfill('0') << setw(8) << right << hex << *it;
        if (n % w == (w - 1)) {
          source << ",\n";
        }
      }
    }

    if (buffer.size() % w != 0) {
      source << ",\n";
    }
    source << "};\n";

    if (i + 1 < argc) {
      source << '\n';
    }
  }

  cout << "Writing header file\n";
  ofstream header_f;
  header_f.exceptions(ostream::failbit | ostream::badbit);
  header_f.open("shaders.h");
  header_f << header.str();
  header_f.close();

  cout << "Writing source file\n";
  ofstream source_f;
  source_f.exceptions(ostream::failbit | ostream::badbit);
  source_f.open("shaders.c");
  source_f << source.str();
  source_f.close();
}
