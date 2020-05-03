// MineSweeper.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <Bangtal.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#pragma comment (lib, "Bangtal.lib")
using namespace std;


SceneID scene1;
ObjectID piece[81];

int arr[9][9];
int board[81];

bool playing = false, pause = false;

// 0~8 : 각 숫자, 9이상 : 지뢰, 10 : 빈칸
const char* imgFileLocate[12] = {"Images\\p0.jpg", "Images\\p1.jpg", "Images\\p2.jpg", "Images\\p3.jpg", "Images\\p4.jpg", 
"Images\\p5.jpg", "Images\\p6.jpg", "Images\\p7.jpg", "Images\\p8.jpg", "Images\\p9.jpg", "Images\\hide.jpg", "Images\\flag.jpg" };

/*
   1 2 3
   4   5
   6 7 8
*/
int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int dy[8] = { -1, -1, -1, 0, 0 , 1, 1, 1 };


// 0 ~ n-1 중 random한 숫자 생성
int myRand(int n) {
    int k = rand() % n;
    if (n > 0) {
        return k;
    }
    return 0;
}

// num 번째 칸이 배열에서의 X좌표
int arrX(int num) {
    return num % 9;
}

// num 번째 칸이 배열에서의 Y좌표
int arrY(int num) {
    return (int)(num / 9);
}

// 배열에서 (x,y)좌표는 num번째 칸
int number(int x, int y) {
    return (9 * y + x);
}

//TODO coolX, coolY


// Arr과 Board의 값을 n으로 바꾸기
void setArrBoard(int x, int y, int n) {
    arr[x][y] = n;
    board[number(x, y)] = n;
}
void setArrBoard(int num, int n) {
    arr[arrX(num)][arrY(num)] = n;
    board[num] = n;
}



// mouseCallback 에서 쓰일, 몇번째 칸을 클릭한건지 알아내기위한 함수
int findNum(ObjectID object) {
    for (int i = 0; i < 81; i++) {
        if (piece[i] == object) {
            return i;
        }
    }
    return -1; // piece 말고 다른걸 클릭했을경우 -1
}



void initMine(int num) {
    // 첫 클릭(=num)을 제외한 80개의 번호를 가진 배열 생성
    int temp[80];
    int count = 0, i=0, x,y,t;
    for (i = 0; i < 81; i++) {
        if (i == num) i++;
        temp[count] = i;
        count++;
    }

    // 배열을 1000번 섞기
    for (i = 0; i < 1000; i++) {
        x = myRand(80);
        y = myRand(80);
        if (x != y) {
            t = temp[x];
            temp[x] = temp[y];
            temp[y] = t;
        }
    }

    int j = 0, tx = 0, ty = 0, n;
    // 배열의 앞에 10개가 지뢰가될거임.
    for (i = 0; i < 10; i++) {
        n = temp[i];
        x = arrX(n);
        y = arrY(n);
        setArrBoard(n, 9);
        for (j = 0; j < 8; j++) {
            tx = x + dx[j];
            ty = y + dy[j];
            // 9X9 타일의 안쪽일때
            if (tx >= 0 || ty >= 0 || tx<9 || ty <9) {
                arr[tx][ty]++;
                board[number(tx, ty)]++;
            }
        }
    }
}

void initArrBoard() {
    //arr 전부 0 초기화
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            setArrBoard(i, j, 0);
        }
    }
}

void setImages(int num, int imgNum) {
    setObjectImage(piece[num], imgFileLocate[imgNum]);
}

int main()
{
    /*
    scene1 = createScene("지뢰찾기", "Image\\배경.jpg");
    for (int i = 0; i < 81; i++) {
        piece[i] = createObject("칸", imgFileLocate[10]);
        locateObject(piece[i], scene1, coolX(i), coolY(j));
        showObject(piece[i]);
    }*/
    initArrBoard();
   
    initMine(3); 
    

  
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (arr[j][i] > 8) {
                printf("* ");
               
            }
            else printf("%d ", arr[j][i]);
        }
        printf("\n");
    }
    

}
