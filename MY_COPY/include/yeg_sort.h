//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

/*
    Implements of insertion sort and quicksort along with helper functions.
    Please see yeg_sort.cpp for full function documentation.
*/

#ifndef _SORTING_H_
#define _SORTING_H_

#include <Arduino.h>
#include "a1part2.h"

// =========================== FUNCTION DECLARATIONS ===========================
/*
    Description: implementation of insertion sort.
*/
void insertionSort(RestDist array[], int n);

/*
    Description: implementation of quick sort.
*/
void quickSort(RestDist array[], int low, int high);

/*
    Description: helper partitioning function for quickSort(). Uses the last
    element in the array as a pivot.
*/
int pivot(RestDist array[], int low, int high);

/*
    Description: times quicksort and insertion sort algorithms as they sort the
    restaurants based on Manhattan distance from the cursor position.
*/
void sortTimer(uint8_t sortMode, uint8_t rating);

/*
    Description: helper function that prints the running time of quicksort to
    the serial monitor.
*/
void timeQSort(uint8_t sortMode, uint8_t rating);

/*
    Description: helper function that prints the running time of insertion sort
    to the serial monitor.
*/
void timeISort(uint8_t sortMode, uint8_t rating);


// =============================== TEMPLATE CODE ===============================
/*
    Description: swaps two items x and y.

    Arguments:
        x (T&): the first item to swap.
        y (T&): the second item to swap.
*/
template <typename T>
void custom_swap(T &x, T &y) {
    T temp = x;
    x = y;
    y = temp;
}

#endif
