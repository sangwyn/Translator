#include "SyntaxAnalyzer.h"

//семантика которую пока некуда деть
void SyntaxAnalyzer::Push_id(int cur_id) {
    if (in_preread) {
        return;
    }
    std::string n_type;
    std::string name;
    bool is_const = var_decl_const;
    if (lexemes[cur_id] == "void") {
        n_type = lexemes[cur_id];
        is_const = true;
    }
    if (lexemes[cur_id] == "list") {
        n_type = lexemes[cur_id] + lexemes[cur_id + 1] + lexemes[cur_id + 2] + lexemes[cur_id + 3]
            + lexemes[cur_id + 4];
        name = lexemes[cur_id + 5];
        cur_id = cur_id + 5;
    } else {
        name = lexemes[cur_id];
        //	cur_id = cur_id + 1;
    }
    if (lexemes[cur_id + 1] == "(") {
        is_const = true;
    }
    n_type = var_decl_type;
    TID_t new_element = {n_type, name, is_const};
    for (int i = 0; i < now->TID.size(); ++i) {
        if (now->TID[i].name == name) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[cur_id]) + ": redefinition " + n_type + " " + name);
        }
    }
    now->TID.push_back(new_element);
}

void SyntaxAnalyzer::NewScope() {
    TID_node *new_t = new TID_node;
    now->next.push_back(new_t);
    new_t->parent = now;
    now = new_t;

}

void SyntaxAnalyzer::BackToScope() {
    now = now->parent;
}

void SyntaxAnalyzer::PreReadLexemes() {

    if (cur == "string" && lexemes[ind + 1] == "main") {
        in_preread = false;
        return;
    } else if (IsType(cur)) {
        int d = 0;
        if (cur == "list")
            d = 4;
        if (types[ind + 1 + d] != 2)
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected name after type, got '" + cur
                    + "' instead.");
        if (lexemes[ind + 2 + d] == "=" || lexemes[ind + 2 + d] == ";") {

            //here
            //here

            VarDeclaration();

            //here

            PreReadLexemes();
        } else if (lexemes[ind + 2 + d] == "(") {
            Function();
            PreReadLexemes();
        } else {
            throw std::runtime_error("Line " + std::to_string(lines[ind])
                                         + ": expected variable/function declaration, got '" + cur
                                         + "' instead.");
        }
        return;
    } else if (cur == "const") {
        //here


        //ReadNext();
        VarDeclaration();

        //here

        PreReadLexemes();
        return;
    } else if (cur == "void") {
        Function();
        PreReadLexemes();
        return;
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected var/func declaration or main, got '"
                + cur + "' instead.");
    }
}

void SyntaxAnalyzer::PushFunction(int cur_id) {
    std::string n_type;
    std::string name;
    if (lexemes[cur_id] == "void") {
        n_type = lexemes[cur_id];
    }
    if (lexemes[cur_id] == "list") {
        n_type = lexemes[cur_id] + lexemes[cur_id + 1] + lexemes[cur_id + 2] + lexemes[cur_id + 3]
            + lexemes[cur_id + 4];
        name = lexemes[cur_id + 5];
        cur_id = cur_id + 5;
    } else {
        n_type = lexemes[cur_id];
        name = lexemes[cur_id + 1];
        cur_id = cur_id + 1;
    }
    cur_id += 2;
    std::vector<std::string> arguments;
    while (lexemes[cur_id] != ")") {
        if (lexemes[cur_id] == "list") {
            arguments.push_back(
                lexemes[cur_id] + lexemes[cur_id + 1] + lexemes[cur_id + 2] + lexemes[cur_id + 3]
                    + lexemes[cur_id + 4]);
            cur_id += 6;
        } else {
            arguments.push_back(lexemes[cur_id]);
            cur_id += 2;
        }
        if (lexemes[cur_id] != ")") {
            cur_id += 1;
        }
    }
    function.insert({name, arguments});
}

void SyntaxAnalyzer::PushStack(std::string str) {
    types_stack.push(str);
}

