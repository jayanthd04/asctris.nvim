#if __has_include(<ncurses/ncurses.h>)
    #include <ncurses/ncurses.h>
#elif __has_include(<ncurses.h>)
    #include <ncurses.h>
#else 
    #error "ncurses.h was not found please verify that ncurses has been properly installed"
#endif
//#include <ncurses.h>
//#include<windows.h>
#include<iostream>
#include <vector>
#include <atomic>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
using namespace std; 

// game board is 10 cells wide by 16 cells height
class Tetris{
    // game logic: 
    // get random tetrimino 
    // Place block at the top center of the grid 
    // while the tetrimino has not collided with another block 
    // to it's relative bottom, user can shift the block left, right 
    // and rotate the block.
    // atter it has collided, the block is placed at the position and orientation
    // that it has collided at. 
    // if any row is filled with blocks, the row can be cleared 
    // if row is cleared, 
    private: 
        //vector<vector<char>> board{16,vector<char>(10,' ')};
        // resize board to take into account the vanish zone 
        vector<vector<char>> board{32,vector<char>(10,' ')};
        vector<pair<vector<vector<char>>,vector<int>>> tetrim = {
            {
                {
                    {'=','=','=','='}
                },
                /*{0,2}*/
                {0,1}
            },
            {
                {
                    {'#',' ',' '},
                    {'#','#','#'}
                },
                {1,1}
            },
            {
                {
                    {' ',' ','*'},
                    {'*','*','*'}
                },
                {1,1}
            },
            {
                {
                    {'@','@'},
                    {'@','@'}
                },
                {1,1}
            },
            {
                {
                    {' ','>','>'},
                    {'>','>',' '}
                },
                {1,1}
            },
            {
                {
                    {' ','+',' '},
                    {'+','+','+'}
                },
                {1,1}
            },
            {
                {
                    {'<','<',' '},
                    {' ','<','<'}
                },
                {1,1}
            },
        };

    public: 
        vector<vector<char>> getState(){
            return board;
        }
        void printTetrim(vector<vector<char>>& tet){
            string ret = "";
            for(int i=0;i<tet.size();i++){
                string t = string(begin(tet[i]),end(tet[i]));
                printw(t.c_str());
                if(i!=tet.size()-1){
                    printw("\n");
                }
            }
        }
        void printTetrims(){
            int i = -1; 
            int ch;
            while(ch!='q'){
                ch= getch();
                if(ch == KEY_UP) {

                    i++;
                }
                else if(ch == KEY_DOWN) i--; 
                
                if(i==tetrim.size()) i=tetrim.size()-1;
                else if(i<0) i=0;
                clear();
                printTetrim(tetrim[i].first);
                refresh();
            }
        }
        void rotateTetrim(pair<vector<vector<char>>, vector<int>>& orig){
            // update shift center of tetrim when rotating 
            int m = orig.first.size(); 
            int n = orig.first[0].size(); 
            // if original center is x=x, y=y new center is x=y, y=m-1-x
            int origX = orig.second[0];
            orig.second[0] = orig.second[1];
            orig.second[1] = (m-1)-origX;
            vector<vector<char>> ret(n,vector<char>(m));
            // row i in orig is column m-i in ret; 

            for(int j=0;j<n;j++){
                vector<char> curr;
                for(int i =0;i<m;i++){
                    curr.insert(curr.begin(),orig.first[i][j]);
                }
                ret[j]=curr; 
            }
            orig.first = ret; 
            //return ret; 
        }
        void rotateTetrimTest(){
            int ch; 
            int i = 0; 
            pair< vector<vector<char>>, vector<int>> tet = tetrim[i];
            while(ch != 'q'){
                if(ch == ' ') {
                    i++;
                    if(i==tetrim.size()) i=0;
                    tet = tetrim[i];
                }
                else if (ch == KEY_UP){
                    rotateTetrim(tet);
                }
                erase();
                refresh();
                printTetrim(tet.first);
                string cent = to_string(tet.second[0]) +","+to_string(tet.second[1]);
                printw("\n");
                printw(cent.c_str());
                refresh();
                ch = getch();
            }
        }
        pair<vector<vector<char>>, vector<int>> getRandomTetrim(){
            //srand(static_cast<unsigned int>(time(0)));
            int i = rand() % 7;
            return tetrim[i];
        }
        // renders game board onto Window gameWin
        void renderBoard(WINDOW*& gameWin){
            int height; 
            int width; 
            getmaxyx(gameWin,width,height);
            /*for(int i=0;i<board.size();i++){
                string r = string(begin(board[i]),end(board[i]));
                mvwprintw(gameWin,i+1,1,r.c_str());
            }
            wrefresh(gameWin);*/
            // alt path to account for vanish zone 
            //board[16]= vector<char>(10,'-');
            for(int i=16;i<board.size();i++){
                string r = string(begin(board[i]),end(board[i]));
                mvwprintw(gameWin,(i-16)+1,1,r.c_str());

            }
            //wrefresh(gameWin);
        }

