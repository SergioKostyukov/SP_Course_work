//#include "parsing.h"
#include "first_view.h"
#include "second_view.h"
#include "result_info.h"

int main() {
    data res;
    file_first_view("C:/Study/Olha_SP_Course_work/test1.asm", res);
    file_second_view("C:/Study/Olha_SP_Course_work/test1.asm", res);

    return 0;
}