void SyntaxAnalyzer::CheckBin(int cur_id) {
    std::string first;
    first = types_stack.top();
    types_stack.pop();
    std::string operation;
    operation = types_stack.top();
    types_stack.pop();
    std::string second;
    second = types_stack.top();
    types_stack.pop();

    if (first > second) {
        std::swap(first, second);
    }
    std::string c_first = first;
    std::string c_second = second;
    if (first[0] == second[0]) {
        if (first[0] != 'l') {
            if (first == "string") {
                if (!(IsAssOp(operation) || operation == "+" || operation == "*" || operation == ">"
                    || operation == "<"
                    || operation == "==" || operation == ">=" || operation == "<="
                    || operation == "!=")) {
                    throw std::runtime_error(
                        "Line " + std::to_string(lines[cur_id]) + ": no operator "
                            + lexemes[cur_id] + "matches these operands");
                }
            }
            types_stack.push(first);
        } else {
            int first_n = std::stoi(first.substr(5, first.find(']') - 5));
            int second_n = std::stoi(second.substr(5, second.find(']') - 5));
            if (first_n != second_n) {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[cur_id]) + ": no operator "
                        + lexemes[cur_id] + "matches these operands");
            }
            c_first = first.substr(first.find(']') + 1);
            c_second = second.substr(second.find(']') + 1);
            if (first == second) {
                if (first == "string") {
                    if (!(IsAssOp(operation) || operation == "+" || operation == "*"
                        || operation == ">" || operation == "<"
                        || operation == "==" || operation == ">=" || operation == "<="
                        || operation == "!=")) {
                        throw std::runtime_error(
                            "Line " + std::to_string(lines[cur_id]) + ": no operator "
                                + lexemes[cur_id] + "matches these operands");
                    }
                }
                types_stack.push(c_first);
            }
        }
    }
    if (second == "string") {
        throw std::runtime_error(
            "Line " + std::to_string(lines[cur_id]) + ": no operator "
                + lexemes[cur_id] + "matches these operands");
    }
    if (second == "float" || first == "float") {
        if (second == "float") {
            types_stack.push(c_second);
        } else {
            types_stack.push(c_first);
        }
        return;
    }
    if (second == "int" || first == "int") {
        if (second == "int") {
            types_stack.push(c_second);
        } else {
            types_stack.push(c_first);
        }
        return;
    }
    if (second == "bool" || first == "bool") {
        if (second == "bool") {
            types_stack.push(c_second);
        } else {
            types_stack.push(c_first);
        }
        return;
    }
}

void SyntaxAnalyzer::CheckUn(int cur_id) {
    std::string first, c_first;
    first = types_stack.top();
    c_first = first;
    std::string operation;
    operation = types_stack.top();
    if (operation == "!") {
        types_stack.push("bool");
    } else if (operation == "+" || operation == "-") {
        if (first[0] == 'l') {
            c_first = first.substr(first.find(']') + 1);
        }
        if (first == "string") {
            throw std::runtime_error(
                "Line " + std::to_string(lines[cur_id]) + ": no operator "
                    + lexemes[cur_id] + "matches these operands.");
        }
        types_stack.push(c_first);
    } else {
        throw std::runtime_error("WTF (CheckUn)");
    }
}

std::string SyntaxAnalyzer::GetType(int cur_id) {
    std::string name = lexemes[cur_id];
    TID_node *ptr = now;
    while (true) {
        for (int i = 0; i < ptr->TID.size(); ++i) {
            if (name == ptr->TID[i].name) {
                return ptr->TID[i].type;
            }
        }
        if (ptr->parent == nullptr) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[cur_id]) + ": identifier \""
                    + name + " \"  is undefined.");

        } else {
            ptr = ptr->parent;
        }
    }
}

