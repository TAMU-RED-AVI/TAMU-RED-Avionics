// This program simulates data coming in from the Genesis rocket engine

#include <vector>
#include <math.h>
#include <Wire.h> // Needed for I2C
#define SERIAL_PORT Serial

// Define global variables
const int ledPin = 13;
float yaw = 0;
std::vector<float> loc_1{1,1,1};
std::vector<float> loc_2{4,4,1};
std::vector<float> loc_3{1,4,1};
std::vector<float> loc_4{4,1,1};

// Simulating random movement of tags
std::vector<float> update_loc(const std::vector<float>& loc){ 
  std::vector<float> nloc;
  float mx = 20; // Maximum movement
  float d = 5.08; // Dimension of simulated cube room
  
  for (const auto& el : loc){
    // Calculate random movement based on the element's proximity to the center
    float random_movement = random(-mx, mx) / 100.0;
    
    // Add random movement to the current element
    float nel = el + random_movement; // new element
    if (nel<0 || nel>d){
      nel = el - random_movement;
    }
    nloc.push_back(nel);
  }

  return nloc;
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // Simulates position of tags and anchors
  std::vector<std::vector<float>> anchors{{0, 0, 0}, {0, 5.08, 0}, {5.08, 0, 0}, {5.08, 5.08, 0}, {2.54, 0, 0}, {0, 2.54, 0}, {0, 0, 2.54}, {0, 5.08, 2.54}, {5.08, 0, 2.54}, {5.08, 5.08, 2.54}, {2.54, 0, 2.54}, {0, 2.54, 2.54}};
  loc_1 = update_loc(loc_1);
  loc_2 = update_loc(loc_2);
  loc_3 = update_loc(loc_3);
  loc_4 = update_loc(loc_4);
  // Initializes tag data vectors
  std::vector<float> tag_1{1,1,1,1,1,1,1,1,1,1,1,1,yaw};
  std::vector<float> tag_2{1,1,1,1,1,1,1,1,1,1,1,1,yaw};
  std::vector<float> tag_3{1,1,1,1,1,1,1,1,1,1,1,1,yaw};
  std::vector<float> tag_4{1,1,1,1,1,1,1,1,1,1,1,1,yaw};
  // Calculates simulated distances
  for (int i=0; i<12; i++){
    tag_1[i] = sqrt( (anchors[i][0]-loc_1[0])*(anchors[i][0]-loc_1[0]) + (anchors[i][1]-loc_1[1])*(anchors[i][1]-loc_1[1]) + (anchors[i][2]-loc_1[2])*(anchors[i][2]-loc_1[2]) );
    tag_2[i] = sqrt( (anchors[i][0]-loc_2[0])*(anchors[i][0]-loc_2[0]) + (anchors[i][1]-loc_2[1])*(anchors[i][1]-loc_2[1]) + (anchors[i][2]-loc_2[2])*(anchors[i][2]-loc_2[2]) );
    tag_3[i] = sqrt( (anchors[i][0]-loc_3[0])*(anchors[i][0]-loc_3[0]) + (anchors[i][1]-loc_3[1])*(anchors[i][1]-loc_3[1]) + (anchors[i][2]-loc_3[2])*(anchors[i][2]-loc_3[2]) );
    tag_4[i] = sqrt( (anchors[i][0]-loc_4[0])*(anchors[i][0]-loc_4[0]) + (anchors[i][1]-loc_4[1])*(anchors[i][1]-loc_4[1]) + (anchors[i][2]-loc_4[2])*(anchors[i][2]-loc_4[2]) );
    Serial.printf("%f, %f, %f, %f", tag_1[i], tag_2[i], tag_3[i], tag_4[i]);
    Serial.println();
  }
  // Creates matrix to be transmitted 
  std::vector<std::vector<float>> all_data;
  all_data.push_back(tag_1);
  all_data.push_back(tag_2);
  all_data.push_back(tag_3);
  all_data.push_back(tag_4);

  delay(200);
}
