#ifndef OLHA_SP_COURSE_WORK_RESULT_INFO_H
#define OLHA_SP_COURSE_WORK_RESULT_INFO_H

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

struct user_label {
    int line_number; // line number
    int dec_size = 0;
    int count = 0;
    string type = "NULL";
    string address;
    string segment;
    string value; // save const value
    bool is_negative = false;

    bool used_in_first_pos = false;
};

struct info_line {
    int size = 0; // line data size
    int dec_size = 0; // line address in decimal
    string address; // line address
    string command; // command in this line
    string main_register; // register in this line
    vector<string> reg_in_brackets; // save registers in brackets
    int jmp_length = 0; // length between jump points (labels)
    bool jmp_vector = false; // false - back; true - front
    string num_value; // number in this line

    string command_code; // filling reserved data size by line elements codes

    string segment; // if segment changed
    string number; // data value
    int number_size = 0; // data size

    string mod; // jump mode

    string error_info = "NULL"; // changed if we have error
};

struct data {
    int count = 0; // line count
    string segment; // must be changed in new segment
    int Full_size = 0; // decimal number of file size

    int first_operand = 0; // size of first operand
    int second_operand = 0; // size of second operand
    string label; // user label in this line
    string status; //<label><mnem><op1><op2>

    int segment_use = 0; // check segment start/end
    bool is_op1_brackets = false;
    bool is_op2_brackets = false;
    int reg_count = 0; // count registers in brackets

    string local_address;
    string code_size; // end of the code segment address
    string data_size; // end of the data segment address

    int Address1 = 0; // Address4 * * Address1
    int Address2 = 0;
    int Address3 = 0;
    int Address4 = 0;

    std::map<string, user_label> labels; // all user labels <name, labels structure>
    std::map<int, info_line> line_info; // info about line <line number, info structure>

    int errors = 0;

    void ChangeAddress(int); // value is a size of operand

    char GetAddress(int);

    string GetLocalAddress();

    void SetAddressNull();

    void SetLocalAddress();

    void SetCodeSize(); // must be used in end of code segment
    void SetDataSize(); // must be used in end of data segment

    map<string, string> commands_code = {
            {"cmpsb", ""},
            {"inc", ""},
            {"mov", "10111000"},
            {"jbe", ""},
            {"dec", ""},
            {"bsr", ""},
            {"and", ""},
            {"or", ""},
            {"idiv", ""},
            {"jmp", ""},
    };
    map<string, string> segments = {
            {"es", "26"},
            {"cs", "2E"},
            {"ss", "36"},
            {"ds", "3E"},
            {"fs", "64"},
            {"gs", "65"},
    };
    map<string, string> registers_8 = {
            {"al", "000"}, //-> {"al", "000"),
            {"cl", "001"},
            {"dl", "010"},
            {"bl", "011"},
            {"ah", "100"},
            {"ch", "101"},
            {"dh", "110"},
            {"bh", "111"}
    };
    map<string, string> registers_16_32 = {
            {"ax", "000"},
            {"cx", "001"},
            {"dx", "010"},
            {"bx", "011"},
            {"sp", "100"},
            {"bp", "101"},
            {"si", "110"},
            {"di", "111"},
            {"eax", "000"},
            {"ecx", "001"},
            {"edx", "010"},
            {"ebx", "011"},
            {"esp", "100"},
            {"ebp", "101"},
            {"esi", "110"},
            {"edi", "111"}
    };
    map<string, std::vector<int>> registers_segment = {
            {"es", {0, 0, 0}},
            {"cs", {0, 0, 1}},
            {"ss", {0, 1, 0}},
            {"ds", {0, 1, 1}},
            {"fs", {1, 0, 0}},
            {"gs", {1, 0, 1}}
    };
private:
    void AddAddress(int, int &, int);

    int &NextAddress(int &);

    vector<char> address = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                 'A', 'B', 'C', 'D', 'E', 'F'};
};

#endif //OLHA_SP_COURSE_WORK_RESULT_INFO_H