bool SyntaxAnalyzer::IsConst(int cur_id) {
    std::string name = lexemes[cur_id];
    TID_node *ptr = now;
    while (true) {
        for (int i = 0; i < ptr->TID.size(); ++i) {
            if (name == ptr->TID[i].name) {
                return ptr->TID[i].is_const;
            }
        }
        if (ptr->parent = nullptr) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[cur_id]) + ": something went wrond :(");
        } else {
            ptr = ptr->parent;
        }
    }
}
//


bool SyntaxAnalyzer::IsType(std::string s) {
    return (s == "int" || s == "bool" || s == "float" || s == "string" || s == "list");
}

void SyntaxAnalyzer::ReadLexemes() {
    std::ifstream fin("lexemes.txt");
    while (!fin.eof()) {
        int type, line;
        std::string word;
        fin >> word >> type >> line;
        lexemes.push_back(word);
        types.push_back(type);
        lines.push_back(line);
    }
}

void SyntaxAnalyzer::ReadNext() {
    if (ind == lexemes.size() - 1) {
        cur = "ok";
        if (ind != 0 && lexemes[ind - 1] != "}")
            cur = "Unexpected end of program.";
        //        cur = "_";
        throw std::runtime_error(cur);
    }
    ++ind;
    cur = lexemes[ind];
    cur_type = types[ind];
}

void SyntaxAnalyzer::Variable() {
    if (!IsType(cur))
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected type, got '" + cur + "' instead.");

    Type();

    if (cur_type != 2)
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected name after type, got '" + cur
                + "' instead.");
    Push_id(ind);
    ReadNext();
}

void SyntaxAnalyzer::VarDeclaration() {
    if (cur == "const") {
        var_decl_const = true;
        ReadNext();
    }

    Variable();
    if (cur == "=") {
        ReadNext();
        Expression();
    }
    while (cur == ",") {
        ReadNext();
        if (cur_type != 2)
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected name after coma, got '" + cur
                    + "' instead.");
        Push_id(ind);
        ReadNext();
        if (cur == "=") {
            ReadNext();
            Expression();
        }
    }
    var_decl_type = "";
    var_decl_const = false;
    if (cur != ";")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected ';' after operator, got '" + cur
                + "' instead.");
    ReadNext();
}

void SyntaxAnalyzer::Assignment() {
    if (IsType(cur)) {
        Variable();
    } else if (cur_type != 2) {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected name in assignment, got '" + cur
                + "' instead.");
    } else {
        ReadNext();
    }
    //ERROR !!!! ERROR
    //ReadNext();
    if (!IsAssOp(cur))
        throw std::runtime_error("Line " + std::to_string(lines[ind])
                                     + ": expected assignment operation in assignment, got '" + cur
                                     + "' instead.");
    ReadNext();
    Expression();
}

void SyntaxAnalyzer::Block() {
    if (cur != "{")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '{', got '" + cur + "' instead.");
    ReadNext();
    while (cur != "}")
        Operator();
    ReadNext();
}

void SyntaxAnalyzer::IfOperator() {
    if (cur != "if")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'if', got '" + cur + "' instead.");
    ReadNext();
    if (cur != "(")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '(' after 'if', got '" + cur
                + "' instead.");
    ReadNext();
    Expression();
    if (cur != ")")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected ')' after expression, got '" + cur
                + "' instead.");
    ReadNext();
    if (!in_preread) {
        NewScope();
    }
    Block();
    if (!in_preread) {
        BackToScope();
    }
    if (cur == "else") {
        ReadNext();
        if (!in_preread) {
            NewScope();
        }
        Block();
        if (!in_preread) {
            BackToScope();
        }
    }
}

void SyntaxAnalyzer::WhileOperator() {
    if (cur != "while")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'while', got '" + cur
                + "' instead.");
    ReadNext();
    if (cur != "(")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '(' after 'while', got '" + cur
                + "' instead.");
    ReadNext();
    Expression();
    if (cur != ")")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected ')' after expression, got '" + cur
                + "' instead.");
    ReadNext();

    if (!in_preread) {
        NewScope();
    }
    cycle_counter += 1;
    Block();
    cycle_counter -= 1;
    if (!in_preread) {
        BackToScope();
    }
}

