//Bryant R. Hays
//02/14/2023
//CS 470 Lab03


#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <random>
#include <vector>
#include <algorithm>


using namespace std;

#define MAX_THREADS 100

pthread_mutex_t lock;
bool isComplete = false;
bool isSorted = false;

//Structures for both the problems.
struct problemOneRunnerStruct {
    pthread_t threadId = 0;
    int** matrix = nullptr;
    int n = 0;
};

struct problemTwoRunnerStruct {
    pthread_t threadId = 0;
    vector<int>* arr = nullptr;
    int length = 0;
};

//Function declarations.
int **generateRandomMatrix(int n);

void printMatrix(int **matrix, int n);

vector<int>* generateRandomArray(int n);


//Function for the problem 1 supervisor thread to check if matrix is completely filled with 1s or 0s.
void *checkIfComplete(void* arguments){

    struct problemOneRunnerStruct *args =
            (struct problemOneRunnerStruct*) arguments;

    while (!isComplete) {

        pthread_mutex_lock(&lock);
        int zeros = 0;
        int ones = 0;

        for (int i = 0; i < args->n; i++) {
            for (int j = 0; j < args->n; j++) {
                if (args->matrix[i][j] == 0) {
                    zeros++;
                } else {
                    ones++;
                }
            }
        }

        if (zeros == 0 || ones == 0) {
            isComplete = true;
            cout << "\nThe board is now complete!";
            cout << "\nThe resulting board is ";
            printMatrix(args->matrix, args->n);
        }

        pthread_mutex_unlock(&lock);

    }
    return NULL;
}


//Function for the problem 2 supervisor thread to check if the array is sorted.
void *checkIfSorted(void* arguments){

    struct problemTwoRunnerStruct *args =
            (struct problemTwoRunnerStruct*) arguments;

    while (!isSorted) {
        pthread_mutex_lock(&lock);
        if (is_sorted((*args->arr).begin(), (*args->arr).end())) {
            isSorted = true;
            cout << "\nThe array is now sorted!";
            cout << "\nThe resulting array is: ";
            cout << "\n";
            for (int i = 0; i < args->length; i++) {

                cout << (*args->arr)[i] << " ";
            }
            cout << "\n";
        }

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}



//Function to run problem one using thread construct.
void* problemOneRunner(void* arguments){

    //Struct to store all the necessary arguments.
    struct problemOneRunnerStruct *args =
            (struct problemOneRunnerStruct*) arguments;

    //Generator for random row and column selection.
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> row_select(0, args->n - 1);
    std::uniform_int_distribution<std::mt19937::result_type> column_select(0, args->n - 1);
    std::uniform_int_distribution<std::mt19937::result_type> rand_select(0, 1);

        int row = 0;
        int col = 0;
        int zeros = 0, ones = 0;
        bool change = false;

    //While matrix isn't completely filled with 1s and 0s, keep flipping the elements.
    while (!isComplete) {
        //Generate random row and column.
        row = row_select(rng);
        col = column_select(rng);

        pthread_mutex_lock(&lock);
        change = false;

        zeros = 0, ones = 0;

        //Count the neighbors.
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int a = row + i, b = col + j;
                if (a >= 0 && a < args->n && b >= 0 && b < args->n && !(i == 0 && j == 0)) {
                    if (args->matrix[a][b] == 0) {
                        zeros++;
                    } else {
                        ones++;
                    }
                }

            }
        }

        // Update the element if most of the neighbors are zeros or ones
        if (zeros > ones && args->matrix[row][col] == 1) {
            change = true;
            args->matrix[row][col] = 0;
        } else if (ones > zeros && args->matrix[row][col] == 0) {
            change = true;
            args->matrix[row][col] = 1;
        }

        //Print the thread ID and selection.
        if (change == true) {
            cout << "\n";
            cout << "\nThread ID #" << args->threadId;
            cout << "\nRow Selected: " << row;
            cout << "\nColumn Selected: " << col;
            cout << "\n" << args->matrix[row][col] << " was the majority. Flipping element to " << args->matrix[row][col];
            printMatrix(args->matrix, args->n);
            cout << "\n";
        }

        pthread_mutex_unlock(&lock);

    }
    return NULL;
}

//Function to run problem two using thread construct.
void* problemTwoRunner(void* arguments){

    //Struct to store all the necessary arguments.
    struct problemTwoRunnerStruct *args =
            (struct problemTwoRunnerStruct*) arguments;

    int index = 0;
    bool change = false;

    //Generator for random index.
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> index_select(0, args->length - 1);


    while (!isSorted) {

        //Generate random index.
        index = index_select(rng);


        //Lock segment.
        pthread_mutex_lock(&lock);
        change = false;

        //Sort the array from selected index.
        int temp = (*args->arr)[index];
        int j = index;

        while (j > 0 && (*args->arr)[j - 1] > temp) {
            change = true;
            (*args->arr)[j] = (*args->arr)[j - 1];
            j--;
        }

        (*args->arr)[j] = temp;

            //If the array changed, print out the thread information and resulting array.
            if (change == true) {
                cout << "\nThread ID #" << args->threadId << " selected index " << index
                     << ", which contains the number "
                     << (*args->arr)[j] << ". Element moved to slot " << j;
                cout << "\nThe resulting array is: ";
                cout << "\n";
                for (int i = 0; i < args->length; i++) {

                    cout << (*args->arr)[i] << " ";
                }

                cout <<"\n";
            }

        //If the array is sorted, exit the procedure.
        if (is_sorted((*args->arr).begin(), (*args->arr).end())){
            isSorted = true;
        }

        //Unlock the segment/
        pthread_mutex_unlock(&lock);
    }

    return NULL;

}

