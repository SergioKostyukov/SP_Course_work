#include "second_view.h"

using std::cout;
using std::endl;
#define info_line res.line_info[res.count]
#define info_label res.labels[res.label]

// word view
void Label_word_view(string &word, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    if (word[word.size() - 1] == ':') {
        word.resize(word.size() - 1);
    }

    // check redefinition of label
    if (res.labels[word].used_in_first_pos) {
        res.errors++;
        info_line.error_info = "**Error**(" + std::to_string(res.count) + ") Redefinition of label " + word;
    }

    if (res.labels[word].type != "CONST") res.labels[word].used_in_first_pos = true;
    res.label = word;
} // parsing <label> type

void Mnem_word_view(std::string &word, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    if (word == "end") {
        info_line.command = word;
        info_line.address.clear();
    } else if (word == "data") {
        if (res.segment_use == 0) {
            res.segment = "DATA";
            res.segment_use++;
        } else {
            res.segment_use = 0;
        }
    } else if (word == "code") {
        if (res.segment_use == 0) {
            res.SetAddressNull();
            res.segment = "CODE";
            res.segment_use++;
        } else {
            res.segment_use = 0;
        }
    } else if (word == "cmpsb" || word == "dec" || word == "mov" || word == "inc" ||
               word == "jbe" || word == "and" || word == "bsr" ||
               word == "or" || word == "idiv" || word == "jmp") {
        info_line.command = word;
        // add command code
    } else if (word == "=") {
        info_label.value.clear();
    }
} // parsing <mnem> type

void Negative_binary_number(string &word) {
    for (auto &i: word) {
        if (i == '0') i = '1';
        else if (i == '1') i = '0';
    }

    if (word[word.size() - 1] == '0') {
        word[word.size() - 1] = '1';
    } else {
        int i = 0;
        while (word[word.size() - 1 - i] != '0') {
            word[word.size() - 1 - i] = '0';
            i++;
        }
        if (i < word.size()) {
            word[word.size() - 1 - i] = '1';
        }
    }
}

void Number_view(string &word, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), toupper);

    // determinate size of number
    if (res.segment == "DATA") {
        if (info_label.type == "BYTE") {
            info_line.number_size = 2;
        } else if (info_label.type == "WORD") {
            info_line.number_size = 4;
        } else if (info_label.type == "DWORD") {
            info_line.number_size = 8;
        }
    }
    if (info_label.type == "CONST") {
        if (word[0] == '-') {
            info_label.is_negative = true;
            for (int i = 1; i < word.size(); i++) {
                word[i - 1] = word[i];
            }
            word.resize(word.size() - 1);
        } else {
            info_label.is_negative = false;
        }
        info_line.number_size = 4;
    } else if (info_line.number_size == 0) {
        info_line.number_size = 2;
    }

    // filling empty number code
    for (int i = 0; i < info_line.number_size; i++) {
        info_line.number += '0';
    }

    // filling value number code
    bool is_negative_number = false;
    if (word[0] == '-') {
        is_negative_number = true;
        for (int i = 1; i < word.size(); i++) {
            word[i - 1] = word[i];
        }
        word.resize(word.size() - 1);
    }

    if (word[word.size() - 1] == 'H') {
        for (int i = 1; i < word.size(); i++) {
            word[i - 1] = word[i];
        }
        word.resize(word.size() - 2);
        for (int i = 0; i <= word.size(); i++) {
            info_line.number[info_line.number.size() - i] = word[word.size() - i];
        }
    } else if (word[word.size() - 1] == 'B') {
        // delete symbol 'H'
        word.resize(word.size() - 1);

        // filling empty bites
        while (word.size() % 4 != 0) {
            word.resize(word.size() + 1);
            for (int i = word.size() - 1; i > 0; i--) {
                word[i] = word[i - 1];
            }
            word[0] = '0';
        }

        // translate to hexadecimal
        int k;
        int buf;
        for (int i = 0; i < word.size() / 4; i++) {
            k = 0;
            buf = 1;
            for (int j = 0; j < 4; j++) {
                if (word[word.size() - 4 * i - j - 1] == '1') {
                    k += buf;
                }
                buf *= 2;
            }
            info_line.number[info_line.number.size() - i - 1] = res.GetAddress(k);
        }
    } else {
        int num = atoi(word.c_str());
        string binary_result;

        unsigned long int i = pow(2, info_line.number_size * 4 - 1);
        while (i != 1) {
            if (num >= i) {
                num -= i;
                binary_result += '1';
            } else {
                binary_result += '0';
            }
            i = i / 2;
            if (i == 1 && num == 1) binary_result += '1';
            else if (i == 1 && num == 0) binary_result += '0';
        }
        // filling empty bites
        while (binary_result.size() % 4 != 0) {
            binary_result.resize(binary_result.size() + 1);
            for (int i = binary_result.size() - 1; i > 0; i--) {
                binary_result[i] = binary_result[i - 1];
            }
            binary_result[0] = '0';
        }

        if (is_negative_number) {
            Negative_binary_number(binary_result);
        }

        // translate to hexadecimal
        int k;
        int buf;
        for (int i = 0; i < binary_result.size() / 4; i++) {
            k = 0;
            buf = 1;
            for (int j = 0; j < 4; j++) {
                if (binary_result[binary_result.size() - 4 * i - j - 1] == '1') {
                    k += buf;
                }
                buf *= 2;
            }
            info_line.number[info_line.number.size() - i - 1] = res.GetAddress(k);
        }
    }

    // save const value
    if (info_label.type == "CONST") {
        info_label.value = info_line.number;
    }

    info_line.number += " ";
}