void SyntaxAnalyzer::ForOperator() {
    if (cur != "for")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'for', got '" + cur + "' instead.");
    ReadNext();
    if (cur != "(")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '(' after 'for', got '" + cur
                + "' instead.");
    ReadNext();

    //ERROR
    if (!in_preread) {
        NewScope();
    }
    if (cur != ";") {

        Assignment();
        if (cur != ";") {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind])
                    + ": expected ';' after 'for' assignment, got '" + cur
                    + "' instead.");
        }
    }
    ReadNext();
    if (cur != ";") {
        Expression();
        if (cur != ";") {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind])
                    + ": expected ';' after 'for' expression, got '" + cur
                    + "' instead.");
        }
    }
    ReadNext();
    if (cur != ";") {
        Expression();
        if (cur != ")")
            throw std::runtime_error("Line " + std::to_string(lines[ind])
                                         + ": expected ')' after 'for' expressions, got '" + cur
                                         + "' instead.");
    }

    ReadNext();
    cycle_counter += 1;
    Block();
    cycle_counter -= 1;
    if (!in_preread) {
        BackToScope();
    }
}

void SyntaxAnalyzer::Arguments() {
    if (!IsType(cur))
        return;
    Variable();
    var_decl_type = "";
    while (cur == ",") {
        ReadNext();
        Variable();
        var_decl_type = "";
    }
}

void SyntaxAnalyzer::Function() {

    if (!IsType(cur) && cur != "void")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected function type, got '" + cur
                + "' instead.");
    if (cur == "void") {
        var_decl_type = "void";
        ReadNext();
    } else
        Type();
    if (cur_type != 2)
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected function name after type, got '"
                + cur + "' instead.");
    //here
    if (in_preread) {
        in_preread = false;
        Push_id(ind);
        in_preread = true;
    }
    var_decl_type = "";
    ReadNext();
    if (cur != "(")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '(' after function name, got '" + cur
                + "' instead.");
    ReadNext();
    if (!in_preread) {
        NewScope();
    }

    Arguments();
    if (cur != ")")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected ')' after function arguments, got '"
                + cur + "' instead.");
    ReadNext();
    Block();

    if (!in_preread) {
        BackToScope();
    }

}

void SyntaxAnalyzer::Program() {

    //    if (cur == "_")
    //        return;
    if (cur == "string" && lexemes[ind + 1] == "main") {
        NewScope();
        ReadNext();
        ReadNext();
        if (cur != "(")
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected '(' after 'main', got '" + cur
                    + "' instead.");
        ReadNext();
        if (cur != ")")
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected ')', got '" + cur
                    + "' instead.");
        ReadNext();
        Block();
        BackToScope();
    } else if (IsType(cur)) {
        int d = 0;
        if (cur == "list")
            d = 4;
        if (types[ind + 1 + d] != 2)
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected name after type, got '" + cur
                    + "' instead.");
        if (lexemes[ind + 2 + d] == "=" || lexemes[ind + 2 + d] == ";") {

            //here
            int c_ind = ind;
            //here

            VarDeclaration();

            //here
            //	Push_id(c_ind);
            Program();
        } else if (lexemes[ind + 2 + d] == "(") {
            int c_ind = ind;
            Function();
            PushFunction(c_ind);
            Program();
        } else {
            throw std::runtime_error("Line " + std::to_string(lines[ind])
                                         + ": expected variable/function declaration, got '" + cur
                                         + "' instead.");
        }
        return;
    } else if (cur == "const") {
        //here
        int c_ind = ind;

        //	ReadNext();
        VarDeclaration();

        //here
        //	Push_id(c_ind);
        Program();
        return;
    } else if (cur == "void") {
        int c_ind = ind;
        Function();
        PushFunction(c_ind);
        Program();
        return;
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected var/func declaration or main, got '"
                + cur + "' instead.");
    }
    //    if (cur == "_")
    //        return;
    //    ReadNext();
}

