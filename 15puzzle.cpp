#include <stdio.h> 
#include <map>
#include <iostream>   
#include <stdlib.h> 
#include <time.h> 
#include <algorithm>
#include <iostream>
#include <bitset>
#include <vector>
#include <unordered_map>
#include<cmath>
#include <queue>

#define LL  long long
#define ULL unsigned long long

using namespace std;

ULL goal = 1147797409030816545;

struct state {
    unsigned long long board;
    int heuristic; //h
    int move_numer; //g
    struct state* prev;

    bool operator<(const state &o) const{
        if((heuristic + move_numer) == (o.heuristic + o.move_numer))
            return heuristic > o.heuristic;
        return (heuristic + move_numer) > (o.heuristic + o.move_numer);
    }
};

priority_queue <state> pq;
state start;
unordered_map <ULL, state> odwiedzone;

//* len_Of_Solution -> store number of visited states */
int len_Of_Solution = 0;
//* g -> store lenght of path */
int g = 0;

int CountnewStates = 0;

int heuristicManhattan(ULL mask);
int heuristicNotInplace(ULL mask);
void createNewState(state st, int index, int index2);

//* Function to change array[] to bitMask */
ULL toMask(int arr[]) {
    ULL mask = 0;
    for(int i = 0; i < 16; i++){
        mask |= (LL)arr[i] << i*4; 
    }
    return mask;
}

//* true if we achive finish node or false if not*/
bool isEnd(ULL num){
    if(num == goal)return true;

    return false;
}

//* Function to print bitMask as int numbers */
void printArray(ULL mask){ 

    for(int i = 0; i < 16; i++){
        cout<<((mask >> i*4) & 0x0f) << " ";
        if(i%4 == 3)cout<<"\n";
    }
}

//* Function to create Random permutation of starting node with 0 in right down corner*/ 
void createRandomPermutation(int permutation[], int n){
    
    srand (time(NULL));
    for(int i = 0; i < n-2; i++){

        int j = i + rand()%(n-1 - i);
        swap(permutation[i], permutation[j]);
    }

}

//* Returns possition of given number in bitMask*/
int getPosition(ULL mask, int number) {
    for(int i = 0; i < 16; i++) {
        if(((mask >> i*4) & 0x0f) == number){
            return i;
        } 
    }
    return -1;
}

//* Looking for possible moves and evoke createNewState*/
void getMoves(state st) {
    ULL mask = st.board;
    int index = getPosition(mask, 0);
    int x = 0, y = 0;

    x = index/4;
    y = index%4;
    
    if( x > 0) {
        createNewState(st, index, index - 4);
    }
    if( y < 3) {
        createNewState(st, index, index + 1);
    }
    if( y > 0) {
        createNewState(st, index, index - 1);
    }
    if( x < 3) {
        createNewState(st, index, index + 4);
    }
            
}

//* Swap index in bitMask (used to create new states from node)*/
ULL swapIndex(ULL mask, int index, int index2) {

    ULL mask2 = 0;
    unsigned int pom = ((mask >> index*4) & 0x0f);
    unsigned int pom2 = ((mask >> index2*4) & 0x0f);

    for(int i = 0; i < 16; i++){
        if(i == index){
            mask2 |= (LL)pom2 << i*4;
        } else if( i == index2){
            mask2 |= (LL)pom << i*4;
        } else {
            mask2 |= (LL)((mask >> i*4) & 0x0f) << i*4;
        }
    }
    return mask2; 
}

//* Method to create new state and add it to map and queue*/
void createNewState(state st, int index, int index2) {
    ULL mask = st.board;
    ULL newmask = swapIndex(mask,index,index2);

    if(st.prev != NULL){
        if(newmask == st.prev->board)return;
    }

    state newState;
    newState.prev = &st;
    newState.board = newmask;
    newState.heuristic = heuristicManhattan(newmask);
    newState.move_numer = st.move_numer + 1;

    if(odwiedzone.find(newState.board) == odwiedzone.end()){
        CountnewStates++;
        pq.push(newState);
        odwiedzone.insert({newState.board, newState});
    } 
    else {
        if(odwiedzone[mask].move_numer >= newState.move_numer && odwiedzone[mask].board != start.board ) {
            newState.move_numer = odwiedzone[mask].move_numer;
            odwiedzone[mask] = newState;
            pq.push(newState);
        }
    }

}

//* Checking if starting node is possible to solve*/ 
bool isSolvable(int permutation[], int n){

    int inversions = 0;
    int whereBlank = 0;

    for (int i = 0; i < n*n-1; i++)
    {
        for (int j = i + 1; j < n*n; j++)
        {
            if (permutation[j] && permutation[i] && permutation[i] > permutation[j])
                inversions++;
        }
    }

    whereBlank = 1;

    if((inversions%2 == 0 && whereBlank%2 == 1) || (inversions%2 == 1 && whereBlank%2 == 0))
        return true;

    return false;
}

//* Returning value of NotInPlace tiles as heuristic for specific node */
int heuristicNotInplace(ULL mask){
    int sum = 0;
    
    for(int i = 0; i < 16; i++){
         if(((mask >> i*4) & 0x0f) == i + 1){
                sum++;
            }
            else if(((mask >> i*4) & 0x0f) == 0 &&  i + 1 == 15 )sum++;
    }
    return 16 - sum;
}

//* Returning value of manhattan heuristic method in specific node*/
int heuristicManhattan(ULL mask){

    int sum = 0;
    for( int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            int index = getPosition(mask, i+j*4);
            int index2 = getPosition(goal, i+j*4);
            sum += abs(index2%4-index%4) + abs(index2/4-index/4);
        }
    }
    
    return sum;
    
}

//* main method to solve game*/
void solve(ULL mask){

    g = 0;
    start.heuristic = heuristicManhattan(mask);
    start.move_numer = g;
    start.prev = NULL;
    start.board = mask;
    odwiedzone.insert({start.board, start});
    pq.push(start);

    while(!pq.empty()) {

        state pom = pq.top();

        if(isEnd(pom.board)){
            cout<<"END! \nLength : "<< pom.move_numer <<endl;
            return;
        }

        pq.pop();

        getMoves(pom);

        len_Of_Solution++;
    }


} 

int main(){

    int n = 4;
    int permutation[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};

    while(1){
        createRandomPermutation(permutation, n*n);
        if(isSolvable(permutation, n)) break;
    }

    // int test[16] = {  11, 8, 10, 3, 0, 1, 7, 12, 5, 13, 9, 6, 14, 15, 4, 2};
    // int test2[16] = { 6, 2, 13, 8, 4, 10, 7, 15, 0, 3, 12, 9, 5, 1, 14, 11};
    // int test3[16] = { 10, 2, 11, 3, 14, 5, 8, 4, 15, 1, 7, 13, 0, 12, 9, 6};
    // int test4[16] = { 1, 9, 0, 4, 2, 7, 8, 14, 13, 10, 5, 15, 3, 12, 11, 6};
    ULL rem = toMask(permutation);
    printArray(rem);

    cout<<"Manhattan : "<< heuristicManhattan(rem)<<endl;
    cout<<"NotInPlace : "<< heuristicNotInplace(rem)<<endl;
    cout<<"\n";

    solve(rem);
    cout<<"petla while " << len_Of_Solution<<endl;
    cout<<"States : "<< CountnewStates<<endl;

}