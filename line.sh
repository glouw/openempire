#!/bin/bash

# COUNTS THE NUMBER OF LINES IN THE PROJECT.

wc src/*.[ch] --lines | sort -n
