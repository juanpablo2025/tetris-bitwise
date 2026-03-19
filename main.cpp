#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>

using namespace std;

struct Piece {
    uint16_t shape;
    int size;
};

struct Game {
    unsigned char** board;
    int width;
    int height;
    int bytesPerRow;

    Piece current;
    int posX;
    int posY;
};

Piece pieces[5] = {
    {0x0F00, 4},
    {0x6600, 2},
    {0x4E00, 3},
    {0x6C00, 3},
    {0xC600, 3}
};

unsigned char** createBoard(int h, int bytesPerRow) {
    unsigned char** board = new unsigned char*[h];
    for (int i = 0; i < h; i++) {
        board[i] = new unsigned char[bytesPerRow];
        for (int j = 0; j < bytesPerRow; j++)
            board[i][j] = 0;
    }
    return board;
}

bool getBit(unsigned char* row, int col) {
    int byteIndex = col / 8;
    int bitIndex = col % 8;
    return (row[byteIndex] >> (7 - bitIndex)) & 1;
}

void setBit(unsigned char* row, int col) {
    int byteIndex = col / 8;
    int bitIndex = col % 8;
    row[byteIndex] |= (1 << (7 - bitIndex));
}

void draw(Game& g) {
    system("cls"); // en Windows usa "cls"

    for (int i = 0; i < g.height; i++) {
        cout << "|";

        for (int j = 0; j < g.width; j++) {

            bool isPiece = false;

            // verificar si la pieza está aquí
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    if ((g.current.shape >> (15 - (r * 4 + c))) & 1) {
                        if (i == g.posY + r && j == g.posX + c) {
                            isPiece = true;
                        }
                    }
                }
            }

            if (isPiece) {
                cout << "@"; // pieza activa
            } else if (getBit(g.board[i], j)) {
                cout << "#";
            } else {
                cout << ".";
            }
        }

        cout << "|" << endl;
    }

    // borde inferior
    cout << "+";
    for (int i = 0; i < g.width; i++) cout << "-";
    cout << "+" << endl;

    cout << "A:Izq D:Der S:Bajar Q:Salir\n";
}

bool collision(Game& g, int newX, int newY) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if ((g.current.shape >> (15 - (r * 4 + c))) & 1) {
                int boardX = newX + c;
                int boardY = newY + r;

                if (boardX < 0 || boardX >= g.width || boardY >= g.height)
                    return true;

                if (boardY >= 0 && getBit(g.board[boardY], boardX))
                    return true;
            }
        }
    }
    return false;
}

void placePiece(Game& g) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if ((g.current.shape >> (15 - (r * 4 + c))) & 1) {
                int x = g.posX + c;
                int y = g.posY + r;
                if (y >= 0)
                    setBit(g.board[y], x);
            }
        }
    }
}

void clearLines(Game& g) {
    for (int i = 0; i < g.height; i++) {
        bool full = true;

        for (int j = 0; j < g.width; j++) {
            if (!getBit(g.board[i], j)) {
                full = false;
                break;
            }
        }

        if (full) {
            for (int k = i; k > 0; k--) {
                for (int b = 0; b < g.bytesPerRow; b++)
                    g.board[k][b] = g.board[k - 1][b];
            }

            for (int b = 0; b < g.bytesPerRow; b++)
                g.board[0][b] = 0;
        }
    }
}

void spawn(Game& g) {
    g.current = pieces[rand() % 5];
    g.posX = g.width / 2 - 2;
    g.posY = 0;
}

int main() {
    srand(time(0));

    Game g;

    cout << "Width (multiple of 8): ";
    cin >> g.width;
    cout << "Height: ";
    cin >> g.height;

    g.bytesPerRow = g.width / 8;
    g.board = createBoard(g.height, g.bytesPerRow);

    spawn(g);

    while (true) {
        draw(g);

        char input;
        cin >> input;

        int newX = g.posX;
        int newY = g.posY;

        if (input == 'a') newX--;
        if (input == 'd') newX++;
        if (input == 's') newY++;

        if (!collision(g, newX, newY)) {
            g.posX = newX;
            g.posY = newY;
        } else if (input == 's') {
            placePiece(g);
            clearLines(g);
            spawn(g);

            if (collision(g, g.posX, g.posY)) {
                cout << "GAME OVER\n";
                break;
            }
        }
    }

    return 0;
}