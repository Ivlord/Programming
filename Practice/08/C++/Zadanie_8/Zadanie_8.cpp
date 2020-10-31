#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale>

int Zadanie_8()
{
    setlocale(LC_ALL, "Russian");
    double a, b, c;
    char d, sp1, sp2;
    while (true)
    {
        scanf("%lf%c%c%c%lf", &a, &sp1, &d, &sp1, &b); // 45 * 76

        while (getchar() != '\n');
        switch (d)
        {
        case '+': c = a + b; break;
        case '-': c = a - b; break;
        case '*': c = a * b; break;
        case '/': c = a / b; break;
        default:
            printf("Операция не поддерживается\n");
            continue;
        }
        printf("%lf\n", c);
        break;
    }
    return 0;
}

int main() {
    Zadanie_8();
    return 0;
}