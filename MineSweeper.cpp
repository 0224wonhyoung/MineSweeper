// MineSweeper.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <Bangtal.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#pragma comment (lib, "Bangtal.lib")
using namespace std;

#define MODE_CLICK 0
#define MODE_FLAG 1

#define SHOW 0
#define HIDE 1
#define FLAG 2

SceneID scene1;
ObjectID piece[81];
ObjectID clickButton, flagButton, restartButton;
TimerID timer1;
TimerID timer2;

int arr[9][9];
int board[81];
int hide[81];  //  0 : show(숫자표시), 1 : hide, 2 : flag
int clickMode; // 0 : click, 1 :flag
int  clearNum; // init : 0, claer : 81
int tick;
int mine[10];

bool playing = false, gameover = false;

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


int stack=0;
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
int coolX(int x, int y) {
    return 340 + 50 * x;
}
int coolY(int x, int y) {
    return 510 - 50 * y;
}
int coolX(int num) {
    return coolX(arrX(num), 0);
}
int coolY(int num) {
    return coolY(0, arrY(num));
}



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
    int count = 0, i=0, x=0,y=0,t=0;
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
    
    int j = 0, tx = 0, ty = 0, n=0;
    // 배열의 앞에 10개가 지뢰가될거임.
    for (i = 0; i < 10; i++) {
        n = temp[i];
        x = arrX(n);
        y = arrY(n);
        setArrBoard(n, 9);
        mine[i] = n;
        for (j = 0; j < 8; j++) {
            tx = x + dx[j];
            ty = y + dy[j];
            // 9X9 타일의 안쪽일때
            if (tx >= 0 && ty >= 0 && tx<9 && ty <9) {
                arr[tx][ty] = arr[tx][ty]+1;
                board[number(tx, ty)] = board[number(tx,ty)] +1;
            }
        }
    }
}

void initArrBoard() {
    printf("초기화 시작\n");
    //arr 전부 0 초기화
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            setArrBoard(i, j, 0);
            hide[9 * i + j] = 1;
            setObjectImage(piece[9 * i + j], imgFileLocate[10]);
        }
    }printf("초기화 중간1\n");
    for (int i = 0; i < 10; i++) {
        mine[i] = 0;
    }printf("초기화 중간2\n");
    gameover = false;
    clearNum = 0;
    tick = 0;
    
    setTimer(timer1, 1.0f);
    setTimer(timer2, 0);
    printf("초기화 끝\n");
}

void setImage(int num, int imgNum) {
    setObjectImage(piece[num], imgFileLocate[imgNum]);
}


void gameOver() {
    gameover = true;
    playing = false;
    stopTimer(timer1);
    int n = 0;
    for (int i = 0; i < 10; i++) {
        n = mine[i];
        setObjectImage(piece[n], imgFileLocate[9]);
    }
    showObject(restartButton);
}

void gameClear() {
    playing = false;
    stopTimer(timer1);
    int n = 0;
    for (int i = 0; i < 10; i++) {
        n = mine[i];
        setObjectImage(piece[n], "Images\\safe.jpg");
    }
    showObject(restartButton);
}

void show(int num) {
    if (hide[num] == 0) {
        return;
    }
    playing = false;
    int a = board[num];
    if (a < 9) {
        setImage(num, a);
        hide[num] = 0;
        clearNum++;
    }

    // 만약 0이면, 인접한 칸들을 밝혀주는 재귀함수
    // stack<1000 : 뭔가가 잘못되서 재귀가 너무많이돌아가는것 방지
    if (a == 0 && stack < 1000) {

        int x = arrX(num), y = arrY(num);
        int i = 0, tx = 0, ty = 0, tn = 0;
        for (i = 0; i < 8; i++) {
            tx = x + dx[i];
            ty = y + dy[i];
            if (tx >= 0 && ty >= 0 && tx < 9 && ty < 9) {
                stack++;
                show(number(tx, ty));
            }
        }
    }
    playing = true;
    if (clearNum == 71) {
        gameClear();

    }
    
}