void SyntaxAnalyzer::LiteralList() {
    if (cur != "{") {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '{', got '" + cur + "' instead.");
    }
    ReadNext();
    if (cur_type == 8) {
        ReadNext();
        while (cur == ",") {
            ReadNext();
            if (cur_type == 8) {
                ReadNext();
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected string literal, got '" + cur
                        + "' instead.");
            }
        }
        if (cur != "}") {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected '}', got '" + cur
                    + "' instead.");
        }
        ReadNext();
    } else {
        if (cur_type == 3) {
            ReadNext();
            if (cur == ".") {
                ReadNext();
                if (cur_type == 3) {
                    ReadNext();
                } else {
                    throw std::runtime_error(
                        "Line " + std::to_string(lines[ind]) + ": expected int literal, got '" + cur
                            + "' instead.");
                }
            }
            while (cur == ",") {
                ReadNext();
                if (cur_type == 3) {
                    ReadNext();
                    if (cur == ".") {
                        ReadNext();
                        if (cur_type == 3) {
                            ReadNext();
                        } else {
                            throw std::runtime_error("Line " + std::to_string(lines[ind])
                                                         + ": expected float literal, got '" + cur
                                                         + "' instead.");
                        }
                    }
                } else {
                    throw std::runtime_error(
                        "Line " + std::to_string(lines[ind]) + ": invalid array element '" + cur
                            + "'.");
                }
            }
            if (cur != "}") {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected '}', got '" + cur
                        + "' instead.");
            }
            ReadNext();
        } else {
            if (cur == "true" || cur == "false") {
                ReadNext();
                while (cur == ",") {
                    ReadNext();
                    if (cur == "true" || cur == "false") {
                        ReadNext();
                    } else {
                        throw std::runtime_error(
                            "Line " + std::to_string(lines[ind]) + ": expected bool literal, got '"
                                + cur + "' instead.");
                    }
                }
                if (cur != "}") {
                    throw std::runtime_error(
                        "Line " + std::to_string(lines[ind]) + ": expected '}', got '" + cur
                            + "' instead.");
                }
                ReadNext();
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": invalid array element '" + cur
                        + "'.");
            }

        }
    }

}

void SyntaxAnalyzer::IsLiteral() {
    if (cur == "{") {
        LiteralList();
        return;
    }
    if (cur_type == 8) {
        ReadNext();
        return;
    }
    if (cur_type == 3) {
        ReadNext();
        if (cur == ".") {
            ReadNext();
            if (cur_type != 3) {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected float literal, got '" + cur
                        + "' instead.");
            }
            ReadNext();
            return;
        }
        return;
    }
    if (cur == "true" || cur == "false") {
        ReadNext();
        return;
    }
    throw std::runtime_error(
        "Line " + std::to_string(lines[ind]) + ": expected literal, got '" + cur + "' instead.");
}

void SyntaxAnalyzer::Priority10() {
    if (cur == "-" || cur == "+") {
        types_stack.push(cur);
        ReadNext();
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected literal, got '" + cur
                + "' instead.");
    }
}

void SyntaxAnalyzer::Priority9() {
    bool unary = false;
    int id = ind;
    if (cur == "-" || cur == "+") {
        unary = true;
        Priority10();
    }
    if (cur == "(") {
        ReadNext();
        Priority2();

        if (cur == ")") {
            ReadNext();
            return;
        } else {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected ')', got '" + cur
                    + "' instead.");
        }
    }
    if (cur_type == 2) {
        if (lexemes[ind + 1] != "[") {
            types_stack.push(GetType(cur_type));
        }
        ReadNext();
        if (cur == "(") {
            ReadNext();
            GiveArguments();
            if (cur == ")") {
                ReadNext();
                return;
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ')', got '" + cur
                        + "' instead.");
            }
        }
        if (cur == "[") {
            ReadNext();
            Priority2();
            if (cur == "]") {
                ReadNext();
                return;
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ']', got '" + cur
                        + "' instead.");
            }
        }
    } else {
        IsLiteral();
    }
    if (unary) {
        CheckUn(id);
    }
}

