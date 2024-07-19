#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

struct vehicle {
  unsigned int weight;
  char type;
  unsigned int id;
};

struct slot {
  pthread_mutex_t lock;
  struct vehicle occupant;
  char type;
};

int const bridge_capacity = 1200;
int bridge_occupancy = 0;
/*
 * create a vehicle with a 50:50 probability
 */
struct vehicle create_vehicle(){
  struct vehicle cv;
  srand((int) getpid());
  int flip = rand() % 100;
  if(flip > 50) {
    // van
    cv.type = 'v';
    cv.weight = 300;
  } else {
    // car
    cv.type = 'c';
    cv.weight = 200;
  }
  return cv;
}
  

int main(int argc, char *argv[]){
  // create a roadway of length 100
  // whereby each y is a cardinal direction N/S
  struct slot roadway[100][2];
  // ensure there is a bridge (b) of length 3(s)
  for(int x = 0; x < 100; x++){
    for(int y = 0; y < 2; y++){
      if(x >= 50 && x < 53)
        // bridge slot
        roadway[x][y].type = 'b';
      else
        roadway[x][y].type = 'r';
      pthread_mutex_init(&roadway[x][y].lock, NULL);
    }
  }
  return EXIT_SUCCESS;
}
