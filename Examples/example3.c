// كود C مولد للغة العربية
// =======================

#include <stdio.h>
#include <stdlib.h>

int main() {
    int المجموع = 0;
    int العداد = 0;
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

    t0 = 0;
    العداد = t0;
    t1 = 0;
    المجموع = t1;
    t2 = 0;
    المجموع = t2;
    t3 = 1;
    العداد = t3;
    L0:
    t4 = العداد;
    t5 = 10;
    t6 = 0;
    /* CMP t4, t5 */
    /* JLE label */
    goto L3;
    L2:
    t6 = 1;
    L3:
    if (!t6) goto L1;
    t7 = المجموع;
    t8 = العداد;
    t9 = t7 + t8;
    المجموع = t9;
    t10 = العداد;
    t11 = 1;
    t12 = t10 + t11;
    العداد = t12;
    goto L0;
    L1:
    printf("%s", str_0);
    t13 = المجموع;
    printf("%d\n", t13);
    return 0;
}
