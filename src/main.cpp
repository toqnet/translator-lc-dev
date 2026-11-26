#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

int show_help(const char *str)
{
    std::cout << "Usage: " << str << " <input_file> -o <output_file>" << std::endl;
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc < 2) { show_help(argv[0]); return 1; }
    std::string in_file = argv[1];
    std::string out_file = "out.c";

    for (int i(2);i < argc; i++)
    {
        if (std::string(argv[i]) == "-o")
        {
            if (++i < argc) out_file = argv[i];
            else 
            {
                std::cerr << "Enter: no filename after -o\n";
                return 1;
            }
        }
        else
        {
            std::cerr << "Error: unknown option '" << argv[i] << "'\n";
            return 1;
        }
    }
    std::ifstream in(in_file);
    if (!in.is_open())
    {
        std::cerr << "Failed to open input file: " << in_file << "\n";
        return 1;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    std::string source = ss.str();
    try 
    {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(std::move(tokens));
        auto program = parser.parseProgram();
        
        std::ofstream out(out_file);
        if (!out.is_open())
        {
            std::cerr << "Failed to open output file: " << out_file << "\n";
            return 1;
        }

        Codegen cg(out);
        cg.generate(*program);

        std::cout << "Successfully compiled to " << out_file << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Compilation error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

