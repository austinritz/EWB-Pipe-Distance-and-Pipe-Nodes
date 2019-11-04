#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <cmath>
using namespace std;

// float haversine(float slat,float slong,float elat,float elong){
// //calculates distance between two points
//
//     // map to radians and convert to numpy array
//     float slat = conv(slat);
//     float slong = conv(slong);
//     float elat = conv(elat);
//     float elong = conv(elong);
//
//     // haversine formula
//     float d = np.sin((elat - slat)/2)**2 + np.cos(slat) * np.cos(elat) * np.sin((elong - slong)/2)**2;
//     float arc = np.arcsin(np.sqrt(d));
//     int r = 6372.8;
//     float dist = 2 * r * arc;
//
//     return dist;
// }

//struct to store the data from final_tracks.csv
struct track_data{
  string name;
  double lat;
  double longi;
  double elev;
};

float radians(float x){
  return x * M_PI / 180.0;
}
float degrees(float x){
  return x * (180.0/M_PI);
}

//this will read in the csv file final_tracks.csv and put it into structs for each line.
void read_record(vector<track_data> &data)
{
    int vectorcount = 0;
    // File pointer
    fstream fin;
    // Open an existing file
    fin.open("final_tracks.csv", ios::in);
    vector<string> row;
    string line, word, temp;
    int count = 0;
    while (getline(fin, temp)) {
        row.clear();
        stringstream ss(temp);
        // read every column data of a row and
        // store it in a string variable, 'word'
        while (getline(ss, word, ',')) {
            row.push_back(word);
            //row[0],[1],[2],and [5] are the ones we need
        }
        if (count > 0){
          track_data temp1;//creates a struct object to push
          data.push_back(temp1);//creates the new struct in vector that we will use
          //add the data from the getline and stringstream into the new struct
          data[vectorcount].name = row[0];
          data[vectorcount].lat = stod(row[1]);
          data[vectorcount].longi = stod(row[2]);
          data[vectorcount].elev = stod(row[5]);
          vectorcount++;
        }
        count ++;
    }
    cout << "how many entries: " << vectorcount << endl;
}
//isNode will itterate through the data vector and find points of interest based on the difference in bearings from points x->x+1 vs x+1->x+2
float isNode(vector<track_data> &data, vector<track_data> &nodes, float maxDif){
  int over5 = 0;
  int over10 = 0;

  for (int i = 0; i < data.size(); i++){
    //first check to see if the consecutive points all belong to the same set of gps data, which is organized by name
    if (data[i].name == data[i+1].name && data[i+1].name == data[i+2].name){
      //note: lat2/lon2 is used for the bearing found between data[i] and data[i+1] as well as data[i+1] and data[i+2]
      //i decided to just assign the variables for the 2 calculations all together, but i seperated the 2 calculations
      float lat = data[i].lat;
      float lat2 = data[i+1].lat;
      float lat3 = data[i+2].lat;
      float lon = data[i].longi;
      float lon2 = data[i+1].longi;
      float lon3 = data[i+2].longi;
      float teta1 = radians(lat);
      float teta2 = radians(lat2);
      float teta3 = radians(lat3);
      float delta1 = radians(lat2-lat);
      float delta2 = radians(lon2-lon);
      float delta3 = radians(lat3-lat2);
      float delta4 = radians(lon3-lon2);
      //finds brng for data[i] and data[i+1]
      float y = sin(delta2) * cos(teta2);
      float x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
      float brng = atan2(y,x);
      brng = degrees(brng);// radians to degrees
      //finds brng2 for data[i+1] and data[i+2]
      float y2 = sin(delta3) * cos(teta3);
      float x2 = cos(teta2)*sin(teta3) - sin(teta2)*cos(teta3)*cos(delta3);
      float brng2 = atan2(y2,x2);
      brng2 = degrees(brng2);// radians to degrees
      float brngdif;
      cout.precision(20);
      if (abs(brng2-brng) > 180) brngdif = 360.0 - abs(brng2-brng);//accounts for cases when brngdif is about 300 or so, we just want the relative bearning dif
      else brngdif = abs(brng2-brng);
//if the change in bearing is greater than the one we decided, it adds the middle node to the potential node vecotre
      if (brngdif >= maxDif) nodes.push_back(data[i+1]);
    }
  }
}

int count_tracks(vector<track_data> &data){
  //function will count how many different names there are, aka how many different count_tracks
  int index = 0;
  int count = 0;
  while (index < data.size()){
    if (data[index].name != data[index+1].name){
      count++;
      cout << data[index].name << " vs " << data[index+1].name << endl;
    }
    index++;
  }
  return count;
}

int main (){
  vector<track_data> data;
  vector<track_data> nodes10d;
  vector<track_data> nodes20d;
  vector<track_data> nodes30d;
  read_record(data);
  cout.precision(20);
  //int count = count_tracks(data);
  cout <<"Total Data Points: " << data.size() << endl;
  isNode(data, nodes10d, 10.0);
  cout << "potential nodes found (10 degree dif): " << nodes10d.size() << endl;
  isNode(data, nodes20d, 20.0);
  cout << "potential nodes found (20 degree dif): " << nodes20d.size() << endl;
  isNode(data, nodes30d, 30.0);
  cout << "potential nodes found (30 degree dif): " << nodes30d.size() << endl;
  return 0;
}
