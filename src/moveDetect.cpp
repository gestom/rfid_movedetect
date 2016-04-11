#include <stdlib.h>
#include "ros/ros.h"
#include "CPhaseModelSet.h"
#include <std_msgs/String.h>

CPhaseModelSet models;

using namespace std;

float *periods = NULL;
ros::NodeHandle *n;
FILE *file;
float alpha = 0.2;
float err = 0;

ros::Publisher detected,ready,moved;

void rfidCallback(const std_msgs::String &msg)
{
	int ptr[5];
	char message[100];
	strcpy(message,msg.data.c_str());

	//parse message: not using strtok because not thread-safe
	int pos = 0;
	int parse = 0;
	ptr[0] = 0;
	while (message[pos]!=0 && parse < 4)
	{
		if (message[pos] == ':') 
		{
			message[pos] = 0;
			ptr[++parse] = pos+1;
		} 
		pos++;
	}

	//get data and add them to the TAG model
	int strength = 	atoi(&message[ptr[2]]);
	int phase = 	atoi(&message[ptr[3]]);
	int frequency = atoi(&message[ptr[4]]);
	float result = models.addMeasurement(&message[ptr[1]],strength,frequency,phase);
	//printf("Tag: %s Strength: %i Freq: %i Phase: %i\n",&message[ptr[1]],strength,frequency,phase);

	//does the model report anything ?
	ETagEvent event = models.readEvent(message);
	std_msgs::String out;
	out.data = message; 
 
	//then publish it
	if (event == TAG_EVENT_DETECTED)   detected.publish(out);
	if (event == TAG_EVENT_READY)   ready.publish(out);
	if (event == TAG_EVENT_MOVED)   moved.publish(out);
}

int main(int argc,char* argv[])
{
	file = fopen("logpico.txt","w");
	ros::init(argc, argv, "movedetect");
	n = new ros::NodeHandle();
	ros::Subscriber rfid = n->subscribe("/rfid/rfid_detect", 1, rfidCallback);
	detected = n->advertise<std_msgs::String>("/rfid/detected", 1000);
	ready = n->advertise<std_msgs::String>("/rfid/model_ready", 1000);
	moved = n->advertise<std_msgs::String>("/rfid/moved", 1000);
	
	while (ros::ok()){
		ros::spinOnce();
		usleep(30000);
	}
	models.print();
	fclose(file);
	ros::shutdown();
	delete n;
	return 0;
}
