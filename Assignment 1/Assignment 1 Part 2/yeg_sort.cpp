//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

#include "include/yeg_sort.h"


/*
    Description: implementation of insertion sort.

    Arguments:
        array[] (int): pointer to an array.
        n (int): the number of items in the array.
    Notes:
        Need to change this function so it sorts based on the RestDist struct.
*/
void insertionSort(RestDist array[], int n) {
    for (int i = 1; i < n; i++) {
        for (int j = i-1; j >= 0 && array[j].dist > array[j+1].dist; j--) {
            custom_swap(array[j], array[j+1]);
        }
    }
}


/*
    Description: implementation of quick sort.

    Arguments:
        array (RestDist[]): the array containing restaurants to sort.
        low (int): the smallest index of the array.
        high (int): the largest index of the array.
*/
void quickSort(RestDist array[], int low, int high) {
    if (low < high) {
        int pi = pivot(array, low, high);
        // pivot around pi
        quickSort(array, low, pi - 1);
        quickSort(array, pi + 1, high);
    }
}


/*
    Description: helper partitioning function for quickSort(). Uses the last
    element in the array as a pivot.

    Arguments:
        array (RestDist[]): the array containing restaurants to sort.
        low (int): the smallest index of the array.
        high (int): the largest index of the array.
*/
int pivot(RestDist array[], int low, int high) {
    uint16_t pivot = array[high].dist;
    // sorting on pivot
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (array[j].dist <= pivot) {
            i++;
            custom_swap(array[i], array[j]);
        }
    }
    custom_swap(array[i + 1], array[high]);
    return (i + 1);
}


/*
    Description: times quicksort and insertion sort algorithms as they sort the
    restaurants based on Manhattan distance from the cursor position.

    Arguments:
        sortMode (uint8_t): the selected sorting algorithm.
        rating (uint8_t): the restaurant rating filter.
*/
void sortTimer(uint8_t sortMode, uint8_t rating) {
    readRestData(rating);
    if (sortMode == 0) {
        timeQSort(sortMode, rating);
    } else if (sortMode == 1) {
        timeISort(sortMode, rating);
    } else if (sortMode == 2) {
        timeQSort(sortMode, rating);
        // reading again for fair comparison
        readRestData(rating);
        timeISort(sortMode, rating);
    }
}


/*
    Description: helper function that prints the running time of quicksort to
    the serial monitor.

    Arguments:
        sortMode (uint8_t): the selected sorting algorithm.
        rating (uint8_t): the restaurant rating filter.
*/
void timeQSort(uint8_t sortMode, uint8_t rating) {
    uint16_t start = millis();
    quickSort(cache.REST_DIST, 0, cache.READ_SIZE-1);
    uint16_t end  = millis();
    Serial.print("Quicksort running time: ");
    Serial.print(end - start);
    Serial.println(" ms");
}


/*
    Description: helper function that prints the running time of insertion sort
    to the serial monitor.

    Arguments:
        sortMode (uint8_t): the selected sorting algorithm.
        rating (uint8_t): the restaurant rating filter.
*/
void timeISort(uint8_t sortMode, uint8_t rating) {
    uint16_t start = millis();
    insertionSort(cache.REST_DIST, cache.READ_SIZE);
    uint16_t end  = millis();
    Serial.print("Insertion sort running time: ");
    Serial.print(end - start);
    Serial.println(" ms");
}