        // renders tetrimino with center at x, y onto Window gameWin based on the current board 
        void renderTetrimFromCenter(WINDOW*& gameWin, pair <vector<vector<char>>,vector<int>>& tetrim, int x, int y){
            //renderBoard(gameWin);
            int m = tetrim.first.size();  
            int n = tetrim.first[0].size();

            // start rendering tetrim starting from the left corner which 
            // is center x, y - center of tetrim 
            int cornerX = x-tetrim.second[1];
            int cornerY = y-tetrim.second[0];
            for(int i=0;i<m;i++){
                //string t = string(begin(tetrim.first[i]), end(tetrim.first[i]));
                //const char* str = t.c_str();
                // considering board state when rendering 
                for(int j =0;j<n;j++){
                    if(board[cornerY+i+16-1][cornerX+j-1]==' '){
                        //mvwaddch(gameWin,cornerY+i,cornerX+j,t[j]);
                        // not using redundant string 
                        mvwaddch(gameWin,cornerY+i,cornerX+j,tetrim.first[i][j]);
                        //mvwprintw(gameWin,cornerY+i,cornerX+j,t[j]);
                    }
                }
                //cout<< t<<" "<<cornerY+i<<","<<cornerX<<endl;
                //mvwprintw(gameWin,cornerY+i,cornerX,t.c_str());
            }
            //wrefresh(gameWin);

        }
        //clear tetrimino with center at x, y on Window gameWin 
        void clearTetrimFromCenter(WINDOW*& gameWin, pair< vector<vector<char>>, vector<int>>& tetrim, int x, int y){
            //renderBoard(gameWin);
            int m = tetrim.first.size(); 

            int n = tetrim.first[0].size();

            int cornerX = x-tetrim.second[1];
            int cornerY = y-tetrim.second[0];
            //vector<vector<char>> erase(m,vector<char>(n,' '));
            for(int i=0;i<m;i++){
                //string t = string(begin(erase[i]), end(erase[i]));
                // considering board state when rendering 
                for(int j =0;j<n;j++){
                    if(board[cornerY+i+16-1][cornerX+j-1]==' '){
                        //mvwaddch(gameWin,cornerY+i,cornerX+j,t[j]);
                        // not using redundant vector 
                        mvwaddch(gameWin,cornerY+i,cornerX+j,' ');
                    }
                }
                //mvwprintw(gameWin,cornerY+i,cornerX,t.c_str());
            }
            //wrefresh(gameWin);
        }
        // checks if tetrim collides with any blocks on the board vertically. 
        void renderTetrimTest(WINDOW*& gameWin){
            int ch; 
            int i=0;
            pair< vector<vector<char>>, vector<int>> tet = tetrim[i];
            while(ch != 'q'){
                if(ch == ' '){
                    i++; 
                    if(i==tetrim.size()) i=0;
                    tet = tetrim[i];
                }
                else if (ch == KEY_UP){
                    clearTetrimFromCenter(gameWin,tet,5,1+tet.second[0]);
                    rotateTetrim(tet);
                }
                renderTetrimFromCenter(gameWin,tet,5,1+tet.second[0]);

                ch = getch();
            }
        }
        // checks if tetrim collides with any blocks on the board vertically. 
        bool collisionVert(pair<vector<vector<char>>, vector<int>>& tetrim, int x, int y){
            int m = tetrim.first.size();
            int n = tetrim.first[0].size(); 

            int cornerX = x-tetrim.second[1]-1;
            //int cornerY = y-tetrim.second[0]-1;
            
            // alt path to account for vanish zone 
            int cornerY = (y+16)-tetrim.second[0]-1; 
            
            // iterate through each cell in the last row 
            // if cell is not blank, check if block below curr cell on board 
            // is filled or if the cell's position is equal to height 
            // of board if so return true; 
            // if cell is blank, go a cell up till we encounter a non empty 
            // cell and check if block below is filled if so return true; 
            if(cornerY+m-1 >= board.size()-1)return true; 
            for(int j=0;j<n;j++){
                if(tetrim.first[m-1][j]!=' '){
                    //cout<< tetrim.first[m-1][j];
                    // should be placed at board[cornerY+n-1][cornerX+i]
                    if(/*cornerY+m-1 == board.size()-1|| */board[cornerY+m][cornerX+j]!=' '){
                        return true;
                    }
                }
                else{
                    int i= m-1; 
                    while(tetrim.first[i][j]==' '){
                        i--;
                    }
                    if(board[cornerY+i+1][cornerX+j]!=' ')return true;
                }
            }
            return false; 
        }
        // To-do: add horizontal collision detection 
        bool collisionLeft(pair<vector<vector<char>>, vector<int>>& tetrim, int x, int y){
            int m = tetrim.first.size(); 
            int n = tetrim.first[0].size(); 

            int cornerX = x-tetrim.second[1]-1; 
            //int cornerY = y-tetrim.second[0]-1; 

            // alt path to account for vanish zone 
            int cornerY = (y+16)-tetrim.second[0]-1;

            // iterate through each cell in the first col 
            // if cell is not blank, check if block to the left of curr 
            // on board is filled or if the x coord of the 
            // cell is equal to 0, if so return true  
            // if cell is blank, go a cell to the right till we reach 
            // a non-empty cell and check if the block to the left 
            // is filled if so return true; 
            if(cornerX == 0 )return true; 
            for(int i=0;i<m;i++){
                if(tetrim.first[i][0]!=' '){
                    // should be placed at board[cornerY+i][cornerX]
                    if(/*cornerX == 0 ||*/ board[cornerY+i][cornerX-1]!=' ')
                        return true; 
                }
                else{
                    int j = 0;
                    while(tetrim.first[i][j]==' '){
                        j++; 
                    }
                    if(board[cornerY+i][cornerX+j-1]!=' ')return true; 
                }
            }
            return false; 
        }
        bool collisionRight(pair<vector<vector<char>>, vector<int>>& tetrim, int x, int y){
            int m = tetrim.first.size(); 
            int n = tetrim.first[0].size(); 

            int cornerX = x - tetrim.second[1] - 1;
            //int cornerY = y - tetrim.second[0] -1; 

            // alt path to account for vanish zone 
            int cornerY = (y+16)-tetrim.second[0]-1;
            if(cornerX+n-1 == board[0].size()-1)return true; 
            for(int i=0;i<m;i++){
                if(tetrim.first[i][n-1]!=' '){
                    // should be placed at board[cornerY+i][cornerX+n-1];
                    if(/*cornerX+n-1 == board[0].size()-1 ||*/ board[cornerY+i][cornerX+n]!=' ')
                        return true; 
                }
                else{
                    int j =0;
                    while(tetrim.first[i][j]==' '){
                        j--; 
                    }
                    if(board[cornerY+i][cornerX+j+1]!=' ')return true; 
                }
            }
            return false; 
        }
        bool collision(pair<vector<vector<char>>, vector<int>>& tetrim, int x, int y){
            return collisionVert(tetrim,x,y)||
                collisionLeft(tetrim,x,y)||
                collisionRight(tetrim,x,y);
        }
        // adds tetrimino to board with centered at x, y on the board
        void addTetrimToBoard(pair<vector<vector<char>>, vector<int>>& tetrim,int x,int y){

            int m = tetrim.first.size(); 
            int n = tetrim.first[0].size(); 

            int cornerX = x-tetrim.second[1]-1;
            //int cornerY = y-tetrim.second[0]-1;

            // alt path to account for vanish zone 
            int cornerY = (y+16)-tetrim.second[0]-1; 

            //cout<<"["<<cornerY<<"]"<<"["<<cornerX<<"]"<<" ";
            // if board is empty just add tetrim[i][j] to board at board[cornerY+i][cornerX+j]
            // else 
            for(int i=0;i<m;i++){
                for(int j=0;j<n;j++){
                    if(board[cornerY+i][cornerX+j]==' '){
                        //cout<<"["<<cornerY+i<<"]"<<"["<<cornerX+j<<"]"<<tetrim.first[i][j]<<endl;
                        board[cornerY+i][cornerX+j] = tetrim.first[i][j];
                    }
                }
            }
        }
        bool canClearRow(int row){
            for(int j=0;j<board[0].size();j++){
                if(board[row][j]== ' ')return false; 
            }
            return true; 
        }
        void shiftRowsAboveOne(int row){
            int i=row; 
            while(i>0){
                board[i]=board[i-1];
                i--;
            }
            board[0] = vector<char>(10,' ');
        }
        // clears lines after placing tetrim at center x, y 
        void clearLines(pair<vector<vector<char>>, vector<int>>& tetrim, int x, int y){
            int m = tetrim.first.size(); 
            int n = tetrim.first[0].size(); 

            int cornerX = x-tetrim.second[1]-1;
            //int cornerY = y-tetrim.second[0]-1;

            // alt path to account for vanish zone 
            int cornerY = (y+16)-tetrim.second[0]-1; 
            
            // only rows cornerY to cornerY+block_height-1 can be cleared
            // start at row cornerY+m-1 and check if it can be cleared 
            // if so shift everything above cornerY+m-1 down by 1 
            // decerement block_height by 1 
            int currRow = cornerY+m-1; 
            int ceilRow = cornerY-1;
            while(currRow>ceilRow){
                // if currRow can be cleared shift everything above currRow 
                // by one and increment ceilRow by 1  
                // else decrement currRow by 1 
                if(canClearRow(currRow)){
                    shiftRowsAboveOne(currRow);
                    ceilRow++;
                }
                else{
                    currRow--;
                }
            }
        }
        bool gameOver(pair<vector<vector<char>>, vector<int>>& tetrim){
            int m = tetrim.first.size(); 
            int n = tetrim.first[0].size(); 

            int cornerX = 4-tetrim.second[1];
            //int cornerY = tetrim.second[0];

            // alt path to account for vanish zone 
            int cornerY = 16/*-tetrim.second[0]*/;


            for(int i=0;i<m;i++){
                for(int j=0; j<n;j++){
                    if(board[cornerY+i][cornerX+j]!=' ')return true; 
                }
            }
            return false; 
        }

};
int main(){
    initscr();
    keypad(stdscr, TRUE);    
    cbreak();
    curs_set(0);
    noecho();
    //WINDOW *gameWin = newwin(17,12,10,10);
    // window for neovim plugin 
    WINDOW *gameWin = newwin(17,12,0,0);
    refresh();
    keypad(gameWin,TRUE);
    wborder(gameWin, 0, 0, 0, ' ', 0, 0, 0, 0);


    //nodelay(gameWin,true);
    srand(chrono::high_resolution_clock::now().time_since_epoch().count());
    Tetris tetris;
    mutex mtx; 
    mutex windowMtx; 
    //queue<int> acts;
    // attach time when action was added to queue and only perform actions that are 
    // within threshold 
    deque<int> acts;
    // attaching time stamp to actions 
    // deque<pair<int,std::chrono::steady_clock::time_point>> acts;
    condition_variable render; 
    condition_variable inp; 
    std::atomic<bool> processAct(false); 
    atomic<bool> isRendering(false);
    int x = 5; 
    int y = 1; 
    int prevX = -1;
    int prevY = -1; 
    pair<vector<vector<char>>, vector<int>> prevTet; 
    pair<vector<vector<char>>, vector<int>> tet = tetris.getRandomTetrim(); 
    int gravityInt=600; 
    int frameInt = 16; 
    bool gameOver = false;
    //auto lastProc = std::chrono::steady_clock::now(); 
    //bool input = false;
    std::thread::id main_id = this_thread::get_id(); 
    auto sig_handle = [/*&*/&acts,&render,&processAct,&x,&y,&prevX,&prevY,
    &prevTet,&tet,&gravityInt,&gameOver,&tetris,&mtx,&gameWin,&windowMtx,&isRendering](){
        auto lastProc = std::chrono::steady_clock::now(); 
        while(!gameOver){
            bool grav = false; 
            bool moved = false; 
            int key; 
            // std::chrono::steady_clock::time_point actTime; 
            {
            std::unique_lock<mutex> lk(mtx);
            render.wait(lk,[&processAct]{return processAct.load();});
            //if(!gameOver){
            //bool grav = false; 
            //if(!acts.empty()){
                key = acts.front();
                // adding time stamp to actions
                // key = acts.front().first; 
                // actTime = acts.front().second;
                // using deque; 
                acts.pop_front();

                processAct= false;
            }
                //lk.unlock();
                // adding time stamp to actions 
                //auto currTime = std::chrono::steady_clock::now(); 
                //auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currTime-actTime).count(); 
                //auto timeSince = std::chrono::duration_cast<std::chrono::milliseconds>(lastProc-actTime).count(); 
                /*if(elapsedTime>0){
                    cout<<elapsedTime<<" "<<timeSince<<" "<<key<<endl; 
                }*/
                //if(timeSince>16) continue; 
                //lastProc = currTime; 
                if(key==KEY_UP || key== 'k'){
                    pair<vector<vector<char>>, vector<int>> next = tet; 
                    tetris.rotateTetrim(next); 
                    if(!tetris.collision(next,x,y+next.second[0])){
                        tet = next;
                        moved = true; 
                    }
                }
                else if(key == KEY_LEFT || key == 'h'){
                    if(!tetris.collisionLeft(tet,x,y+tet.second[0])){
                        x--; 
                        moved = true; 
                    }
                }
                else if(key == KEY_RIGHT || key == 'l'){
                    if(!tetris.collisionRight(tet,x,y+tet.second[0])){
                        x++;
                        moved = true; 
                    }
                }
                else if(key == KEY_DOWN || key == 'j'){
                    if(!tetris.collisionVert(tet,x,y+tet.second[0])){
                        y++;
                        moved = true; 
                    }
                }
                else if(key == 200){
                    moved = true; 
                    if(tetris.collisionVert(tet,x,y+tet.second[0])){
                        tetris.addTetrimToBoard(tet,x,y+tet.second[0]);
                        tetris.clearLines(tet,x,y+tet.second[0]);
                        x=5;
                        y=1;
                        prevX=-1; 
                        prevY=-1;
                        //acts = queue<int>(); 
                        // using deque 
                        acts = deque<int>(); 
                        // adding time stamp to actions 
                        // acts = deque<pair<int,std::chrono::steady_clock::time_point>>();
                        tet = tetris.getRandomTetrim(); 
                        gameOver = tetris.gameOver(tet);
                        tetris.renderBoard(gameWin);
                    }
                    else
                        grav=true;
                }
            //} 
            //cout<<"Rendering"<<endl;
            //mtx.lock();
            if(moved){
                /*if(prevX==x&&prevY==y)
                    cout<<prevX<<" "<<prevY<<endl;*/
                // {
                // lock_guard<mutex> lk(windowMtx);
                // isRendering = true; 
            //mtx.lock();
            //cout<<"Started rendering"<<endl; 
        
                if(prevX!=-1 && prevY!=-1){
                    tetris.clearTetrimFromCenter(gameWin, prevTet,prevX,prevY+prevTet.second[0]);
                //cout<< "Clearing: "<<prevX<<" "<<prevY+prevTet.second[0]<<endl;
                }
                tetris.renderTetrimFromCenter(gameWin,tet,x,y+tet.second[0]);
            //cout<< "Rendering: "<<x<<" "<<y+tet.second[0]<<endl;
                wrefresh(gameWin);
                // isRendering = false;
                // }
                prevX = x; 
                prevY = y; 
                prevTet = tet;
            }
            //mtx.unlock();
            //cout<<"Done Rendering"<<endl;
            //cout<<endl;
            //processAct = false;
            if(grav)
                y++;
            //mtx.unlock();
            
            // lastProc = std::chrono::steady_clock::now();
        }
    };
    WINDOW *inpPad=newpad(1,1);
    keypad(inpPad,TRUE);
    auto func = [&processAct,&render,&gameWin,&mtx,&acts,&gameOver,&windowMtx,&isRendering,&inpPad](){
        while(!gameOver){
            /*if(!isRendering.load()){
                {
                    lock_guard<mutex> windLk(windowMtx);
                    //wrefresh(gameWin);
                }*/
                int key = wgetch(inpPad);
                {
                    lock_guard<mutex> lk(mtx);
                    acts.push_back(key);
                    // adding time stamp to actions 
                    // std::chrono::steady_clock::time_point currTime = std::chrono::steady_clock::now(); 
                    // acts.push_back({key,currTime});
                    processAct = true;
                //cout<<"Added user input"<<endl;
                }
                render.notify_all();
            //}
        }
    };
    //std::thread input(func);
    /*int x=5; 
    int y=1;
    int prevX = -1; 
    int prevY = -1; 
    pair<vector<vector<char>>, vector<int>> prevTet; 
    pair<vector<vector<char>>, vector<int>> tet = tetris.getRandomTetrim();*/
    //gameOver = tetris.gameOver(tet); 
    auto lastGravTime = std::chrono::steady_clock::now();
    auto lastFrameTime = std::chrono::steady_clock::now(); 
    //cout<< typeid(lastGravTime).name()<<endl;
    //int gravityInt = 600; 
    //int frameInt = 16; 
    // create a thread that adds a special gravity action to the front of the queue 
    auto gravFunc = [&processAct,&render,&gravityInt,&mtx,&acts,&gameOver](){
        while(!gameOver){
            //mtx.lock(); 
            //acts.push_front(200);
            //mtx.unlock();
            // send signal for main thread to handle gravity action
            {
                lock_guard<mutex> lk(mtx);
                acts.push_front(200);
                // adding time stamp to actions 
                // std::chrono::steady_clock::time_point currTime = std::chrono::steady_clock::now(); 
                // acts.push_front({200,currTime});
                processAct = /*processAct ^ */true;
                //cout<<"Added grav event"<<endl;
            }
            render.notify_all();
            //Sleep(gravityInt);
            //cout<<"Grav Thread: "<<this_thread::get_id()<<endl;
            this_thread::sleep_for(chrono::milliseconds(gravityInt));
        }
    }; 
    //cout<<"Main Thread: "<<this_thread::get_id()<<endl;
    std::thread input(func);
    std::thread gravity(gravFunc);
    sig_handle();
    /*while(!gameOver){
        //int key = wgetch(gameWin);
        auto currTime = std::chrono::steady_clock::now();
        auto elapsedGravTime = std::chrono::duration_cast<std::chrono::milliseconds>(currTime-lastGravTime).count();
        auto elapsedFrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(currTime-lastFrameTime).count();

        int key;
        bool moved = false;
        while((key=wgetch(gameWin))!=ERR)
            acts.push(key);
        //performing act first 
        if(!acts.empty()){
            int key = acts.front(); 
            acts.pop(); 
            if(key==KEY_UP){
                pair<vector<vector<char>>, vector<int>> next = tet; 
                tetris.rotateTetrim(next);
                if(!tetris.collision(next,x,y+next.second[0])){
                    moved = true; 
                    tet = next; 
                }
            }
            if(key == KEY_LEFT){
                if(!tetris.collisionLeft(tet,x,y+tet.second[0])){ 
                    moved=true; 
                    x--; 
                }
            }
            if(key == KEY_RIGHT){
                if(!tetris.collisionRight(tet,x,y+tet.second[0])){
                    moved=true; 
                    x++;
                }
            }
            if(key == KEY_DOWN){
                if(!tetris.collisionVert(tet,x,y+tet.second[0]) && elapsedGravTime<gravityInt){
                    moved=true; 
                    y++;
                }
            }
        }
        if(elapsedGravTime >=gravityInt){
        if(tetris.collisionVert(tet,x,y+tet.second[0])){
            tetris.addTetrimToBoard(tet,x,y+tet.second[0]);
            tetris.clearLines(tet,x,y+tet.second[0]); 
            x=5;
            y=1;
            
            //rendering efficiently 
            prevX = -1; 
            prevY=-1;
            // update gameOver bool 
            acts = queue<int>();
            tet = tetris.getRandomTetrim(); 
            gameOver = tetris.gameOver(tet);
            // render Efficently 
            tetris.renderBoard(gameWin);
            //wrefresh(gameWin);
        }
        moved = true; 
        }

        if(elapsedFrameTime >= frameInt && (moved)){
        //tetris.clearTetrimFromCenter(gameWin,tet,x,y+tet.second[0]);
        //Sleep(1);
    
        // render efficiently 
        
        //tetris.clearTetrimFromCenter(gameWin,tet,x,y+tet.second[0]);
        // render efficiently 
        if(prevX!=-1 && prevY!=-1)
            tetris.clearTetrimFromCenter(gameWin,prevTet,prevX,prevY+prevTet.second[0]);
        tetris.renderTetrimFromCenter(gameWin,tet,x,y+tet.second[0]);
        wrefresh(gameWin);
        lastFrameTime = currTime; 
        prevX=x; 
        prevY = y;
        prevTet = tet; 
        }
        //lastFrameTime=currTime;
        //y++;
        if(elapsedGravTime >= gravityInt){
            y++;
            lastGravTime = currTime;
        }
        auto nextEventTime = min(lastFrameTime+ std::chrono::milliseconds(frameInt),
                lastGravTime+ std::chrono::milliseconds(gravityInt));
        auto sleepDur = std::chrono::duration_cast<std::chrono::milliseconds>(nextEventTime-std::chrono::steady_clock::now());
        if(sleepDur.count()>0)
            Sleep(sleepDur.count());
    }*/
    //tetris.renderBoard(gameWin);
    //wrefresh(gameWin);
    gameOver=true;
    cout<<"Game Over"<<endl;
    getch();
    input.join(); 
    gravity.join();
    delwin(gameWin);
    endwin();
}
