//
//  main.c
//  KnapsackProblem
//
//  Created by 山崎 慎太郎 on 2014/11/25.
//  Copyright (c) 2014年 山崎 慎太郎. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/** 個体数 */
#define INDIVIDUAL_SIZE 200
/** 最終世代 */
#define GENERATION_NUMBER 500
/** 世代ギャップ */
#define GENERATION_GAP 0.5
/** 突然変異確率 */
#define MUTATION_RATE 0.001

/** 物体数 */
#define OBJECT_SIZE 100
/** ナップザックの最大重量 */
#define WEIGHT_MAX 200

/** 個体群定義 */
typedef struct {
    unsigned int code;
    int fit;
} INDIVIDUAL;


/** 物体[重量][価値] */
int objects[OBJECT_SIZE][2];
/** 個体群 */
INDIVIDUAL population[INDIVIDUAL_SIZE];
/** 適応度の合計 */
int fitnessSum;


/** 物体の生成 */
void createObject() {
    int i;
    
    printf("objects\n");
    for (i = 0; i < OBJECT_SIZE; i++) {
        objects[i][0] = rand() % 10 + 1;
        objects[i][1] = rand() % 50;
        
        printf("%3d | %2d | %3d\n", i, objects[i][0], objects[i][1]);
    }
    printf("\n");
}

/** 個体群の生成 */
void createPopulation() {
    int i, j;
    
    printf("population\n");
    for (i = 0; i < INDIVIDUAL_SIZE; i++) {
        population[i].code = 0;
        printf("%3d | ", i);
        for (j = 0; j < OBJECT_SIZE; j++) {
            int b = rand() % 2;
            population[i].code = population[i].code | b << j;
            population[i].fit = 0;
            printf("%d", b);
        }
        printf("\n");
    }
    printf("\n");
}

/** 適応度関数 */
int fitnessFunction(int weight, int value) {
    int fitness = 0;
    
    if (weight < WEIGHT_MAX) {
        fitness = value;
    } else {
        fitness = 0;
    }
    
    return fitness;
}

/** 適応度計算 */
void calcFitness() {
    int i, j;
    int weight, value;
    
    fitnessSum = 0;
    
    for (i = 0; i < INDIVIDUAL_SIZE; i++) {
        weight = 0;
        value = 0;
        for (j = 0; j < OBJECT_SIZE; j++) {
            if ((population[i].code >> j) & 1) {
                weight += objects[j][0];
                value += objects[j][1];
            }
        }
        population[i].fit = fitnessFunction(weight, value);
        fitnessSum += population[i].fit;
    }
}

/** ルーレット選択 */
void rouletteSelect() {
    int i, j;
    int s, f;
    INDIVIDUAL selected[INDIVIDUAL_SIZE];
    
    if (fitnessSum > 0) {
        for (j = 0; j < INDIVIDUAL_SIZE; j++) {
            s = rand() % fitnessSum;
            f = 0;
            for (i = 0; i < INDIVIDUAL_SIZE; i++) {
                f += population[i].fit;
                if (f >= s) {
                    selected[j] = population[i];
                    break;
                }
            }
        }
        
        for (i = 0; i < INDIVIDUAL_SIZE; i++) {
            population[i] = selected[i];
        }

    }
}

/** 交叉 */
void crossover() {
    int i, j;
    int partner = 0;
    
    INDIVIDUAL children[INDIVIDUAL_SIZE * 2];
    
    for (i = 0; i < INDIVIDUAL_SIZE; i++) {
        do {
            partner = rand() % INDIVIDUAL_SIZE;
        } while (i == partner);
        
        INDIVIDUAL child1, child2;
        
        child1 = population[i];
        child2 = population[partner];
        
        if (rand() % 2) {
            // 1点交叉
            int point = rand() % OBJECT_SIZE;
            unsigned int mask = 0;
            for (j = 0; j < point; j++) {
                mask = (mask << j) | 1;
            }
            unsigned int a, b;
            a = child1.code & mask;
            b = child2.code & mask;
            child1.code = (child1.code & ~mask) | b;
            child2.code = (child2.code & ~mask) | a;
        }
        children[i] = child1;
        children[i + 1] = child2;
    }
    
    for (i = 0; i < (int)(INDIVIDUAL_SIZE * GENERATION_GAP); i++) {
        population[rand() % INDIVIDUAL_SIZE] = children[rand() % (INDIVIDUAL_SIZE * 2)];
    }
}

