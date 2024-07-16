#include <stdio.h>#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>

#define WIDTH 160
#define HEIGHT 44
#define CUBE_WIDTH 20
#define INCREMENT_SPEED 0.6
#define DISTANCE_FROM_CAM 100
#define K1 40.0
#define ROTATION_SPEED 0.005
#define SLEEP_DURATION 1000

typedef struct {
    double x, y, z;
} Point3D;

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

double rotationX = 0, rotationY = 0, rotationZ = 0;
double zBuffer[WIDTH * HEIGHT];
char buffer[WIDTH * HEIGHT];

Point3D calculate3DPoint(double i, double j, double k) {
    Point3D point;
    point.x = j * sin(rotationX) * sin(rotationY) * cos(rotationZ) - k * cos(rotationX) * sin(rotationY) * cos(rotationZ)
        + j * cos(rotationX) * sin(rotationZ) + k * sin(rotationX) * sin(rotationZ) + i * cos(rotationY) * cos(rotationZ);
    point.y = j * cos(rotationX) * cos(rotationZ) + k * sin(rotationX) * cos(rotationZ) - j * sin(rotationX) * sin(rotationY) * sin(rotationZ)
        + k * cos(rotationX) * sin(rotationY) * sin(rotationZ) - i * cos(rotationY) * sin(rotationZ);
    point.z = k * cos(rotationX) * cos(rotationY) - j * sin(rotationX) * cos(rotationY) + i * sin(rotationY);
    return point;
}

void calculateForSurface(double cubeX, double cubeY, double cubeZ, char ch) {
    Point3D point = calculate3DPoint(cubeX, cubeY, cubeZ);
    double z = point.z + DISTANCE_FROM_CAM;
    double ooz = 1 / z;
    int xp = (int)(WIDTH / 2 - 2 * CUBE_WIDTH + K1 * ooz * point.x * 2);
    int yp = (int)(HEIGHT / 2 + K1 * ooz * point.y);
    int idx = xp + yp * WIDTH;
    if (idx >= 0 && idx < WIDTH * HEIGHT) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

void renderFrame() {
    memset(buffer, ' ', WIDTH * HEIGHT);
    memset(zBuffer, 0, sizeof(zBuffer));
    for (double cubeX = -CUBE_WIDTH; cubeX < CUBE_WIDTH; cubeX += INCREMENT_SPEED) {
        for (double cubeY = -CUBE_WIDTH; cubeY < CUBE_WIDTH; cubeY += INCREMENT_SPEED) {
            calculateForSurface(cubeX, cubeY, -CUBE_WIDTH, '.');
            calculateForSurface(CUBE_WIDTH, cubeY, cubeX, '$');
            calculateForSurface(-CUBE_WIDTH, cubeY, -cubeX, '~');
            calculateForSurface(-cubeX, cubeY, CUBE_WIDTH, '#');
            calculateForSurface(cubeX, -CUBE_WIDTH, -cubeY, ';');
            calculateForSurface(cubeX, CUBE_WIDTH, cubeY, '+');
        }
    }
}

void displayFrame() {
    printf("\x1b[H"); // Move cursor to home position
    for (int k = 0; k < WIDTH * HEIGHT; k++) {
        putchar(k % WIDTH ? buffer[k] : '\n');
    }
}

int main() {
    signal(SIGINT, handle_sigint);
    printf("\x1b[2J"); // Clear the screen

    while (keep_running) {
        renderFrame();
        displayFrame();
        rotationX += ROTATION_SPEED;
        rotationY += ROTATION_SPEED;
        usleep(SLEEP_DURATION);
    }

    printf("\x1b[2J\x1b[H"); // Clear screen and reset cursor
    printf("Program terminated.\n");
    return 0;
}
