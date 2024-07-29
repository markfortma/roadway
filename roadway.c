#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>
#include<pthread.h>

struct vehicle {
  unsigned int weight;
  char type;
  unsigned int id;
};
static int ident = 0;

struct slot {
  pthread_mutex_t lock;
  struct vehicle occupant;
  char type;
};

int const bridge_capacity = 1200;
int bridge_occupancy = 0;
pthread_mutex_t bridge_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bridge_occupancy_cv = PTHREAD_COND_INITIALIZER;
#define ROADWAY_LENGTH 20
#define ROADWAY_BRIDGE_INDEX 9
#define ROADWAY_BRIDGE_END (ROADWAY_BRIDGE_INDEX + 3)
#define ROADWAY_WIDTH 2

struct scenario {
  unsigned int north;
  unsigned int south;
  unsigned int delay;
};

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
    cv.id = ident++;
  } else {
    // car
    cv.type = 'c';
    cv.weight = 200;
    cv.id = ident++;
  }
  return cv;
}

void parse_scenario(struct scenario *sarr, int sarr_length, const char *schedule){
#define TOKENS " :/()[]"
  unsigned int pos = 0;
  char *token = strtok(schedule, TOKENS);
  unsigned int count = atoi(token);
  float ratio = 0.0;
  while((token = strtok(NULL, TOKENS))){
    if(strcmp(token, "S") == 0){
      // the next two tokens are our north/south
      token = strtok(NULL, TOKENS);
      ratio = atof(token);
      sarr[pos].north = ratio * count;
      token = strtok(NULL, TOKENS);
      ratio = atof(token);
      sarr[pos].south = ratio * count;
    }else if(strcmp(token, "DELAY") == 0){
      // the next token is a delay
      token = strtok(NULL, TOKENS);
      sarr[pos++].delay = atoi(token);
    }else if(isdigit(token[0])){
      // the token is a digit
      count = atoi(token);
    }
  }
#undef TOKENS
}

void run_scenario(struct slot road[][2], int roadlen, int roadw, char direction){  
  int track = 0;
  if(direction == 's')
    track = 1;
  struct vehicle occupant = create_vehicle();
  for(int s = 0; s < roadlen; s++){
    if(road[s][track].type == 'b' && road[s - 1][track].type == 'r'){
      // transition onto bridge
      printf("vehicle(%c:%i) arriving bridge at index %i\n", occupant.type, occupant.id, s);
      while(bridge_occupancy + occupant.weight >  bridge_capacity)
	pthread_cond_wait(&bridge_occupancy_cv, &bridge_mutex);
      pthread_mutex_lock(&road[s][track].lock);
      pthread_mutex_lock(&bridge_mutex);
      road[s][track].occupant = occupant;
      bridge_occupancy += occupant.weight;
      sleep(1);
      pthread_mutex_unlock(&bridge_mutex);
      pthread_mutex_unlock(&road[s][track].lock);
    } else if(road[s][track].type == 'r' && road[s - 1][track].type == 'b'){
      // transition off bridge
      printf("vehicle(%c:%i) leaving bridge at index %i\n", occupant.type, occupant.id, s);
      pthread_mutex_lock(&road[s][track].lock);
      pthread_mutex_lock(&bridge_mutex);
      bridge_occupancy -= occupant.weight;
      pthread_mutex_unlock(&bridge_mutex);
      pthread_cond_signal(&bridge_occupancy_cv);
      road[s][track].occupant = occupant;
      sleep(1);
      pthread_mutex_unlock(&road[s][track].lock);
    } else {
      //printf("vehicle(%c:%i) at index %i\n", occupant.type, occupant.id, s);
      // start on road way
      pthread_mutex_lock(&road[s][track].lock);
      road[s][track].occupant = occupant;
      sleep(1);
      pthread_mutex_unlock(&road[s][track].lock);
    } 
  }
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
  struct scenario schedules[5];
  int schlength = sizeof(schedules) / sizeof(schedules[0]);
  // ensure there is a bridge (b) of length 3(s)
  for(int x = 0; x < ROADWAY_LENGTH; x++){
    for(int y = 0; y < ROADWAY_WIDTH; y++){
      if(x >= ROADWAY_BRIDGE_INDEX && x < ROADWAY_BRIDGE_END)
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
  parse_scenario(schedules, schlength, schedule);

  // free the line allocated
  free(schedule);

  for(int sched = 0; sched < schlength; sched++){
    for(int n = 0; n < schedules[sched].north; n++){
      ;
    }
    for(int s = 0; s < schedules[sched].south; s++){
      ;
    }
    sleep(schedules[sched].delay);
  }

  return EXIT_SUCCESS;
}
