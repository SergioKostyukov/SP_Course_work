#include "first_view.h"

#define info_line res.line_info[res.count]

// word view
void label_word_view(string &word, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    if (word[word.size() - 1] == ':') {
        word.resize(word.size() - 1);
    }

    res.labels[word].dec_size = res.Full_size;
    res.labels[word].line_number = res.count;
    res.labels[word].count++;
    res.labels[word].type = "NEAR";
    res.labels[word].address = res.GetLocalAddress();
    res.labels[word].segment = res.segment;

    res.label = word;
} // parsing <label> type

void mnem_word_view(std::string &word, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    if (word == "end") {
        info_line.command = word;
    } else if (word == "data") {
        if (res.segment_use == 0) {
            res.segment = "DATA";
            res.segment_use++;
        } else {
            res.SetDataSize();
            res.segment_use = 0;
        }
    } else if (word == "code") {
        if (res.segment_use == 0) {
            res.Full_size = 0;
            res.SetAddressNull();
            res.segment = "CODE";
            res.segment_use++;
        } else {
            res.SetCodeSize();
            res.segment_use = 0;
        }
    } else if (word == "cmpsb" || word == "dec" || word == "mov" || word == "inc" ||
               word == "jbe" || word == "and" ||
               word == "or" || word == "idiv" || word == "jmp") {
        info_line.command = word;
        info_line.size += 1;
    } else if (word == "bsr") {
        info_line.command = word;
        info_line.size += 2;
    } else if (word == "db") {
        res.labels[res.label].type = "BYTE";
        info_line.size += 1;
    } else if (word == "dw") {
        res.labels[res.label].type = "WORD";
        info_line.size += 2;
    } else if (word == "dd") {
        res.labels[res.label].type = "DWORD";
        info_line.size += 4;
    } else if (word == "=") {
        res.labels[res.label].type = "CONST";
    }
} // parsing <mnem> type

void op1_view(string &word, struct data &res, bool &is_status_changed) {
    if (word == ":" || word == "segment" || word == "]" ||
        word == "+" || word == "*" || word == "/" ||
        word == "-" || word == "ends" || word == "ptr" ||
        word == "dword" || info_line.command == "end") { // there must be all correct symbols that don`t change size
    } else if (word == "short") {
        info_line.mod = word;
    } else if (word == ",") {
        is_status_changed = true;
    } else if (word == "[") {
        res.is_op1_brackets = true;
        res.first_operand += 1;
    } else if (word == "ss" || word == "ds" || word == "cs" ||
               word == "es" || word == "fs" || word == "gs") {
        res.first_operand += 1;
        if (res.segment == "CODE" && word != "cs" || res.segment == "DATA" && word != "ds") {
            info_line.segment = word;
        }
    } else if (word == "ah" || word == "al" || word == "ch" || word == "cl" ||
               word == "dh" || word == "dl" || word == "bh" || word == "bl") {
        if (res.is_op1_brackets) res.errors++;
        else if (info_line.command == "mov") res.first_operand += 1;
        else res.first_operand += 1;
    } else if (word == "ebx" || word == "edi" || word == "eax" || word == "ecx" ||
               word == "edx" || word == "esp" || word == "ebp" || word == "esi") {
        if (res.is_op1_brackets) {
            res.reg_count++;
            if (res.reg_count == 2) res.first_operand += 1;
            if (info_line.segment == "ss" && (word == "ebp" || word == "esp")) {
                res.first_operand -= 1;
                info_line.segment.clear();
            }
            if (info_line.segment == "ds" && word != "ebp" && word != "esp") {
                res.first_operand -= 1;
                info_line.segment.clear();
            } else {
                info_line.segment.clear();
            }
        } else if (info_line.command == "mov") res.first_operand += 4;
        else res.first_operand += 1;
    } else if (word[word.size() - 1] == 'h') {
        if (res.segment == "CODE") res.first_operand += word.size() / 2;
    } else if (word[word.size() - 1] == 'b') {
        if (res.segment == "CODE") {
            if(res.label == "") res.first_operand += (word.size() + 4) / 8;
        }
    } else if (isdigit(word[0]) || word[0] == '-') {
        if (res.is_op1_brackets) res.first_operand += 1;
        else if (res.labels.find(res.label) != res.labels.end()
                 && res.labels[res.label].type == "CONST") {
            res.first_operand += 0;
        }
    } else if (word[word.size() - 1] == '\'') {
        info_line.size = 0;
        res.first_operand += (word.size() - 2);
    } else {
        //res.labels[word].count++;
        res.label = word;
        if (info_line.command == "jmp") {
            if (res.labels[word].count >= 1 || info_line.mod == "short") {
                res.first_operand += 1;
            } else {
                info_line.size += 1;
                res.first_operand += 4;
            }
        } else if (info_line.command == "jbe") {
            if (res.labels[word].count >= 1) {
                res.first_operand += 1;
            } else {
                info_line.size += 1;
                res.first_operand += 4;
            }
        }
    }
}

