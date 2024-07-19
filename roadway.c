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
#define ROADWAY_LENGTH 100
#define ROADWAY_WIDTH 2
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
  FILE *scenario = NULL;
  size_t linelen = 0;
  ssize_t readbytes = 0;
  char *schedule = NULL;
  if(argc != 2){
    fprintf(stderr, "Usage: %s scenario.txt\n", argv[0]);
    return EXIT_FAILURE;
  }
  // create a roadway of length 100
  // whereby each y is a cardinal direction N/S
  struct slot roadway[ROADWAY_LENGTH][ROADWAY_WIDTH];
  // ensure there is a bridge (b) of length 3(s)
  for(int x = 0; x < ROADWAY_LENGTH; x++){
    for(int y = 0; y < ROADWAY_WIDTH; y++){
      if(x >= 50 && x < 53)
        // bridge slot
        roadway[x][y].type = 'b';
      else
        // normal roadway
        roadway[x][y].type = 'r';
      pthread_mutex_init(&roadway[x][y].lock, NULL);
    }
  }
  scenario = fopen(argv[1], "r");
  if(scenario == NULL){
    fprintf(stderr, "failed to open: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
  
  readbytes = getline(&schedule, &linelen, scenario);
  fclose(scenario);
  printf("read: %s\n", schedule);
  return EXIT_SUCCESS;
}
