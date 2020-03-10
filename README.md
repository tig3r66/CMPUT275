# CMPUT 275
## Introduction to Tangible Computing II, University of Alberta

**COURSE DESCRIPTION**

CMPUT 274/275 is an experimental 2-course pilot introduction to computer science at the University of Alberta. It integrates the introductory computing courses (CMPUT 174/175) with the second-year introductory algorithms course (CMPUT 204) and the second year introductory systems course (CMPUT 201).

The course uses a problem-based approach to motivate the concepts and illustrate their application. It will be using the Arduino concrete-computing platform so that students will both become familiar with the typical screen-keyboard-mouse style of computing, but also the kind of embedded computing that is behind the scenes in the many devices that surround us. Delivery is hands on, with the classes taking place in the lab environment.

**REPOSITORY DESCRIPTION**

This repository contains projects and assignments from a computing science course at the University of Alberta called Introduction to Tangible Computing II. Please take all uploaded code with a grain of salt as errata may be present and code may not be optimized.

# Project Descriptions

## Assignment 1: Restaurant Finder
This project implements a scrollable map of Edmonton on the Arduino using an Adafruit 3.5" TFT LCD display (touchscreen) and joystick. Users can tap the screen to view restaurants based on their mapped ratings. They may also find a restaurant based on Manhattan distance from the cursor (a caching system, quicksort, and/or insertion sort used to sort the restaurants). This assignment received a grade of 100% for both part 1 and part 2.

## Final Project: EEG Visualizer
This project visualizes live-time mock EEG data alongside a fast Fourier fransform plot in an application run by PyQt5. Here, we implement the radix-2 decimation-in-time algorithm and the chirp Z-transform (aka Bluestein’s FFT algorithm).

## License
This repository is licensed under the MIT license. See [LICENSE](./LICENSE) for further detail.
