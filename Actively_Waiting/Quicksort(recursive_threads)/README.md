# QuickSort

This program implements quicksort algorithm recursively and multithreaded.<br>

Each thread creates two new threads to sort half the array it has been given waiting them for them to finish before it sorts its part.

In order to run the user should provide as arguments seperated number that will get stored in an array and will be sorted.
```bash
./quicksort
```

In each step the program prints the array it has to sort and the pivot which is always the biggest element