void SyntaxAnalyzer::Operation8() {
    if (cur == "!") {
        ReadNext();
        return;
    }
    throw std::runtime_error(
        "Line " + std::to_string(lines[ind]) + ": expected '!', got '" + cur + "' instead.");
}

void SyntaxAnalyzer::Priority8() {
    bool unary = false;
    int id = ind;
    if (cur == "!") {
        unary = true;
        ReadNext();
    }
    Priority9();
    if (unary) {
        CheckUn(id);
    }
}

void SyntaxAnalyzer::Operation7() {
    if (cur == "*") {
        ReadNext();
        return;
    }
    if (cur == "/") {
        ReadNext();
        return;
    }
    if (cur == "//") {
        ReadNext();
        return;
    }
    if (cur == "%") {
        ReadNext();
        return;
    }
    throw std::runtime_error(
        "Line " + std::to_string(lines[ind]) + ": expected operation, got '" + cur + "' instead.");
}

void SyntaxAnalyzer::Priority7() {
    Priority8();
    if (ind + 1 >= lexemes.size()) {
        return;
    } else {
        while (cur == "*" || cur == "/" || cur == "//" || cur == "%") {
//            Operation7();
            types_stack.push(cur);
            int id = ind;
            ReadNext();
            Priority8();
            CheckBin(id);
        }
    }
}

void SyntaxAnalyzer::Operation6() {
    if (cur == "+") {
        ReadNext();
        return;
    }
    if (cur == "-") {
        ReadNext();
        return;
    }
    throw std::runtime_error(
        "Line " + std::to_string(lines[ind]) + ": expected operation, got '" + cur + "' instead.");
}

void SyntaxAnalyzer::Priority6() {
    Priority7();
    if (ind + 1 >= lexemes.size()) {
        return;
    } else {
        while (cur == "+" || cur == "-") {
//            Operation6();
            types_stack.push(cur);
            int id = ind;
            ReadNext();
            Priority7();
            CheckBin(id);
        }
    }
}

void SyntaxAnalyzer::Operation5() {
    if (cur == ">") {
        ReadNext();
        return;
    }
    if (cur == "<") {
        ReadNext();
        return;
    }
    if (cur == "<=") {
        ReadNext();
        return;
    }
    if (cur == ">=") {
        ReadNext();
        return;
    }
    throw std::runtime_error(
        "Line " + std::to_string(lines[ind]) + ": expected operation, got '" + cur + "' instead.");
}

void SyntaxAnalyzer::Priority5() {
    Priority6();
    if (ind + 1 >= lexemes.size()) {
        return;
    } else {
        while (cur == ">" || cur == "<" || cur == ">=" || cur == "<=") {
//            Operation5();
            types_stack.push(cur);
            int id = ind;
            ReadNext();
            Priority6();
            CheckBin(id);
        }
    }
}

void SyntaxAnalyzer::Operation4() {
    if (cur == "==") {
        ReadNext();
        return;
    }
    if (cur == "!=") {
        ReadNext();
        return;
    }
    throw std::runtime_error(
        "Line " + std::to_string(lines[ind]) + ": expected operation, got '" + cur + "' instead.");
}

void SyntaxAnalyzer::Priority4() {
    Priority5();
    if (ind + 1 >= lexemes.size()) {
        return;
    } else {
        while (cur == "==" || cur == "!=") {
//            Operation4();
            types_stack.push(cur);
            int id = ind;
            ReadNext();
            Priority5();
            CheckBin(id);
        }
    }
}

void SyntaxAnalyzer::Priority3() {
    Priority4();
    if (ind + 1 >= lexemes.size()) {
        return;
    } else {
        while (cur == "&&") {
            types_stack.push(cur);
            int id = ind;
            ReadNext();
            Priority4();
            CheckBin(id);
        }
    }
}