void String_view(string &word, struct data &res) {
    // delete brackets ''
    for (int i = 1; i < word.size(); i++) {
        word[i - 1] = word[i];
    }
    word.resize(word.size() - 2);

    // get symbol ASCII code and change to binary
    int code = 0;
    string buf;
    for (auto &i: word) {
        code = i;
        buf = std::to_string(code);
        info_line.number_size = 2;
        Number_view(buf, res);
    }
}

void Op1_view(string &word, struct data &res, bool &is_status_changed) {
    if (word == ":" || word == "segment" || word == "]" ||
        word == "+" || word == "*" || word == "/" ||
        word == "-" || word == "ends" || word == "ptr" ||
        word == "dword" || info_line.command == "end" ||
        word == "short") { // there must be all correct symbols that don`t change size
    } else if (word == ",") {
        is_status_changed = true;
    } else if (word == "[") {
        res.is_op1_brackets = true;
    } else if (word == "ss" || word == "ds" || word == "cs" ||
               word == "es" || word == "fs" || word == "gs") {
        if (res.segment == "CODE" && word != "cs" || res.segment == "DATA" && word != "ds") {
            info_line.segment = word;
        }
    } else if (word == "ah" || word == "al" || word == "ch" || word == "cl" ||
               word == "dh" || word == "dl" || word == "bh" || word == "bl") {
        if (res.is_op1_brackets) {
            info_line.error_info = "**Error** (" + std::to_string(res.count) + ") Error register in brackets";
        } else if (info_line.command == "mov") {
            info_line.number_size = 2;
        }

        if (!res.is_op1_brackets) {
            info_line.main_register = word;
        }
    } else if (word == "ebx" || word == "edi" || word == "eax" || word == "ecx" ||
               word == "edx" || word == "esp" || word == "ebp" || word == "esi") {
        if (res.is_op1_brackets) {
            info_line.reg_in_brackets.push_back(word);
        } else if (info_line.command == "mov") {
            info_line.number_size = 8;
        }

        if (!res.is_op1_brackets) {
            info_line.main_register = word;
        }
    } else if (isdigit(word[0]) || word[0] == '-') {
        info_line.num_value = word;
        Number_view(word, res);
    } else if (word[word.size() - 1] == '\'') {
        String_view(word, res);
    } else {
        res.label = word;
        if (info_line.command == "jmp" || info_line.command == "jbe") {
            if (res.count < res.labels[word].line_number) {
                info_line.jmp_vector = true;
            }
            info_line.jmp_length = res.labels[word].dec_size - info_line.dec_size;
        } else { // if we don`t seen this label before, that it`s error
            if (info_label.type == "NULL") {
                res.errors++;
                info_line.error_info = "**Error** (" + std::to_string(res.count) + ") Undefined label";
            }
        }
    }
}

