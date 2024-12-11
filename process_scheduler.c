#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10

void calculateFCFS(int processes[MAX_PROCESSES][5], int n);
void calculateSJF(int processes[MAX_PROCESSES][5], int n);
void calculatePriority(int processes[MAX_PROCESSES][5], int n);

int compareCompletionTime(const void *a, const void *b) {
    int *processA = (int *)a;
    int *processB = (int *)b;
    return processA[4] - processB[4];  // Completion time'a göre karşılaştır
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Scheduling Algorithm> <Input File>\n", argv[0]);
        return 1;
    }

    char *algorithm = argv[1];
    char *filename = argv[2];
    int processes[MAX_PROCESSES][5]; // 5. sütun tamamlanma zamanı için
    int n = 0; // Process sayısı

    // Dosyadan process'leri okuma
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    while (fscanf(file, "%d %d %d %d", &processes[n][0], &processes[n][1], &processes[n][2], &processes[n][3]) != EOF) {
        n++;
        if (n >= MAX_PROCESSES) {
            break;
        }
    }
    fclose(file);

    if (strcmp(algorithm, "FCFS") == 0) {
        calculateFCFS(processes, n);
    } else if (strcmp(algorithm, "SJF") == 0) {
        calculateSJF(processes, n);
    } else if (strcmp(algorithm, "PRIO") == 0) {
        calculatePriority(processes, n);
    } else {
        printf("Invalid scheduling algorithm. Use 'FCFS', 'SJF', or 'PRIO'.\n");
        return 1;
    }

    return 0;
}

void calculateFCFS(int processes[MAX_PROCESSES][5], int n) {
    float totalTurnaroundTime = 0, totalWaitingTime = 0;

    processes[0][4] = processes[0][1] + processes[0][3]; // İlk process'in tamamlanma zamanı

    for (int i = 1; i < n; i++) {
        processes[i][4] = processes[i - 1][4] + processes[i][3];
    }

    // Tamamlanma zamanına göre sırala
    qsort(processes, n, sizeof(processes[0]), compareCompletionTime);

    for (int i = 0; i < n; i++) {
        int turnaroundTime = processes[i][4] - processes[i][1];
        int waitingTime = turnaroundTime - processes[i][3];
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

        printf("Process %d Completion Time: %d\n", processes[i][0], processes[i][4]);
    }

    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime / n);
    printf("Average Waiting Time: %.2f\n", totalWaitingTime / n);
}

void calculateSJF(int processes[MAX_PROCESSES][5], int n) {
    int completed[MAX_PROCESSES] = {0};
    int time = 0;
    float totalTurnaroundTime = 0, totalWaitingTime = 0;

    for (int count = 0; count < n; count++) {
        int shortestIndex = -1;
        int minBurstTime = 99999;

        for (int j = 0; j < n; j++) {
            if (processes[j][1] <= time && !completed[j] && processes[j][3] < minBurstTime) {
                minBurstTime = processes[j][3];
                shortestIndex = j;
            }
        }

        if (shortestIndex != -1) {
            time += processes[shortestIndex][3];
            processes[shortestIndex][4] = time;
            completed[shortestIndex] = 1;
        } else {
            time++;
            count--;
        }
    }

    // Tamamlanma zamanına göre sırala
    qsort(processes, n, sizeof(processes[0]), compareCompletionTime);

    for (int i = 0; i < n; i++) {
        int turnaroundTime = processes[i][4] - processes[i][1];
        int waitingTime = turnaroundTime - processes[i][3];
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

        printf("Process %d Completion Time: %d\n", processes[i][0], processes[i][4]);
    }

    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime / n);
    printf("Average Waiting Time: %.2f\n", totalWaitingTime / n);
}

void calculatePriority(int processes[MAX_PROCESSES][5], int n) {
    int remainingBurstTime[MAX_PROCESSES]; // Kalan burst time'ları tutmak için
    int completed[MAX_PROCESSES] = {0};    // Process tamamlama durumları
    int time = 0;                          // Geçen süre
    int completedProcesses = 0;            // Tamamlanmış process sayısı
    float totalTurnaroundTime = 0, totalWaitingTime = 0;

    // Başlangıçta tüm process'lerin kalan burst time'larını orijinal burst time'a eşitle
    for (int i = 0; i < n; i++) {
        remainingBurstTime[i] = processes[i][3];
    }

    while (completedProcesses < n) {
        int highestPriorityIndex = -1;
        int minPriority = 99999;

        // O an çalışabilecek en yüksek öncelikli process'i seç
        for (int j = 0; j < n; j++) {
            if (processes[j][1] <= time && !completed[j] && processes[j][2] < minPriority && remainingBurstTime[j] > 0) {
                minPriority = processes[j][2];
                highestPriorityIndex = j;
            }
        }

        // Çalışacak bir process bulunduysa
        if (highestPriorityIndex != -1) {
            remainingBurstTime[highestPriorityIndex]--; // Process'in kalan burst time'ını azalt
            time++; // Zamanı bir birim arttır

            // Eğer process tamamlandıysa
            if (remainingBurstTime[highestPriorityIndex] == 0) {
                completed[highestPriorityIndex] = 1;
                completedProcesses++;
                processes[highestPriorityIndex][4] = time; // Tamamlanma zamanı
            }
        } else {
            time++; // Zamanı ilerlet
        }
    }

    // Tamamlanma zamanına göre sırala
    qsort(processes, n, sizeof(processes[0]), compareCompletionTime);

    for (int i = 0; i < n; i++) {
        int turnaroundTime = processes[i][4] - processes[i][1];
        int waitingTime = turnaroundTime - processes[i][3];
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

        printf("Process %d Completion Time: %d\n", processes[i][0], processes[i][4]);
    }

    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime / n);
    printf("Average Waiting Time: %.2f\n", totalWaitingTime / n);
}