/** 突然変異 */
void mutation() {
    unsigned int mask = 0;
    int i, j;
    
    for (i = 0; i < INDIVIDUAL_SIZE; i++) {
        for (j = 0; j < OBJECT_SIZE; j++) {
            if (rand() % 10000 < MUTATION_RATE * 10000) {
                mask = mask | 1;
            }
            mask = mask << 1;
        }
        population[i].code = population[i].code ^ mask;
    }
}

int main(int argc, const char * argv[]) {
    int count, i, j;
    INDIVIDUAL maxfit;
    int maxfitindex = 0;
    float fitsum;
    
    INDIVIDUAL totalmax;
    int totalmaxindex = 0;
    totalmax.fit = 0;
    
    // 乱数設定
    srand((unsigned) time(NULL));
    
    /** 実行結果をファイルに出力 */
    FILE *datafile;
    
    datafile = fopen("data.csv", "w");
    if(datafile == NULL){
        printf( "Cannot open file\n");
        return 1;
    }

    
    createObject();
    createPopulation();

    for (count = 0; count < GENERATION_NUMBER; count++) {
        printf("%d\n", count);
        fprintf(datafile, "%d,", count);
        maxfit.code = 0;
        maxfit.fit = 0;
        fitsum = 0.0;
        calcFitness();
        
        for (i = 0; i < INDIVIDUAL_SIZE; i++) {
            printf("%3d | ", i);
            for (j = 0; j < OBJECT_SIZE; j++) {
                printf("%d", (population[i].code >> (OBJECT_SIZE - j)) & 1);
            }
            printf("| %3d\n", population[i].fit);
            if (maxfit.fit <= population[i].fit) {
                maxfit = population[i];
                maxfitindex = i;
            }
            fitsum += population[i].fit;
        }
        if (totalmax.fit <= maxfit.fit) {
            totalmax = maxfit;
            totalmaxindex = count;
        }
        
        printf("\n");

        fprintf(datafile, "%d,", maxfitindex);
        for (j = 0; j < OBJECT_SIZE; j++) {
            fprintf(datafile, "%d", (maxfit.code >> (OBJECT_SIZE - j)) & 1);
        }
        fprintf(datafile, ",");
        fprintf(datafile, "%d,", maxfit.fit);
        fprintf(datafile, "%.3f\n", fitsum / INDIVIDUAL_SIZE);
        
        rouletteSelect();
        crossover();
        mutation();
        
    }
    
    
    printf("%d\n", count);
    fprintf(datafile, "%d,", count);
    maxfit.code = 0;
    maxfit.fit = 0;
    fitsum = 0.0;
    calcFitness();
    
    for (i = 0; i < INDIVIDUAL_SIZE; i++) {
        printf("%3d | ", i);
        for (j = 0; j < OBJECT_SIZE; j++) {
            printf("%d", (population[i].code >> (OBJECT_SIZE - j)) & 1);
        }
        printf("| %3d\n", population[i].fit);
        if (maxfit.fit <= population[i].fit) {
            maxfit = population[i];
            maxfitindex = i;
        }
        fitsum += population[i].fit;
    }
    
    printf("total max : %d ", totalmaxindex);
    for (j = 0; j < OBJECT_SIZE; j++) {
        printf("%d", (totalmax.code >> (OBJECT_SIZE - j)) & 1);
    }
    printf(" %d\n", totalmax.fit);
    
    fclose(datafile);
    
    return 0;
}