void Op2_view(string &word, struct data &res) {
    if (word == ":" || word == "]" || word == "+" ||
        word == "*" || word == "/" || word == "-") // there must be all correct symbols that don`t change size
    {}
    else if (word == "[") {
        res.is_op2_brackets = true;
    } else if (word == "ss" || word == "ds" || word == "cs" ||
               word == "es" || word == "fs" || word == "gs") {
        if (res.segment == "CODE" && word != "cs" || res.segment == "DATA" && word != "ds") {
            info_line.segment = word;
        }
    } else if (word == "ah" || word == "al" || word == "ch" || word == "cl" ||
               word == "dh" || word == "dl" || word == "bh" || word == "bl") {
        if (res.is_op2_brackets) {
            info_line.error_info = "**Error** (" + std::to_string(res.count) + ") Error register in brackets";
        }
        if (!res.is_op2_brackets) {
            info_line.main_register = word;
        }
    } else if (word == "ebx" || word == "edi" || word == "eax" || word == "ecx" ||
               word == "edx" || word == "esp" || word == "ebp" || word == "esi") {
        if (res.is_op2_brackets) {
            info_line.reg_in_brackets.push_back(word);
        }
        if (!res.is_op2_brackets) {
            info_line.main_register = word;
        }
    } else if (isdigit(word[0]) || word[0] == '-') {
        info_line.num_value = word;
        Number_view(word, res);
    } else if (word[0] == '\'') {
        String_view(word, res);
    } else {
        res.label = word;
        if (info_line.command == "jmp" || info_line.command == "jbe") {
            if (res.count < res.labels[word].line_number) {
                info_line.jmp_vector = true;
            }
            info_line.jmp_length = res.labels[word].dec_size - info_line.dec_size;
        } else { // if we don`t seen this label before, that it`s error
            if (info_label.type == "NULL") {
                res.errors++;
                info_line.error_info = "**Error** (" + std::to_string(res.count) + ") Undefined label";
            }
        }
    }
}

void Op_word_view(string &word, bool &is_status_changed, struct data &res) {
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
        } else {
            res.is_op2_brackets = true;
        }

    } // check symbol before word and reduce the length

    if (word[word.size() - 1] == ':' || word[word.size() - 1] == ']') {
        word.resize(word.size() - 1);
    } // check symbol after word, reduce the length

    if (res.status == "<op1>") {
        Op1_view(word, res, is_status_changed);
    } else if (res.status == "<op2>") {
        Op2_view(word, res);
    }
} // parsing <op> type

// line view
// line structure: <label><mnem><op1>,<op2>
string View_first_word(string &word) {
    std::transform(word.begin(), word.end(), word.begin(), tolower);

    std::vector<string> mnem = {"begin", "data", "code", "end", "cmpsb", "inc",
                                "mov", "jbe", "dec", "bsr", "and", "or", "idiv", "jmp"}; // all <mnem> codes

    for (auto &i: mnem) {
        if (word == i) return "<mnem>";
    }
    return "<label>";
}

