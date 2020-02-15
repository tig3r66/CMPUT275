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
    int pivot = array[high].dist;
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
