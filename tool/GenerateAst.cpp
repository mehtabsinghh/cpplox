#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Trim function to remove leading and trailing spaces
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(const std::string& str, const std::string& delim) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delim);
    while (end != std::string::npos) {
        tokens.push_back(trim(str.substr(start, end - start)));
        start = end + delim.length();
        end = str.find(delim, start);
    }
    tokens.push_back(trim(str.substr(start, end - start)));
    return tokens;
}

void defineVisitor(std::ofstream& file, const std::string& baseName, const std::vector<std::string>& types) {
    file << "template<typename R>\n";
    file << "class " << baseName << "Visitor {\n";
    file << "public:\n";
    for (const std::string& type : types) {
        const std::string typeName = type.substr(0, type.find(":"));
        file << "    virtual R visit" << typeName << "(const " << typeName << "& " << baseName << ") = 0;\n";
    }
    file << "};\n";
    file << "\n";
}

void defineType(std::ofstream& file, const std::string& baseName, const std::string& className, const std::string& fieldList) {
    file << "class " << className << " : public " << baseName << " {\n";
    file << "public:\n";
    std::vector<std::string> fields = split(fieldList, ", ");
    for (const std::string& field : fields) {
        file << "    " << field << ";\n";
    }
    // Constructor
    file << "\n";
    file << "    " << className << "(";
    for (size_t i = 0; i < fields.size(); i++) {
        file << fields[i];
        if (i != fields.size() - 1) {
            file << ", ";
        }
    }

    file << ")\n";

    // Initializer list
    file << "        : ";
    for (size_t i = 0; i < fields.size(); i++) {
        std::vector<std::string> fieldTokens = split(fields[i], " ");
        std::string fieldType = fieldTokens[0];
        std::string fieldName = fieldTokens[1];
        if (fieldType.substr(0, 11) == "std::unique") {
            file << fieldName << "(std::move(" << fieldName << "))";
        } else {
            file << fieldName << "(" << fieldName << ")";
        }
        if (i != fields.size() - 1) {
            file << ", ";
        }
    }
    file << " {}\n";

    // Accept method
    file << "    template<typename R>\n";
    file << "    R accept(" << baseName << "Visitor<R>& visitor) const {\n";
    file << "        return visitor.visit" << className << "(*this);\n";
    file << "    }\n";

    file << "};\n";
    file << "\n";
}

void defineAst(const std::string& outputDir, const std::string& baseName, const std::vector<std::string>& types) {
    std::string path = outputDir + "/" + baseName + ".hpp";
    std::ofstream file(path);
    file << "#ifndef " << baseName << "_HPP\n";
    file << "#define " << baseName << "_HPP\n";
    file << "\n";

    // Headers
    file << "#include <memory>\n";
    file << "#include \"Token.hpp\"\n";
    file << "\n";

    // Visitor interface
    defineVisitor(file, baseName, types);

    // Base class
    file << "class " << baseName << " {\n";
    file << "public:\n";
    file << "    virtual ~" << baseName << "() = default;\n";
    file << "    virtual void accept() const = 0;\n";  // Non-templated pure virtual function
    file << "};\n";
    file << "\n";

    // Derived classes
    for (const std::string& type : types) {
        const std::string className = type.substr(0, type.find(":"));
        const std::string fields = type.substr(type.find(":") + 1);
        defineType(file, baseName, className, fields);
    }
    file << "#endif\n";
}

int main() {
    std::string outputDir = "../include";
    defineAst(outputDir, "Expr", {
        "Binary : std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right",
        "Grouping : std::unique_ptr<Expr> expression",
        "Literal : std::shared_ptr<void> value",
        "Unary : Token op, std::unique_ptr<Expr> right"
    });
}