void binary_translate(string &word, struct data &res) {
    std::transform(word.begin(), word.end(), word.begin(), toupper);

    int obj_size = 2;
    string obj_byte;
    // filling empty number code
    for (int i = 0; i < obj_size; i++) {
        obj_byte += '0';
    }

    int k;
    int buf;
    for (int i = 0; i < word.size() / 4; i++) {
        k = 0;
        buf = 1;
        for (int j = 0; j < 4; j++) {
            if (word[word.size() - 4 * i - j - 1] == '1') {
                k += buf;
            }
            buf *= 2;
        }
        obj_byte[obj_byte.size() - i - 1] = res.GetAddress(k);
    }

    info_line.command_code += obj_byte + " ";
}

void decimal_translate(int value, struct data &res) {
    string result = "00";
    bool is_end = false;
    int i;
    int j = 0;
    while (!is_end) {
        i = 0;
        if (value <= 16) is_end = true;
        else {
            while (value > 16) {
                value = value - 16;
                i++;
            }
        }
        result[result.size() - j - 1] = res.GetAddress(value);
        value = i;
        j++;
    }
    info_line.command_code += result + " ";
}

string Negative_num(int num) {
    string buf;

    int i = pow(2, 7);
    while (i != 1) {
        if (num >= i) {
            num -= i;
            buf += '1';
        } else {
            buf += '0';
        }
        i = i / 2;
        if (i == 1 && num == 1) buf += '1';
        else if (i == 1 && num == 0) buf += '0';
    }
    // filling empty bites
    while (buf.size() < 8) {
        buf.resize(buf.size() + 1);
        for (int i = buf.size() - 1; i > 0; i--) {
            buf[i] = buf[i - 1];
        }
        buf[0] = '0';
    }

    Negative_binary_number(buf);
    return buf;
}

