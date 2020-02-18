

 /** RF24Mesh_Example_Master.ino by TMRh20
  *
  * Note: This sketch only functions on -Arduino Due-
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include "RF24Mesh/RF24Mesh.h"
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <sys/time.h>

#define NUM_LAPS 6

RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);
RF24Network network(radio);
RF24Mesh mesh(radio,network);



int main(int argc, char** argv) {
  int car1_laps = 0, car2_laps = 0;
  int new_race = 1, dummy = 0;
  float final_time1, final_time2;
  char name1[50], name2[50];
  char error_msg[80];
  struct timespec start, end_car_1, end_car_2, time1, time2;

  FILE *Race_time_out;
	if ((Race_time_out = fopen("Race_timings.txt","a")) == NULL){
			sprintf(error_msg, "Error: Cannot open file to append timings.\n");
			perror(error_msg);
		}

  while(new_race == 1){
	  new_race = 0;
	  printf("Please enter the name of the Blue car driver:\n");
	  scanf("%s", name1);
	  printf("Please enter the name of the Black car driver:\n");
	  scanf("%s", name2);

	  printf("Enter 1 to start the race.\n");
	  scanf("%d", &dummy);

	//Setup the network
	// Set the nodeID to 0 for the master node
	mesh.setNodeID(0);
	// Connect to the mesh
	printf("start\n");
	mesh.begin();
	radio.printDetails();

	//Beep Beep Beep Go
	//system("aplay /home/pi/RF24Mesh/examples_RPi/mk64_racestart.wav");
	//system("aplay /home/pi/RF24Mesh/examples_RPi/mk64_countdown.wav");
	system("aplay /home/pi/RF24Mesh/examples_RPi/race_start.wav");

	printf("GO!\n");

	clock_gettime( CLOCK_MONOTONIC, &start); //Store starting clock value

while(car1_laps < NUM_LAPS || car2_laps < NUM_LAPS)
{

  // Call network.update as usual to keep the network updated
  mesh.update();

  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();


  // Check for incoming data from the sensors
  while(network.available()){
//    printf("rcv\n");
    RF24NetworkHeader header;
    network.peek(header);

    uint32_t dat=0;
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
      case 'M': network.read(header,&dat,sizeof(dat));
				if(dat == 1){
					car1_laps++;
					if(car1_laps == NUM_LAPS){
						clock_gettime( CLOCK_MONOTONIC, &end_car_1);	//Store end clock value
						printf("%s has finished!\n", name1);
					}else{
						printf("%s laps = %d\n", name1, car1_laps);
					}
				}else if(dat == 2){
					car2_laps++;
					if(car2_laps == NUM_LAPS){
						clock_gettime( CLOCK_MONOTONIC, &end_car_2);	//Store end clock value
						 printf("%s has finished!\n", name2);
					}else{
						printf("%s laps = %d\n", name2, car2_laps);
					}

				}else{
					printf("What car was that?\n");
				}
                //printf("Rcv car %u from 0%o\n",dat,header.from_node);
                 break;
      default:  network.read(header,0,0);
                printf("Rcv bad type %d from 0%o\n",header.type,header.from_node);
                break;
    }
  }
delay(2);


  }

 //Calculate the time taken
	if( (end_car_1.tv_nsec - start.tv_nsec) < 0){
		time1.tv_sec = end_car_1.tv_sec - start.tv_sec -1;
		time1.tv_nsec = 1000000000 + end_car_1.tv_nsec - start.tv_nsec;
	}else{
		time1.tv_sec = end_car_1.tv_sec - start.tv_sec;
		time1.tv_nsec = end_car_1.tv_nsec - start.tv_nsec;
	}

	printf("%s finished in %d.%d seconds\n", name1, (int)time1.tv_sec, (int)time1.tv_nsec);
	fprintf(Race_time_out, "%s, %d.%d\n", name1, (int)time1.tv_sec, (int)time1.tv_nsec);
	final_time1 = (float)time1.tv_sec + (float)time1.tv_nsec/1000000;

	if( (end_car_2.tv_nsec - start.tv_nsec) < 0){
		time2.tv_sec = end_car_2.tv_sec - start.tv_sec -1;
		time2.tv_nsec = 1000000000 + end_car_2.tv_nsec - start.tv_nsec;
	}else{
		time2.tv_sec = end_car_2.tv_sec - start.tv_sec;
		time2.tv_nsec = end_car_2.tv_nsec - start.tv_nsec;
	}

	printf("%s finished in %d.%d seconds\n", name2, (int)time2.tv_sec, (int)time2.tv_nsec);
	fprintf(Race_time_out, "%s, %d.%d\n", name2, (int)time2.tv_sec, (int)time2.tv_nsec);
	final_time2 = (float)time2.tv_sec + (float)time2.tv_nsec/1000000;

	if(final_time1 < final_time2){
		printf("%s is the winner!\n", name1);
	}else{
		printf("%s is the winner!\n", name2);
	}

 //Would you like to run another race?
	printf("Would you like to run another race? 1 for yes, 0 for no.\n");
	scanf("%d",&new_race);
	car1_laps = 0;
	car2_laps = 0;
	final_time1 = 0;
	final_time2 = 0;
 }

  fclose(Race_time_out);
return 0;
}
