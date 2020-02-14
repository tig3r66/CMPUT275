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