void Line_view(const string &data, std::ostream &result, struct data &res) {
    if (!data.empty()) {
        std::stringstream line;
        line << data; // use line as cin

        bool is_status_changed = false; // check if we need change <op1> -> <op2>

        string word;
        string status;

        line >> word;
        status = View_first_word(word); // <mnem> or <label>

        do {
            res.status = status;
            if (status == "<label>") {
                Label_word_view(word, res);
                status = "<mnem>"; // <label> has only one symbol
            } else if (status == "<mnem>") {
                Mnem_word_view(word, res);
                status = "<op1>"; // <mnem> has only one symbol
            } else if (status == "<op1>") {
                Op_word_view(word, is_status_changed, res);
                if (is_status_changed) {
                    res.reg_count = 0;
                    status = "<op2>";
                } // is we have ',' after <op1>
            } else if (status == "<op2>") {
                Op_word_view(word, is_status_changed, res);
            }
        } while (line >> word);

        // create code command
        if (!info_line.segment.empty()) {
            if (info_line.segment == "ss") {
                bool is_true = true;
                for (auto &i: info_line.reg_in_brackets) {
                    if (i == "ebp" || i == "esp") {
                        is_true = false;
                    }
                }
                if (is_true) {
                    info_line.command_code += res.segments[info_line.segment] + ": ";
                }
            } else if (info_line.segment == "ds") {
                bool is_true = false;
                for (auto &i: info_line.reg_in_brackets) {
                    if (i == "ebp" || i == "esp") {
                        is_true = true;
                    }
                }
                if (is_true) {
                    info_line.command_code += res.segments[info_line.segment] + ": ";
                }
            } else {
                info_line.command_code += res.segments[info_line.segment] + ": ";
            }
        } // byte for segment

        if (info_label.type == "CONST" && info_line.command.empty()) {
            info_line.command_code += "=";
            if (info_label.is_negative) info_line.command_code += "-";
            info_line.address.clear();
        } else if (info_label.type == "CONST" && !info_line.command.empty()) {
            if (!info_label.is_negative) {
                info_line.number += "0000" + info_label.value;
            } else {
                int num = atoi(info_label.value.c_str());
                string binary_result;

                // filling empty number code
                for (int i = 0; i < info_line.number_size; i++) {
                    info_line.number += '0';
                }

                unsigned long int i = pow(2, info_line.number_size * 4 - 1);
                while (i != 1) {
                    if (num >= i) {
                        num -= i;
                        binary_result += '1';
                    } else {
                        binary_result += '0';
                    }
                    i = i / 2;
                    if (i == 1 && num == 1) binary_result += '1';
                    else if (i == 1 && num == 0) binary_result += '0';
                }
                // filling empty bites
                while (binary_result.size() % 4 != 0) {
                    binary_result.resize(binary_result.size() + 1);
                    for (int i = binary_result.size() - 1; i > 0; i--) {
                        binary_result[i] = binary_result[i - 1];
                    }
                    binary_result[0] = '0';
                }

                if (info_label.is_negative) {
                    Negative_binary_number(binary_result);
                }

                // translate to hexadecimal
                int k;
                int buf;
                for (int i = 0; i < binary_result.size() / 4; i++) {
                    k = 0;
                    buf = 1;
                    for (int j = 0; j < 4; j++) {
                        if (binary_result[binary_result.size() - 4 * i - j - 1] == '1') {
                            k += buf;
                        }
                        buf *= 2;
                    }
                    info_line.number[info_line.number.size() - i - 1] = res.GetAddress(k);
                }
            }
        } // info about constant

        string buffer;
        // command code
        if (!info_line.command.empty() && info_line.command != "end") {
            buffer.clear(); // empty byte template
            if (info_line.command == "mov") {
                if (res.registers_8.find(info_line.main_register) != res.registers_8.end()) {
                    buffer = "10110000";
                    for (int i = 0; i < 3; i++) {
                        buffer[buffer.size() - i - 1] = res.registers_8[info_line.main_register][2 - i];
                    }
                } else {
                    buffer = "10111000";
                    for (int i = 0; i < 3; i++) {
                        buffer[buffer.size() - i - 1] = res.registers_16_32[info_line.main_register][2 - i];
                    }
                }
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "dec") {
                buffer = "11111111";
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "cmpsb") {
                buffer = "10100110";
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "inc") {
                if (res.registers_8.find(info_line.main_register) != res.registers_8.end()) {
                    buffer = "11111110";
                    binary_translate(buffer, res); // add one byte to result code
                    buffer = "11000";
                    buffer += res.registers_8[info_line.main_register];
                } else {
                    buffer = "01000";
                    buffer += res.registers_16_32[info_line.main_register];
                }
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "bsr") {
                buffer = "00001111";
                binary_translate(buffer, res); // add one byte to result code
                buffer = "10111101";
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "and") {
                if (res.registers_8.find(info_line.main_register) != res.registers_8.end()) {
                    buffer = "00100000";
                } else {
                    buffer = "00100001";
                }
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "or") {
                if (info_line.num_value[info_line.num_value.size() - 1] == 'h') {
                    buffer = "10000001";
                } else if (info_line.num_value[info_line.num_value.size() - 1] == 'b') {
                    buffer = "10000011";
                }
                binary_translate(buffer, res); // add one byte to result code
            } else if (info_line.command == "idiv") {
                if (res.registers_8.find(info_line.main_register) != res.registers_8.end()) {
                    buffer = "11110110";
                    binary_translate(buffer, res); // add one byte to result code
                    buffer = "11111";
                    buffer += res.registers_8[info_line.main_register];
                } else {
                    buffer = "11110111";
                    binary_translate(buffer, res); // add one byte to result code
                    buffer = "11111";
                    buffer += res.registers_16_32[info_line.main_register];
                }
                binary_translate(buffer, res);
            } else if (info_line.command == "jmp") {
                buffer = "11101011";
                binary_translate(buffer, res);
                if (info_line.jmp_length < 0 || !info_line.jmp_vector) { // jump back
                    buffer = Negative_num(2 - info_line.jmp_length);
                    binary_translate(buffer, res);
                } else if (info_line.jmp_vector && info_line.jmp_length == 0) {
                    buffer = "00000000";
                    binary_translate(buffer, res);
                } else { // jump forward
                    decimal_translate(info_line.jmp_length - 2, res);

                    if (info_line.mod != "short") {
                        buffer = "10010000";
                        for (int i = 0; i < 4; i++) {
                            binary_translate(buffer, res);
                        }
                    }
                }
            } else if (info_line.command == "jbe") {
                buffer = "01110110";
                binary_translate(buffer, res);
                if (info_line.jmp_length < 0 || !info_line.jmp_vector) { // jump back
                    buffer = Negative_num(2 - info_line.jmp_length);
                    binary_translate(buffer, res);
                } else if (info_line.jmp_vector && info_line.jmp_length == 0) {
                    buffer = "00000000";
                    binary_translate(buffer, res);
                } else { // jump forward
                    decimal_translate(info_line.jmp_length + 2, res);

                    buffer = "10010000";
                    for (int i = 0; i < 4; i++) {
                        binary_translate(buffer, res);
                    }
                }
            }
        }

        // r/m code
        if (res.is_op1_brackets || res.is_op2_brackets) {
            buffer = "01";
            if (!info_line.main_register.empty()) {
                buffer += res.registers_16_32[info_line.main_register];
            } else {
                buffer += "001";
            }
            buffer += "100";

            binary_translate(buffer, res); // add one byte to result code
        }

        // sib code
        if ((res.is_op1_brackets || res.is_op2_brackets) && info_line.reg_in_brackets.size() == 2) {
            buffer = "00"; // clear byte
            for (int i = info_line.reg_in_brackets.size() - 1; i >= 0; i--) {
                buffer += res.registers_16_32[info_line.reg_in_brackets[i]];
            }

            binary_translate(buffer, res); // add one byte to result code
        }

        info_line.command_code += info_line.number; // add info about number code

        // out info about line
        int code_size = 25;
        result << std::setw(3) << std::setfill(' ') << res.count << " "
               //<< std::setw(4) << std::setfill(' ') << info_line.dec_size << " "
               //<< std::setw(2) << std::setfill(' ') << info_line.size << " "
               << std::setw(4) << std::setfill(' ') << info_line.address << " "
               << info_line.command_code
               << std::setw(code_size - info_line.command_code.size()) << std::setfill(' ') << ""
               << " " << data << endl;
        if (info_line.error_info != "NULL") {
            result << info_line.error_info << endl;
        }

        // clear iteration parameters
        res.reg_count = 0;
        res.is_op1_brackets = false;
        res.is_op2_brackets = false;
        res.first_operand = 0;
        res.second_operand = 0;
        res.label.clear();
        res.local_address.clear();
    } else {
        result << std::setw(3) << std::setfill(' ') << res.count << endl;
    }

    res.count++;
}

// file view
void file_second_view(const string &name, data &res) {
    std::ofstream result("C:/Study/Olha_SP_Course_work/Result_file.lst");

    result << "Assembly translator STEP 1\n"
              "Developed by Olha Makarevych KV-03\n"
              "Filename: Result_file.lst\n\n"
              "1-line, 2-address, 3-size, 4-assembly operator\n\n";

    std::ifstream file(name);
    if (!file.is_open()) {
        cout << "Open error" << endl;
    } else {
        string line;
        while (getline(file, line)) { // read line from file
            Line_view(line, result, res);
        }
    }

    result << "\nSegment Size\n"
           << "CODE \t" << res.code_size << "\n"
           << "DATA \t" << res.data_size << "\n"
           << "\nName\tType\tAddress\tSegment\n\n";

    for (auto &i: res.labels) {
        if (i.second.type != "NULL") {
            result << i.first << "\t" << i.second.type << "\t"
                   << i.second.address << "\t" << i.second.segment << endl;
        }
    }

    result << "\n\t" << res.errors << " Errors" << endl;

    file.close();
    result.close();
}