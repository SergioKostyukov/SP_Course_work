#include "result_info.h"

void data::ChangeAddress(const int value) {
    AddAddress(value, Address1, 1);
} // value is a size of operand

char data::GetAddress(const int i) {
    return address[i];
}

string data::GetLocalAddress() {
    local_address.clear();
    SetLocalAddress();
    return local_address;
}

void data::SetAddressNull() {
    Address1 = 0;
    Address2 = 0;
    Address3 = 0;
    Address4 = 0;
}

void data::SetLocalAddress() {
    local_address.push_back(GetAddress(Address4));
    local_address.push_back(GetAddress(Address3));
    local_address.push_back(GetAddress(Address2));
    local_address.push_back(GetAddress(Address1));
}

void data::SetCodeSize() {
    code_size.push_back(GetAddress(Address4));
    code_size.push_back(GetAddress(Address3));
    code_size.push_back(GetAddress(Address2));
    code_size.push_back(GetAddress(Address1));
} // must be used in end of code segment
void data::SetDataSize() {
    data_size.push_back(GetAddress(Address4));
    data_size.push_back(GetAddress(Address3));
    data_size.push_back(GetAddress(Address2));
    data_size.push_back(GetAddress(Address1));
} // must be used in end of data segment

void data::AddAddress(int value, int &operand, int i) {
    int buf = operand + value;
    if (buf >= address.size()) {
        operand = buf - static_cast<int>(address.size());
        AddAddress(1, NextAddress(i), i);
    } else {
        operand += value;
    }
}

int &data::NextAddress(int &i) {
    i++;
    if (i == 2) {
        return Address2;
    } else if (i == 3) {
        return Address3;
    } else {
        return Address4;
    }
}
