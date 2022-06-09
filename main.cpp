//#include "parsing.h"
#include "first_view.h"
#include "second_view.h"
#include "result_info.h"

using namespace std;

int main() {
    data res;
    file_first_view("C:/Study/Olha_SP_Course_work/test2.asm", res);
    file_second_view("C:/Study/Olha_SP_Course_work/test2.asm", res);

    return 0;
}