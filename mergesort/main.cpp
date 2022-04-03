#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>

using std::cout, std::endl, std::thread;

const int array_size = 1000;
int max_threads = 8;

int find_location(const int *array, int size, int nr){
    // Returns the index of where the element would be in the array
    for (int i = 0; i < size; i++){
        if (nr <= array[i]) { return i; }
    }

    return size;
}

void merge(int *array, int size){
    // Merge an array by sorting its halves into one fully sorted array
    int *sorted = (int *)malloc(sizeof(int) * size);
    int location;

    //sort first half
    for (unsigned int i = 0; i < size/2; i++){
        location = find_location(&array[size / 2], size - (size / 2), array[i]);
        sorted[location+i] = array[i];
    }

    //sort second half
    for (unsigned int  i = size/2; i < size; i++){
        location = find_location(&array[0], size / 2, array[i]);

        // Check if array_size already exists in sorted array. If so, move it up until there is space.
        for (unsigned int  j = 0; j < size; j++){
            if (sorted[location+i - size/2 + j] != array[i]) {
                sorted[location + i - size / 2 + j] = array[i];
                break;
            }
        }
    }

    //Update original array
    for (unsigned int i = 0; i < size; i++)
        array[i] = sorted[i];
    free(sorted);

}

void merge_sort(int *array, int length_haystack, int current_thread_nr){
    if (length_haystack > 1){
        if (current_thread_nr < max_threads) {
            // array is split and passed between 2 threads
            merge_sort(&array[0], length_haystack/2, current_thread_nr);
            thread second_half = thread(merge_sort, &array[length_haystack / 2],
                                        length_haystack - (length_haystack / 2), current_thread_nr+1);
            second_half.join();

        }
        else{
            merge_sort(&array[0], length_haystack/2, current_thread_nr);
            merge_sort( &array[length_haystack/2], length_haystack - (length_haystack/2), current_thread_nr);
        }
        // The 2 arrays are returned and now get passed to be sorted/merged
        merge(array, length_haystack);
    }
}

int main()
{

    // create array
    srand(time(nullptr));
    int *array = (int *)malloc(sizeof(int) * array_size);
    for (size_t i = 0; i < array_size; i++){
        array[i] = rand() % 100000;
    }

    for (int i = 1; i < 9; i++) {
        max_threads = i;
        auto start = std::chrono::steady_clock::now();
        // Merge sort start
        merge_sort(array, array_size, 1);

        auto end = std::chrono::steady_clock::now();

        // Check if sorted
        int current_number = array[0];
        for (size_t i = 1; i < array_size; i++){
            if (current_number > array[i]){
                cout << "error" << endl;
                return 0;
            }
        }
        cout << "sorted" << endl;
        cout << "Elapsed time in nanoseconds: "
             << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
             << " ns." << " Max threads used: " << max_threads << endl;


    }
    return 0;
}


