/*Author: Blake Lofgreen
 * Started: 01/31/2024
 * Finished: 02/06/2024
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>


using namespace std;

//class to store lla info
class lla{
public:
    lla(long double epoch_i,long double lat_i, long double lon_i, long double alt_i){
        epoch = epoch_i;
        latitude = lat_i;
        longitude = lon_i;
        altitude = alt_i;
    }
    long double get_time(){
        return epoch;
    }
    long double get_lat(){
        return latitude;
    }
    long double get_lon(){
        return longitude;
    }
    long double get_alt() {
        return altitude;
    }
private:
    long double epoch;
    long double latitude;
    long double longitude;
    long double altitude;

};

//class to store ecefs data
class ecef{
public:
    ecef(long double time_i, long double x_i, long double y_i, long double z_i, long double xv_i, long double yv_i, long double zv_i){
        time = time_i;
        x = x_i;
        y = y_i;
        z = z_i;
        xv = xv_i;
        yv = yv_i;
        zv = zv_i;
    }
    double get_time(){
        return time;
    }
    long double get_x(){
        return x;
    }
    long double get_y(){
        return y;
    }
    long double get_z(){
        return z;
    }
    long double get_xv(){
        return xv;
    }
    long double get_yv(){
        return yv;
    }
    long double get_zv(){
        return zv;
    }
    void set_xv(long double xvi){
        xv=xvi;
    }
    void set_yv(long double yvi){
        yv=yvi;
    }
    void set_zv(long double zvi){
        zv=zvi;
    }

private:
    long double time;
    long double x;
    long double y;
    long double z;
    long double xv;
    long double yv;
    long double zv;
};

//vector of classes
vector<lla> llas;
vector<ecef> ecefs;

//function to read data fromn the csv file and store it in the llas vector
void LLaPopulate(){
    string file = "";// data sheet here
    ifstream data;
    data.open(file, ios::in);
    if(!data.is_open()){
        cout << "file not open"<<endl;
    }
    vector<long double> row;
    string line, word, temp;
while(getline(data,line)) {
    row.clear();

    stringstream s(line);
    cout <<fixed<<setprecision(15);
    while (getline(s, word, ',')) {

        long double v = stold(word);
            row.push_back(v);

    }

    llas.push_back(lla(row[0], row[1], row[2], row[3]));
}
data.close();
}

//runs conversion from lla to ecef and stores it in ecef vector
void convert(){
    long double a = 6378137.0;
    long double f = 1/298.257223563;
    long double b = (1-f)* a;

    long double e = sqrt((pow(a,2) - pow(b,2))/ pow(a, 2));

   for(int i= 0; i<llas.size(); i++) {
        double long altinMeters = (llas.at(i).get_alt() *1000);
        double long rad_lat = (llas.at(i).get_lat() * M_PI)/180;
        double long rad_lon= (llas.at(i).get_lon() * M_PI)/180;

        long double n = (a/(sqrt(1-pow(e,2)* pow(sin(rad_lat),2))));
        long double x = (n+altinMeters) * cos(rad_lat) * cos(rad_lon);
        long double y = ((n + altinMeters) * (cos(rad_lat)) * (sin(rad_lon)));
        long double z = ((n*(1-pow(e,2)) + altinMeters)* (sin(rad_lat)));

        ecefs.push_back(ecef(round(llas.at(i).get_time()),x,y,z,0,0,0));

    }
}

//takes in the xyz position deltas and divides it by the delta in time
void cal_velocity(){

    for(int i=0; i< ecefs.size(); i++){

        if(i!=0){

            long double xvi = (ecefs.at(i).get_x() - ecefs.at(i-1).get_x()) / (ecefs.at(i).get_time() - ecefs.at(i-1).get_time());
            long double yvi = (ecefs.at(i).get_y() - ecefs.at(i-1).get_y()) / (ecefs.at(i).get_time() - ecefs.at(i-1).get_time());
            long double zvi = (ecefs.at(i).get_z() - ecefs.at(i-1).get_z()) / (ecefs.at(i).get_time() - ecefs.at(i-1).get_time());

            ecefs.at(i).set_xv(xvi);
            ecefs.at(i).set_yv(yvi);
            ecefs.at(i).set_zv(zvi);
        }

    }
}

//this is the main ui, which will allow the use to put in time frames to recieve the velocity.
void specifics(){
    int time;
    bool check = true;
while(check){
    cout<< "Please enter a time(or enter 0 to close the program): ";
    cin >> time;
    if(time ==0){
        check = false;
    }else if(time < ecefs.at(0).get_time() || time > ecefs.at(500).get_time()){
        cout<<"Time entered is out of scope"<<endl;
        cout<<endl;
    }else{
    for (int i = 0; i < ecefs.size(); i++) {
        if (time == ecefs.at(i).get_time()){
            cout<< "Time: "<<ecefs.at(i).get_time()<<endl;
            cout<<"X Velocity: " << ecefs.at(i).get_xv()<<endl;
            cout<<"Y Velocity: " << ecefs.at(i).get_yv()<<endl;
            cout<<"Z Velocity: " << ecefs.at(i).get_zv()<<endl;

        }
    }
    for (int y = 0; y < ecefs.size(); y++) {

        if (time > ecefs.at(y).get_time() && time < ecefs.at(y + 1).get_time()) {
            long double interx = ((ecefs.at(y + 1).get_x() - ecefs.at(y).get_x()) / 2) + ecefs.at(y).get_x();
            long double intery = ((ecefs.at(y + 1).get_y() - ecefs.at(y).get_y()) / 2) + ecefs.at(y).get_y();
            long double interz = ((ecefs.at(y + 1).get_z() - ecefs.at(y).get_z()) / 2) + ecefs.at(y).get_z();

            long double dx = (interx - ecefs.at(y).get_x()) / (time - ecefs.at(y).get_time());
            long double dy = (intery - ecefs.at(y).get_y()) / (time - ecefs.at(y).get_time());
            long double dz = (interz - ecefs.at(y).get_z()) / (time - ecefs.at(y).get_time());

            cout << "Time: " << time << endl;
            cout << "X Velocity: " << dx << endl;
            cout << "Y Velocity: " << dy << endl;
            cout << "Z Velocity: " << dz << endl;
        }
    }
    }
    }
}


int main() {
    llas.reserve(501); //sets a certain amount of memory to the lla vector
    ecefs.reserve(501);//sets a certain amount of memory to the ecef vector
    LLaPopulate();
    convert();
    cal_velocity();
    llas.clear(); //frees up memory
    specifics();

    // code for testing the data produced by code
    /*for (int i = 0; i < ecefs.size(); ++i) {
        cout<<"Time: "<<ecefs.at(i).get_time()<<endl;
        cout<<"X: "<< ecefs.at(i).get_x()<<endl;
        cout<<"Y: "<< ecefs.at(i).get_y()<<endl;
        cout<<"Z: "<< ecefs.at(i).get_z()<<endl;
        cout<<"X Velocity: "<< ecefs.at(i).get_xv()<<endl;
        cout<<"Y Velocity: "<< ecefs.at(i).get_yv()<<endl;
        cout<<"z Veloctiy: "<< ecefs.at(i).get_zv()<<endl;

    }*/



    return 0;
}