//Function to generate the random array for problem 2.
vector<int>* generateRandomArray(int n) {
    std::random_device randomNum;
    std::mt19937 generator(randomNum());
    std::uniform_int_distribution<> dis(1, 1000);

    vector<int>* arr = new vector<int>(n);
    srand((unsigned int)time(nullptr));

    for (int i = 0; i < n; i++){

        (*arr)[i] = dis(generator);
    }

    return arr;
}

//Function to print the matrix for problem 1.
void printMatrix(int **matrix, int n) {

    for (int i = 0; i < n; i++){
        cout << "\n";
        for (int j = 0; j < n; j++){
            cout << matrix[i][j] << " ";
        }
    }
    cout << "\n";
}

//Generates and returns a nxn matrix randomly populated with 0s and 1s.
int **generateRandomMatrix(int n) {

    int** matrix = new int*[n];
    srand((unsigned int)time(nullptr));

    for (int i = 0; i < n; i++){
        matrix[i] = new int[n];
        for (int j = 0; j < n; j++){

            matrix[i][j] = rand() % 2;
        }
    }

    return matrix;
}


int main(int argc, char *argv[]) {

    bool exitMenu = false;
    int selection = 0;
    int num_threads = 0;
    string choice;

    //If no arguments are provided, then break the program.
    if (argc < 2){
        cout << "Error: a board size must be provided!" << endl;
        exit(0);
    }

    //Get the size of the matrix/array from the command line.
    int n = atoi(argv[1]);


    //Check for erroneous matrix/array size. The size can be 3x3, 5x5, or 7x7.
    if (n < 3 || n > 7 || n % 2 != 1){
        cout << "\nError: matrix/array size n must be an odd number greater than or equal to 3 (for 3 x 3 matrix) and less than or equal to 7 (for 7 x 7 matrix)." << endl;
        exit(0);
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    //Execute problems until user explicitly exits.
    do {

        cin.clear();
        fflush(stdin);
        selection = 0;

            //Prompt user until input is valid.
            cout << "\n1 - Problem 1";
            cout << "\n2 - Problem 2";
            cout << "\n3 - exit";
            cout << "\nPlease make a menu selection: ";

            while(!(cin >> selection) || selection > 3 || selection < 1){
                cout << "\nError: Please make a valid menu selection!";
                cout << "\nPlease make a menu selection: ";
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        //Exit condition.
        if (selection == 3) {
            exitMenu = true;
        }

        //Run problem 1.
        else if (selection == 1){

            num_threads = 0;

            //Ask user for the number of treads they would like to run.
            cout << "\nHow many threads would you like to run?";
                while (!(cin >> num_threads) || num_threads <= 0 || num_threads > MAX_THREADS){

                    cout << "\nError: Please enter a valid number of threads! Thread count must be a positive integer less than or equal to 100.";
                    cout << "\nHow many threads would you like to run?";
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                }

            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


            //Array of pthread ids.
            pthread_t tids[num_threads+1];

            //Generate matrix;
            int** matrix;
            matrix = generateRandomMatrix(n);
            isComplete = false;
            printMatrix(matrix, n);

            //Arguments for the process.
            struct problemOneRunnerStruct args;

            //Launch the supervisor thread.
            args.threadId = tids[num_threads];
            args.matrix = matrix;
            args.n = n;
            pthread_create(&tids[num_threads], &attr, checkIfComplete, &args);


            //Launch threads.
            for (int i = 0; i < num_threads; i++) {

                    args.threadId = tids[i];
                    args.matrix = matrix;
                    args.n = n;
                    pthread_create(&tids[i], &attr, problemOneRunner, &args);

                }

            //Wait for the threads to finish working.
            for (int i = 0; i <= num_threads; i++) {
                pthread_join(tids[i], nullptr);
            }

        }

        //Run problem 2.
        else if (selection == 2){
            num_threads = 0;

            //Ask user for the number of treads they would like to run.
            cout << "\nHow many threads would you like to run?";
            while (!(cin >> num_threads) || num_threads <= 0 || num_threads > MAX_THREADS){

                cout << "\nError: Please enter a valid number of threads! Thread count must be a positive integer less than or equal to 100.";
                cout << "\nHow many threads would you like to run?";
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            }

            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            //Array of pthread ids.
            pthread_t tids[num_threads+1];

            //Generate the array.
            vector<int> arr;
            arr = *generateRandomArray(n);
            isSorted = false;

            //Print the initial array.
            for (int i = 0; i < n; i++){

               cout << arr[i] << " ";
            }

            //Arguments for the process.
            struct problemTwoRunnerStruct args;

            //Launch the supervisor thread.
            args.threadId = tids[num_threads];
            args.arr = &arr;
            args.length = n;
            pthread_create(&tids[num_threads], &attr, checkIfSorted, &args);

            //Launch threads.
            for (int i = 0; i < num_threads; i++){
                args.arr = &arr;
                args.length = n;
                args.threadId = tids[i];

                pthread_create(&tids[i], &attr, problemTwoRunner, &args);

                if(isSorted == true){
                    pthread_cancel(tids[i]);
                }
            }

            /*
            //Start the supervisor thread to see if the process is over.
            pthread_t supervisor;
            args.arr = &arr;
            args.length = n;
            args.threadId = supervisor;

            pthread_create(&supervisor, &attr, checkIfSorted, &args);
             */

            //Wait for worker threads to finish.
            for (int i = 0; i <= num_threads; i++){
                pthread_join(tids[i], nullptr);
            }

            //Wait for the supervisor thead to finish.
            //pthread_join(supervisor, nullptr);

            arr.clear();
        }




    } while (exitMenu == false);

    return 0;
}