// كود C مولد للغة العربية
// =======================

#include <stdio.h>
#include <stdlib.h>

int main() {
    int س = 0;
    int درجة = 0;
    int ص = 0;
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
    int t16 = 0;
    int t17 = 0;
    int t18 = 0;
    int t19 = 0;
    int t20 = 0;
    int t21 = 0;
    int t22 = 0;
    int t23 = 0;
    int t24 = 0;
    int t25 = 0;
    int t26 = 0;
    int t27 = 0;
    int t28 = 0;
    char* str_0 = "امتياز";
    char* str_1 = "جيد جداً";
    char* str_2 = "جيد";
    char* str_3 = "مقبول";
    char* str_4 = "راسب";
    char* str_5 = "الشرط الأول صحيح";
    char* str_6 = "الشرط الثاني صحيح";

    t0 = 85;
    درجة = t0;
    t1 = 10;
    س = t1;
    t2 = 20;
    ص = t2;
    t3 = درجة;
    t4 = 90;
    t5 = 0;
    /* CMP t3, t4 */
    // JGE L2;
    goto L3;
    L2:
    t5 = 1;
    L3:
    if (!t5) goto L0;
    printf("%s\n", str_0);
    goto L1;
    L0:
    t6 = درجة;
    t7 = 80;
    t8 = 0;
    /* CMP t6, t7 */
    // JGE L6;
    goto L7;
    L6:
    t8 = 1;
    L7:
    if (!t8) goto L4;
    printf("%s\n", str_1);
    goto L5;
    L4:
    t9 = درجة;
    t10 = 70;
    t11 = 0;
    /* CMP t9, t10 */
    // JGE L10;
    goto L11;
    L10:
    t11 = 1;
    L11:
    if (!t11) goto L8;
    printf("%s\n", str_2);
    goto L9;
    L8:
    t12 = درجة;
    t13 = 60;
    t14 = 0;
    /* CMP t12, t13 */
    // JGE L14;
    goto L15;
    L14:
    t14 = 1;
    L15:
    if (!t14) goto L12;
    printf("%s\n", str_3);
    goto L13;
    L12:
    printf("%s\n", str_4);
    L13:
    L9:
    L5:
    L1:
    t15 = س;
    t16 = 5;
    t17 = ص;
    // AND t18, t16, t17;
    t19 = 0;
    /* CMP t15, t18 */
    // JG L18;
    goto L19;
    L18:
    t19 = 1;
    L19:
    t20 = 30;
    t21 = 0;
    /* CMP t19, t20 */
    // JL L20;
    goto L21;
    L20:
    t21 = 1;
    L21:
    if (!t21) goto L16;
    printf("%s\n", str_5);
    L16:
    t22 = س;
    t23 = 10;
    t24 = ص;
    // OR t25, t23, t24;
    t26 = 0;
    /* CMP t22, t25 */
    /* JE L24 - مقارنة للمساواة */
    goto L25;
    L24:
    t26 = 1;
    L25:
    t27 = 15;
    t28 = 0;
    /* CMP t26, t27 */
    /* JE L26 - مقارنة للمساواة */
    goto L27;
    L26:
    t28 = 1;
    L27:
    if (!t28) goto L22;
    printf("%s\n", str_6);
    L22:
    return 0;
}
