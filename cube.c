#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define WIDTH 160
#define HEIGHT 44
#define CUBE_WIDTH 20
#define INCREMENT_SPEED 0.6
#define DISTANCE_FROM_CAM 100
#define K1 40.0

float A = 0, B = 0, C = 0; // Rotation angles about the axes
float zBuffer[WIDTH * HEIGHT];
char buffer[WIDTH * HEIGHT];

float calculateX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C)
        + j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C)
        + k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
    float x = calculateX(cubeX, cubeY, cubeZ);
    float y = calculateY(cubeX, cubeY, cubeZ);
    float z = calculateZ(cubeX, cubeY, cubeZ) + DISTANCE_FROM_CAM;
    float ooz = 1 / z;

    int xp = (int)(WIDTH / 2 - 2 * CUBE_WIDTH + K1 * ooz * x * 2);
    int yp = (int)(HEIGHT / 2 + K1 * ooz * y);

    int idx = xp + yp * WIDTH;
    if (idx >= 0 && idx < WIDTH * HEIGHT) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main() {
    printf("\x1b[2J"); // Clear the screen
    while (1) {
        memset(buffer, ' ', WIDTH * HEIGHT);
        memset(zBuffer, 0, sizeof(zBuffer));
        for (float cubeX = -CUBE_WIDTH; cubeX < CUBE_WIDTH; cubeX += INCREMENT_SPEED) {
            for (float cubeY = -CUBE_WIDTH; cubeY < CUBE_WIDTH; cubeY += INCREMENT_SPEED) {
                calculateForSurface(cubeX, cubeY, -CUBE_WIDTH, '.');
                calculateForSurface(CUBE_WIDTH, cubeY, cubeX, '$');
                calculateForSurface(-CUBE_WIDTH, cubeY, -cubeX, '~');
                calculateForSurface(-cubeX, cubeY, CUBE_WIDTH, '#');
                calculateForSurface(cubeX, -CUBE_WIDTH, -cubeY, ';');
                calculateForSurface(cubeX, CUBE_WIDTH, cubeY, '+');
            }
        }
        printf("\x1b[H"); // Move cursor to home position
        for (int k = 0; k < WIDTH * HEIGHT; k++) {
            putchar(k % WIDTH ? buffer[k] : '\n');
        }
        A += 0.005;
        B += 0.005;
        usleep(1000);
    }
    return 0;
}

