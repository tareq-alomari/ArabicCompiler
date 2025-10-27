// كود C مولد للغة العربية
// =======================

#include <stdio.h>
#include <stdlib.h>

int main() {
    int عمر = 0;
    int اسم = 0;
    int t0 = 0;
    int t1 = 0;
    int t2 = 0;
    int t3 = 0;
    int t4 = 0;
    int t5 = 0;
    int t6 = 0;
    char* str_0 = "أحمد";
    char* str_1 = "مرحباً بك ";
    char* str_2 = "عمرك هو: ";
    char* str_3 = "أنت بالغ";
    char* str_4 = "أنت قاصر";
    char* str_5 = "نهاية البرنامج";

    t0 = str_0;
    اسم = t0;
    t1 = 25;
    عمر = t1;
    printf("%s\n", str_1);
    t2 = اسم;
    printf("%d\n", t2);
    printf("%s\n", str_2);
    t3 = عمر;
    printf("%d\n", t3);
    t4 = عمر;
    t5 = 18;
    t6 = 0;
    /* CMP t4, t5 */
    // JG L2;
    goto L3;
    L2:
    t6 = 1;
    L3:
    if (!t6) goto L0;
    printf("%s\n", str_3);
    goto L1;
    L0:
    printf("%s\n", str_4);
    L1:
    printf("%s\n", str_5);
    return 0;
}
