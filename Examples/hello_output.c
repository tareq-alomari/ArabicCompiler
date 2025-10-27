// كود C مولد للغة العربية
// =======================

#include <stdio.h>
#include <stdlib.h>

int main() {
    int عمر = 0;
    char* اسم = NULL;
    int t0 = 0;
    int t1 = 0;
    int t2 = 0;
    int t3 = 0;
    int t4 = 0;
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
    printf("%s\n", اسم);
    printf("%s\n", str_2);
    printf("%d\n", عمر);
    t2 = عمر;
    t3 = 18;
    t4 = 0;
    /* CMP t2, t3 */
    // JG L2;
    goto L3;
    L2:
    t4 = 1;
    L3:
    if (!t4) goto L0;
    printf("%s\n", str_3);
    goto L1;
    L0:
    printf("%s\n", str_4);
    L1:
    printf("%s\n", str_5);
    return 0;
}