void SyntaxAnalyzer::Priority2() {
    Priority3();
    if (ind + 1 >= lexemes.size()) {
        return;
    } else {
        while (cur == "||") {
            types_stack.push(cur);
            int id = ind;
            ReadNext();
            Priority3();
            CheckBin(id);
        }
    }
}

void SyntaxAnalyzer::Priority1() {
    if (!IsAssOp(cur) && cur != ",")
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected operation, got '" + cur
                + "' instead.");
    types_stack.push(cur);
    ReadNext();
}

void SyntaxAnalyzer::Expression() {
    if (ind + 1 <= lexemes.size() && lexemes[ind + 1] == "[") {
        int c_ind = ind;
        if (cur_type != 2) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected name, got '" + cur
                    + "' instead.");
        }
        ReadNext();
        ReadNext();
        Priority2();
        if (cur != "]") {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected ']', got '" + cur
                    + "' instead.");
        }
        ReadNext();
        if (IsAssOp(cur)) {
            types_stack.push(GetType(c_ind));
            if (IsConst(c_ind)) {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": operand must be modifiable lvalue.");
            }
            Priority1();
            Priority2();
        } else {
            ind = c_ind - 1;
            ReadNext();
            Priority2();
        }
        return;
    }
    if (ind + 1 <= lexemes.size() && IsAssOp(lexemes[ind + 1])) {
        int c_ind = ind;
        if (cur_type != 2) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected name, got '" + cur
                    + "' instead.");
        }

        ReadNext();
        if (cur == "[") {
            ReadNext();
            Priority2();
            if (cur != "]") {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ']', got '" + cur
                        + "' instead.");
            }
            ReadNext();
        }
        types_stack.push(GetType(c_ind));
        if (IsConst(c_ind)) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": operand must be modifiable lvalue.");
        }
        Priority1();
        Priority2();
    } else {
        Priority2();
    }
    //    }
}

void SyntaxAnalyzer::Input() {
    if (cur == "input") {
        ReadNext();
        if (cur == "(") {
            ReadNext();
            if (cur_type == 2) {
                ReadNext();
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected name, got '" + cur
                        + "' instead.");
            }
            if (cur == ")") {
                ReadNext();
                if (cur == ";") {
                    ReadNext();
                } else {
                    throw std::runtime_error(
                        "Line " + std::to_string(lines[ind]) + ": expected ';', got '" + cur
                            + "' instead.");
                }
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ')', got '" + cur
                        + "' instead.");
            }
        } else {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected '(', got '" + cur
                    + "' instead.");
        }
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'input', got '" + cur
                + "' instead.");
    }
}

void SyntaxAnalyzer::Print() {
    if (cur == "print") {
        ReadNext();
        if (cur == "(") {
            ReadNext();
            while (cur != ")") {
                if (ind + 1 == lexemes.size()) {
                    throw std::runtime_error(
                        "Line " + std::to_string(lines[ind]) + ": expected ')', got '" + cur
                            + "' instead.");
                }

                if (cur_type == 8) {
                    ReadNext();
                } else {
                    if (cur_type == 2) {
                        ReadNext();
                        if (cur == "[") {
                            ReadNext();
                            Priority2();
                            if (cur != "]") {
                                throw std::runtime_error(
                                    "Line " + std::to_string(lines[ind]) + ": expected ], got '"
                                        + cur
                                        + "' instead.");
                            }
                            ReadNext();
                        }
                    } else {
                        throw std::runtime_error(
                            "Line " + std::to_string(lines[ind]) + ": expected name, got '" + cur
                                + "' instead.");
                    }
                }

            }
            ReadNext();
            if (cur == ";") {
                ReadNext();
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ';', got '" + cur
                        + "' instead.");
            }
        } else {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected '(', got '" + cur
                    + "' instead.");
        }
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'print', got '" + cur
                + "' instead.");
    }
}