void op2_view(string &word, struct data &res) {
    if (info_line.command == "mov") {
        if (res.labels.find(word) != res.labels.end()) res.labels[word].count++;
        res.second_operand = 0;
    } else if (word == ":" || word == "]" || word == "+" ||
               word == "*" || word == "/" || word == "-") // there must be all correct symbols that don`t change size
    {}
    else if (word == "[") {
        res.is_op2_brackets = true;
        res.first_operand = 0;
        res.second_operand += 1;
    } else if (word == "ss" || word == "ds" || word == "cs" ||
               word == "es" || word == "fs" || word == "gs") {
        res.second_operand += 1;
        if (res.segment == "CODE" && word != "cs" || res.segment == "DATA" && word != "ds") {
            info_line.segment = word;
        }
    } else if (word == "ah" || word == "al" || word == "ch" || word == "cl" ||
               word == "dh" || word == "dl" || word == "bh" || word == "bl") {
        if (res.is_op2_brackets) res.errors++;
        else if (res.is_op1_brackets) res.second_operand = 0;
        else res.second_operand += 1;
    } else if (word == "ebx" || word == "edi" || word == "eax" || word == "ecx" ||
               word == "edx" || word == "esp" || word == "ebp" || word == "esi") {
        if (res.is_op2_brackets) {
            res.reg_count++;
            if (res.reg_count == 2) res.second_operand += 1;
            if (info_line.segment == "ss" && (word == "ebp" || word == "esp")) {
                res.second_operand -= 1;
                info_line.segment.clear();
            }
            if (info_line.segment == "ds" && word != "ebp" && word != "esp") {
                res.first_operand -= 1;
                info_line.segment.clear();
            } else {
                info_line.segment.clear();
            }
        } else if (res.is_op1_brackets) res.second_operand = 0;
        else res.second_operand += 1;
    } else if (word[word.size() - 1] == 'h') {
        res.second_operand += word.size() / 2;
    } else if (word[word.size() - 1] == 'b') {
        if(res.label == "") res.first_operand += (word.size() + 4) / 8;
    } else if (isdigit(word[0]) || word[0] == '-') {
        res.second_operand += 1;
    } else if (word[word.size() - 1] == '\'') {
        res.second_operand += 1;
    } else {
        //res.labels[word].count++;
        if (res.labels[word].type == "CONST") res.second_operand += 1;
        else if (info_line.command == "jmp") {
            if (res.labels[word].count >= 1 || info_line.mod == "short") {
                res.first_operand += 1;
            } else {
                info_line.size += 1;
                res.first_operand += 4;
            }
        } else if (info_line.command == "jbe") {
            if (res.labels[word].count >= 1) {
                res.first_operand += 1;
            } else {
                info_line.size += 1;
                res.first_operand += 4;
            }
        }
    }
}

void op_word_view(string &word, bool &is_status_changed, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    if (word[word.size() - 1] == ',') {
        word.resize(word.size() - 1);
        is_status_changed = true;
    } // we must reduce the length for next parsing

    if (word[0] == '[') {
        for (int i = 1; i < word.size(); i++) {
            word[i - 1] = word[i];
        }
        word.resize(word.size() - 1);
        if (res.status == "<op1>") {
            res.is_op1_brackets = true;
            res.first_operand += 1;
        } else {
            res.is_op2_brackets = true;
            res.first_operand = 0;
            res.second_operand += 1;
        }
    } // check symbol before word and reduce the length

    if (word[word.size() - 1] == ':' || word[word.size() - 1] == ']') {
        word.resize(word.size() - 1);
    } // check symbol after word, reduce the length

    if (res.status == "<op1>") {
        op1_view(word, res, is_status_changed);
    } else if (res.status == "<op2>") {
        op2_view(word, res);
    }
} // parsing <op> type

// line view
// line structure: <label><mnem><op1>,<op2>
string view_first_word(string &word) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    std::vector<string> mnem = {"begin", "data", "code", "end", "cmpsb", "inc",
                                "mov", "jbe", "dec", "bsr", "and", "or", "idiv", "jmp"}; // all <mnem> codes

    for (auto &i: mnem) {
        if (word == i) return "<mnem>";
    }
    return "<label>";
}

void line_view(const string &data, struct data &res) {
    if (!data.empty()) {
        std::stringstream line;
        line << data; // use line as cin

        bool is_status_changed = false; // check if we need change <op1> -> <op2>

        string word;
        string status;

        line >> word;
        status = view_first_word(word); // <mnem> or <label>

        do {
            res.status = status;
            if (status == "<label>") {
                label_word_view(word, res);
                status = "<mnem>"; // <label> has only one symbol
            } else if (status == "<mnem>") {
                mnem_word_view(word, res);
                status = "<op1>"; // <mnem> has only one symbol
            } else if (status == "<op1>") {
                op_word_view(word, is_status_changed, res);
                if (is_status_changed) {
                    res.reg_count = 0;
                    status = "<op2>";
                } // is we have ',' after <op1>
            } else if (status == "<op2>") {
                op_word_view(word, is_status_changed, res);
            }
        } while (line >> word);

        // save info about line
        info_line.size += res.first_operand;
        info_line.size += res.second_operand;

        info_line.dec_size = res.Full_size;
        res.Full_size += info_line.size;

        info_line.address = res.GetLocalAddress();

        res.ChangeAddress(info_line.size);

        // clear iteration parameters
        res.reg_count = 0;
        res.is_op1_brackets = false;
        res.is_op2_brackets = false;
        res.first_operand = 0;
        res.second_operand = 0;
        res.label.clear();
        res.local_address.clear();
    }

    res.count++;
}

// file view
void file_first_view(const string &name, data &res) {
    std::ifstream file(name);
    if (!file.is_open()) {
        std::cout << "Open error" << std::endl;
    } else {
        string line;
        while (getline(file, line)) { // read line from file
            line_view(line, res);
        }
    }

    res.count = 0;

    file.close();
}