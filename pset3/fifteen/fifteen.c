/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */

#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

// my global variables
// blank tile location
int blanki, blankj;

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE *file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();

        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }

    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(void)
{
    // set the largest tile value
    int tile = d * d - 1;

    // to start the board right, swap tiles 1, 2 at the bottom of the board
    bool swap = 0;
    swap = (d * d) % 2 == 0 ? true : false;

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // if last row
            if (swap && i == d - 1 && (j == d - 3 || j == d - 2))
            {
                if (tile == 2)
                {
                    board[i][j] = 1;
                }
                if (tile == 1)
                {
                    board[i][j] = 2;
                }
            }
            // if very last tile (lower right)
            else if (i == d - 1 && j == d - 1)
            {
                board[i][j] = 0;
                blanki = i;
                blankj = j;
            }
            else
            {
                board[i][j] = tile;
            }
            tile = tile - 1;
        }
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] == 0)
            {
                printf(" _");
            }
            else
            {
                printf("%2i ", board[i][j]);
            }
        }
        printf("\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false.
 */
bool move(int tile)
{
    // keep location of the blank tile
    int tilei, tilej;
    int tmpi, tmpj;

    // find location of the given tile
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] == tile)
            {
                // save
                tilei = i;
                tilej = j;
                // no need to go through the rest of the board
                break;
            }
        }
    }

    // check if location is next to blank
    if (((tilei - 1 == blanki || tilei + 1 == blanki) && (tilej == blankj)) || ((tilej - 1 == blankj || tilej + 1 == blankj) && (tilei == blanki)))
    {
        // "move" blank temporarily
        tmpi = blanki;
        tmpj = blankj;
        // replace blank with tile
        board[blanki][blankj] = tile;
        // replace tile with blank's value
        board[tilei][tilej] = 0;

        // save blank's new location
        blanki = tilei;
        blankj = tilej;

        clear();
        draw();
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Returns true if game is won (i.e., board is in winning configuration),
 * else false.
 */
bool won(void)
{
    int checktile = 1;
    int inorder = 1;

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // checkvalue != board's tile value
            if (board[i][j] != checktile && board[i][j] != 0)
            {
                inorder = 0;
                break;
            }
            checktile++;
        }
    }

    return inorder;
}