void SyntaxAnalyzer::Return() {
    if (cur == "return") {
        ReadNext();
        if (cur == ";") {
            ReadNext();
        } else {
            Priority2();
            ReadNext();
            if (cur == ";") {
                ReadNext();
            }
        }
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'return', got '" + cur
                + "' instead.");
    }
}

void SyntaxAnalyzer::Break() {
    if (cur == "break") {
        if (cycle_counter == 0) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind])
                    + ": a break statement may only be used within a loop.");
        }
        ReadNext();
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'break', got '" + cur
                + "' instead.");
    }
}

void SyntaxAnalyzer::Continue() {
    if (cur == "continue") {
        if (cycle_counter == 0) {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind])
                    + ": a continue statement may only be used within a loop.");
        }
        ReadNext();
    } else {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected 'continue', got '" + cur
                + "' instead.");
    }
}

void SyntaxAnalyzer::Operator() {
    if (cur == "_")
        ReadNext();
    if (ind + 1 == lexemes.size()) {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected '}', got '" + cur + "' instead.");
    }
    std::string one_step = lexemes[ind + 1];
    int one_step_ind = types[ind + 1];

    one_step = cur;
    one_step_ind = cur_type;

    if (one_step_ind == 1) {
        if (one_step == "if") {
            IfOperator();
        } else if (one_step == "while") {
            WhileOperator();
        } else if (one_step == "for") {
            ForOperator();
        } else if (one_step == "input") {
            Input();
        } else if (one_step == "print") {
            Print();
        } else if (one_step == "return") {
            Return();
        } else if (one_step == "break") {
            Break();
            if (cur == ";") {
                ReadNext();
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ';', got '" + cur
                        + "' instead.");
            }
        } else if (one_step == "continue") {
            Continue();
            if (cur == ";") {
                ReadNext();
            } else {
                throw std::runtime_error(
                    "Line " + std::to_string(lines[ind]) + ": expected ';', got '" + cur
                        + "' instead.");
            }
        } else if (IsType(one_step) || one_step == "const" || one_step == "list") {
            int c_ind = ind;

            VarDeclaration();

            //	//here
            //	Push_id(c_ind);

            if (cur == ";") {
                ReadNext();
            }
        } else {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected operator, got '" + cur
                    + "' instead.");
        }

    } else {
        if (cur == "_") {
            ReadNext();
        }
        Expression();
        if (cur == ";") {
            ReadNext();
        } else {
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected ';', got '" + cur
                    + "' instead.");
        }
    }
}

void SyntaxAnalyzer::Analyze() {
    ReadLexemes();
    ReadNext();
    int ind_copy = ind;
    std::string cur_copy = cur;
    in_preread = true;
    PreReadLexemes();
    ind = ind_copy;
    cur = cur_copy;
    Program();
    if (cur_type != 0)
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected end of program, got '" + cur
                + "' instead.");
}

void SyntaxAnalyzer::Type() {
    if (cur == "list") {
        var_decl_type += cur;
        ReadNext();
        if (cur != "[")
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected '[', got '" + cur
                    + "' instead.");
        var_decl_type += cur;
        ReadNext();
        if (cur_type != 3)
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected int literal, got '" + cur
                    + "' instead.");
        var_decl_type += cur;
        ReadNext();
        if (cur != "]")
            throw std::runtime_error(
                "Line " + std::to_string(lines[ind]) + ": expected ']', got '" + cur
                    + "' instead.");
        var_decl_type += cur;
        ReadNext();

    }
    if (cur != "int" && cur != "float" && cur != "bool" && cur != "string") {
        throw std::runtime_error(
            "Line " + std::to_string(lines[ind]) + ": expected type, got '" + cur + "' instead.");
    }
    var_decl_type += cur;
    ReadNext();
}

void SyntaxAnalyzer::GiveArguments() {
    if (cur == ")")
        return;
    Priority2();
    while (cur == ",") {
        ReadNext();
        Priority2();
    }
}