void mouseCallback(ObjectID object, int x, int y, MouseAction) {
    if (!gameover) {
        // num : 칸 번호
        int num = findNum(object);
        

        // 첫 클릭
        if (!playing)  {
            if(num>=0){
                
                initMine(num);
                playing = true;
                show(num);


                //정답을 콘솔에 출력
                printf("\n");
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {
                        if (arr[j][i] > 8) {
                            printf("* ");

                        }
                        else printf("%d ", arr[j][i]);
                    }
                    printf("\n");
                }
                startTimer(timer1);
            }
        }
        else {
            
            
            if (num >= 0) {
                if (clickMode == MODE_CLICK) {
                    int a = board[num];
                    // hide 를 클릭한 경우
                    if (hide[num] == HIDE) {
                        // 지뢰 밟음
                        if (a > 8) {
                            setObjectImage(object, imgFileLocate[9]);
                            gameOver();
                        }
                        else {
                            show(num);
                        }
                    }
                    // 이미 밝혀진 숫자를 클릭한 경우
                    else if (hide[num] == SHOW) {
                        int ax = arrX(num), ay = arrY(num);
                        int i = 0, tx = 0, ty = 0, tn = 0;
                        bool clear = true;
                        for (i = 0; i < 8; i++) {
                            tx = ax + dx[i];
                            ty = ay + dy[i];
                            tn = number(tx, ty);
                            if (tx >= 0 && ty >= 0 && tx < 9 && ty < 9) {
                                //지뢰인데 깃발처리안했으면 게임오버
                                if (board[tn] > 8 && (hide[tn] != FLAG)) {
                                    clear = false;
                                    gameover = true;
                                    playing = false;
                                    setObjectImage(piece[tn], imgFileLocate[9]);
                                    gameOver();
                                    
                                    return;
                                }
                                // 깃발인데 지뢰가 아니여도 게임오버
                                else if (hide[tn] == FLAG && board[tn] < 9) {
                                    clear = false;
                                    gameover = true;
                                    playing = false;
                                    gameOver();
                                    
                                    return;
                                }
                            }
                            
                        }
                        // 자기주변 지뢰는 다 찾은상황
                        
                        for (i = 0; i < 8; i++) {
                            tx = ax + dx[i];
                            ty = ay + dy[i];
                            tn = number(tx, ty);
                            if (tx >= 0 && ty >= 0 && tx < 9 && ty < 9) {
                                show(tn);
                            }
                        }

                    }
                    // else hide[num] == FLAG -> 깃발을 클릭한 경우로, 아무런 동작 하지않음
                }
                // clickMode == MODE_FLAG
                else {
                    if (hide[num] == HIDE) {
                        hide[num] = FLAG;
                        setObjectImage(object, imgFileLocate[11]);
                        
                    }
                    else if (hide[num] == FLAG) {
                        hide[num] = HIDE;
                        setObjectImage(object, imgFileLocate[10]);

                    }
                }

            }
            // 칸이 아닌 버튼류를 누른 경우
            else {
                if (object == clickButton) {
                    clickMode = MODE_CLICK;
                    setObjectImage(clickButton, "Images\\click2.png");
                    setObjectImage(flagButton, "Images\\flag1.png");
                }
                else if (object == flagButton) {
                    clickMode = MODE_FLAG;
                    setObjectImage(clickButton, "Images\\click1.png");
                    setObjectImage(flagButton, "Images\\flag2.png");
                }
                
            }
        }
    }
    if (object == restartButton) {
    initArrBoard();
    hideObject(restartButton);
    }
    
    
}


void timerCallback(TimerID timer) {
    if (timer == timer1 && playing) {
        tick++;
        setTimer(timer1, 1.0f);
        startTimer(timer1);
        setTimer(timer2, tick);
    }
}
int main()
{
    setMouseCallback(mouseCallback);
    setTimerCallback(timerCallback);
    
    timer1 = createTimer(1.0f);
    timer2 = createTimer(0);
    showTimer(timer2);

    scene1 = createScene("지뢰찾기", "Images\\배경.jpg");
    for (int i = 0; i < 81; i++) {
        piece[i] = createObject("칸", imgFileLocate[10]);
        locateObject(piece[i], scene1, coolX(i), coolY(i));
        showObject(piece[i]);
    }

    clickButton = createObject("클릭모드", "Images\\click2.png");
    locateObject(clickButton, scene1, 800, 400);
    showObject(clickButton);
    flagButton = createObject("깃발모드", "Images\\flag1.png");
    locateObject(flagButton, scene1, 800, 320);
    showObject(flagButton);
    restartButton = createObject("재시작", "Images\\restart1.png");
    locateObject(restartButton, scene1, 800, 120);


    initArrBoard();
    
    
    timer1 = createTimer(1.0f);
    timer2 = createTimer(0);
    showTimer(timer2);


    
    //TEST
            

    startGame(scene1);
    

}
