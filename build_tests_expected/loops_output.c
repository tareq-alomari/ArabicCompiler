// كود C مولد للغة العربية
// =======================

#include <stdio.h>
#include <stdlib.h>

int main() {
    int عداد = 0;
    int عدد = 0;
    int t0 = 0;
    int t1 = 0;
    int t2 = 0;
    int t3 = 0;
    int t4 = 0;
    int t5 = 0;
    int t6 = 0;
    int t7 = 0;
    int t8 = 0;
    int t9 = 0;
    int t10 = 0;
    int t11 = 0;
    int t12 = 0;
    int t13 = 0;
    int t14 = 0;
    int t15 = 0;
    char* str_0 = "عداد = ";
    char* str_1 = "العدد: ";
    char* str_2 = "انتهت الحلقات";

    t0 = 1;
    عداد = t0;
    t1 = 1;
    عدد = t1;
    L0:
    t2 = عداد;
    t3 = 5;
    t4 = 0;
    /* CMP t2, t3 */
    // JLE L2;
    goto L3;
    L2:
    t4 = 1;
    L3:
    if (!t4) goto L1;
    printf("%s\n", str_0);
    t5 = عداد;
    printf("%d\n", t5);
    t6 = عداد;
    t7 = 1;
    t8 = t6 + t7;
    عداد = t8;
    goto L0;
    L1:
    L4:
    printf("%s\n", str_1);
    t9 = عدد;
    printf("%d\n", t9);
    t10 = عدد;
    t11 = 1;
    t12 = t10 + t11;
    عدد = t12;
    t13 = عدد;
    t14 = 3;
    t15 = 0;
    /* CMP t13, t14 */
    // JG L5;
    goto L6;
    L5:
    t15 = 1;
    L6:
    if (!t15) goto L4;
    printf("%s\n", str_2);
    return 0;
}
