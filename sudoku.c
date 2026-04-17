#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 9
#define MAX_ENTRIES 100

typedef struct
{
    char name[100];
    char difficulty[20];
    int time_taken;
    char date[20];
} Entry;

void printBoard(int board[SIZE][SIZE])
{
    printf("\nSudoku Board:\n");
    for (int i = 0; i < SIZE; i++)
    {
        if (i % 3 == 0 && i != 0)
            printf("---------------------\n");
        for (int j = 0; j < SIZE; j++)
        {
            if (j % 3 == 0 && j != 0)
                printf("| ");
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

int isValidMove(int board[SIZE][SIZE], int row, int col, int num)
{
    for (int i = 0; i < SIZE; i++)
    {
        if (board[row][i] == num || board[i][col] == num)
            return 0;
    }
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i + startRow][j + startCol] == num)
                return 0;
        }
    }
    return 1;
}

int solveSudoku(int board[SIZE][SIZE])
{
    for (int row = 0; row < SIZE; row++)
    {
        for (int col = 0; col < SIZE; col++)
        {
            if (board[row][col] == 0)
            {
                for (int num = 1; num <= 9; num++)
                {
                    if (isValidMove(board, row, col, num))
                    {
                        board[row][col] = num;
                        if (solveSudoku(board))
                            return 1;
                        board[row][col] = 0;
                    }
                }
                return 0;
            }
        }
    }
    return 1;
}

void shuffleArray(int *array, int size)
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void fillDiagonalBoxes(int board[SIZE][SIZE])
{
    for (int k = 0; k < SIZE; k += 3)
    {
        int nums[SIZE];
        for (int i = 0; i < SIZE; i++)
            nums[i] = i + 1;
        shuffleArray(nums, SIZE);
        int idx = 0;
        for (int i = k; i < k + 3; i++)
        {
            for (int j = k; j < k + 3; j++)
            {
                board[i][j] = nums[idx++];
            }
        }
    }
}

void generateFullBoard(int board[SIZE][SIZE])
{
    memset(board, 0, SIZE * SIZE * sizeof(int));
    fillDiagonalBoxes(board);
    solveSudoku(board);
}

void removeCells(int board[SIZE][SIZE], const char *difficulty)
{
    int blanks;
    if (strcmp(difficulty, "Easy") == 0)
        blanks = 35;
    else if (strcmp(difficulty, "Medium") == 0)
        blanks = 45;
    else
        blanks = 55;

    while (blanks > 0)
    {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        if (board[row][col] != 0)
        {
            board[row][col] = 0;
            blanks--;
        }
    }
}

int isSolved(int board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

int compareEntries(const void *a, const void *b)
{
    return ((Entry *)a)->time_taken - ((Entry *)b)->time_taken;
}

void showLeaderboard(const char *difficulty)
{
    Entry entries[MAX_ENTRIES];
    int count = 0;
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file)
        return;

    while (fscanf(file, "%[^,],%[^,],%d,%[^\n]\n", entries[count].name, entries[count].difficulty,
                  &entries[count].time_taken, entries[count].date) == 4)
    {
        if (strcmp(entries[count].difficulty, difficulty) == 0)
            count++;
    }
    fclose(file);

    qsort(entries, count, sizeof(Entry), compareEntries);

    printf("\nTop 5 Players (%s):\n", difficulty);
    for (int i = 0; i < count && i < 5; i++)
    {
        printf("%s - %d seconds on %s\n", entries[i].name, entries[i].time_taken, entries[i].date);
    }
}

void addToLeaderboard(const char *name, const char *difficulty, int time_taken)
{
    FILE *file = fopen("leaderboard.txt", "a");
    if (!file)
        return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d", t);

    fprintf(file, "%s,%s,%d,%s\n", name, difficulty, time_taken, date);
    fclose(file);
}

void playSudoku(int board[SIZE][SIZE], const char *difficulty)
{
    int row, col, num;
    time_t start_time = time(NULL);

    while (!isSolved(board))
    {
        printBoard(board);
        printf("Enter row (0-8), column (0-8), and number (1-9) to place: ");
        scanf("%d %d %d", &row, &col, &num);
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == 0 && isValidMove(board, row, col, num))
        {
            board[row][col] = num;
        }
        else
        {
            printf("Invalid move, try again.\n");
        }
    }

    time_t end_time = time(NULL);
    int duration = (int)difftime(end_time, start_time);

    printf("Congratulations! You solved the Sudoku.\n");
    printBoard(board);

    char player[100];
    printf("Enter your name for the leaderboard: ");
    scanf("%s", player);

    addToLeaderboard(player, difficulty, duration);
    showLeaderboard(difficulty);
}

void generatePuzzle(int board[SIZE][SIZE], const char *difficulty)
{
    generateFullBoard(board);
    removeCells(board, difficulty);
}

void selectDifficulty(int board[SIZE][SIZE], char *difficulty)
{
    int choice;
    printf("Select Difficulty Level: 1. Easy 2. Medium 3. Hard: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        strcpy(difficulty, "Easy");
        break;
    case 2:
        strcpy(difficulty, "Medium");
        break;
    case 3:
        strcpy(difficulty, "Hard");
        break;
    default:
        printf("Invalid choice, defaulting to Easy.\n");
        strcpy(difficulty, "Easy");
    }
    generatePuzzle(board, difficulty);
}

int main()
{
    srand(time(NULL));
    char playAgain = 'y';
    while (playAgain == 'y' || playAgain == 'Y')
    {
        int board[SIZE][SIZE] = {0};
        char difficulty[20];
        selectDifficulty(board, difficulty);
        playSudoku(board, difficulty);

        printf("\nWould you like to play again? (y/n): ");
        scanf(" %c", &playAgain);
    }

    printf("Thanks for playing!\n");
    return 0;
}