#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

// Constante que representa el número de posiciones en la margarita. En este caso se toma a-z A-Z, siendo 52 posibles posiciones
#define POSICION_MAXIMA 52

// Constantes para el controlador PID
#define Kp 0.1
#define Ki 0.01
#define Kd 0.05

// Función para convertir la posición en un ángulo
double calcular_angulo(int posicion) {
    return (posicion - 1) * (360.0 / POSICION_MAXIMA);
}

// Busca evitar que la margarita de la vuelta más larga para llegar al ángulo deseado, ya que es 360 grados
double calculo_error_minimo(double angulo_deseado, double angulo_actual) {
    double error = angulo_deseado - angulo_actual;
    if (error > 180) {
        error -= 360;
    } else if (error < -180) {
        error += 360;
    }
    return error;
}

// Función para el controlador PID
double pid_control(double angulo_deseado, double angulo_actual, double *integral, double *error_previo) {
    double error = calculo_error_minimo(angulo_deseado, angulo_actual);
    *integral += error;
    double derivativo = error - *error_previo;
    *error_previo = error;
    return Kp * error + Ki * (*integral) + Kd * derivativo;
}

// Función para mapear una letra a una posición numérica
int letra_a_posicion(char letra) {
    if (isalpha(letra)) {
        if (islower(letra)) {
            return letra - 'a' + 1; // Para minúsculas
        } else {
            return letra - 'A' + 27; // Para mayúsculas
        }
    }
    return -1; // Retorna -1 si no es una letra válida
}

int main() {
    char letra_deseada;
    double perturbacion;
    int posicion_deseada;
    double angulo_deseado, angulo_actual = 0.0;
    double integral = 0.0;
    double error_previo = 0.0;
    double senal_de_control;
    FILE* archivo;

    // Inicializa el generador de números aleatorios
    srand(time(NULL));

    printf("Ingrese una letra (a-z, A-Z) para la posicion deseada de la margarita: ");
    scanf(" %c", &letra_deseada);

    // Mapea la letra a una posición en número
    posicion_deseada = letra_a_posicion(letra_deseada);

    // Se valida la entrada del usuario
    if (posicion_deseada < 1 || posicion_deseada > POSICION_MAXIMA) {
        printf("Error: La letra debe estar entre 'a' y 'z' o 'A' y 'Z'.\n");
        return 1;
    }

    // Se calcula el ángulo deseado correspondiente
    angulo_deseado = calcular_angulo(posicion_deseada);

    int iteracion = 0;
    while(1){

    // Simula el movimiento de la margarita hasta alcanzar la posición deseada
        while (1) {
            printf("Iteracion %d - Angulo actual: %.2f grados\n", iteracion, angulo_actual);

            if (angulo_actual == angulo_deseado) {
                break;
            }   

            senal_de_control = pid_control(angulo_deseado, angulo_actual, &integral, &error_previo);
            angulo_actual += senal_de_control;

            // Limita el ángulo para que esté en el rango de 0 a 360 grados
            if (angulo_actual >= 360.0) {
                angulo_actual -= 360.0;
            } else if (angulo_actual < 0.0) {
                angulo_actual += 360.0;
            }

            // Sale del bucle cuando el error es menor a 0.01
            if (fabs(angulo_deseado - angulo_actual) < 0.01) {
                break;
            }

            archivo = fopen("data.dat", "a");
    
            if (archivo == NULL) {
                printf("Error al abrir el archivo.\n");
                return 1;
            }

            // Carga los valores de salida en el archivo .dat
            fprintf(archivo, "%.2lf\n", angulo_actual);
            fclose(archivo);

            iteracion++;
        }

    printf("La posicion %d (letra %c) ha sido alcanzada con un angulo de %.2f grados.\n", posicion_deseada, letra_deseada, angulo_actual);
    
    printf("Ingrese un posible valor de perturbacion entre -100 y 100\n");
    scanf(" %lf", &perturbacion);
    
    if (perturbacion < -100 || perturbacion > 100) {
        printf("Error: La perturbacion debe ser un valor entre -100 y 100.\n");
        return 1;
    }

    angulo_actual += perturbacion;

    }
    
    return 0;
}