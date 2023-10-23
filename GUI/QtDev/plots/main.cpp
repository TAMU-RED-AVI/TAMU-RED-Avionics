#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <Eigen/Dense>
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <random>
#include <sys/stat.h>
#include "RootFinder.h"
#define BUF_SIZE 1024
#define WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::runtime_error;
using namespace Eigen;


// Definitions
const int num_tags = 4;
const int num_data_pts = 13;
const double inch_to_meter = 0.0254;

PositionMatrix anchor_positions; // Anchor Positions in x,y,z
Matrix<double, 4, 3> Tags_previous
    {
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0}
    };

Matrix<double, 4, 4> previous_IMU_data
    {
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0}
    };
int displayIterations = 0;

// Socket setup
//const int num_ports = 1;
//int ports[num_ports] = { 1234 };
//SOCKET socks[num_ports];
//sockaddr_in serverAddrs[num_ports];
//int recvLens[num_ports];
//sockaddr_in clientAddrs[num_ports];
//int clientAddrLens[num_ports] = { sizeof(sockaddr_in) };
//char buffer[1024];

// Serial setup
HANDLE hSerial;
DCB dcbSerialParams = { 0 };
std::string completeMessage;
byte buffer[1024];
size_t MAX_MESSAGE_SIZE = 750;


// Function to get dimensions of room from user input
double* getDimensions()
{
    double length;
    double width;

    cout << "Room Setup Menu" << endl;
    cout << "Input the length of the room in inches and press enter: ";
    cin >> length;


    cout << "Input the width of the room in inches and press enter: ";
    cin >> width;

    cout << endl;

    double* result = new double[2];
    result[0] = (length * inch_to_meter);
    result[1] = (width * inch_to_meter);

    return result;
}


// Function to get anchor locations within room from user input
PositionMatrix getAnchors()
{
    // Define the number of anchor points
    const int numAnchors = 12;

    // Ask for dimensions of the anchor points in inches
    cout << "Input the x, y, and z position of " << numAnchors << " anchor points in inches with spaces in between each and press enter: " << endl;

    // Initialize the result matrix with correct size
    PositionMatrix result(numAnchors, 3);

    double x, y, z;

    // Loop through each anchor point and ask for its dimensions
    for (int i = 0; i < numAnchors; ++i)
    {
        cout << "Anchor Point " << i + 1 << ": ";
        cin >> x >> y >> z;

        // Convert each anchor point from inches to meters and insert it into the result
        result.row(i) << x * inch_to_meter, y* inch_to_meter, z* inch_to_meter;
    }

    return result;
}


// Function to read dimensions from a text file
void readDimensions(string filename, double& length, double& width, PositionMatrix& anchor_positions)
{
    std::ifstream inputFile;
    inputFile.open(filename);

    const int numberOfAnchors = 12;

    if (inputFile.is_open())
    {
        string line;
        getline(inputFile, line);
        sscanf_s(line.c_str(), "Room Dimensions: %lf, %lf", &length, &width);

        // Resize the anchor_positions matrix to the correct size
        anchor_positions.resize(numberOfAnchors, 3);

        for (int i = 0; i < numberOfAnchors; i++)
        {
            Vector3d point;
            getline(inputFile, line);
            sscanf_s(line.c_str(), "Anchor %*d: %lf, %lf, %lf", &point[0], &point[1], &point[2]);

            // Set the i-th row of anchor_positions to point
            anchor_positions.row(i) = point;
        }

        cout << "Dimensions read from " << filename << endl;
    }
    else
    {
        cout << "Error: Unable to open file" << endl;
    }

    inputFile.close();
}


// Function to save dimensions to a text file
void saveDimensions(double length, double width, PositionMatrix anchor_positions, string filename)
{
    std::ofstream outputFile;
    outputFile.open(filename);

    if (outputFile.is_open())
    {
        outputFile << "Room Dimensions: " << length << ", " << width << endl;

        // Loop through the anchor positions and write each one to the file
        for (int i = 0; i < anchor_positions.rows(); ++i)
        {
            Vector3d point = anchor_positions.row(i);
            outputFile << "Anchor " << i + 1 << ": " << point[0] << ", " << point[1] << ", " << point[2] << endl;
        }

        cout << "Dimensions saved to " << filename << endl;
    }
    else
    {
        cout << "Error: Unable to open file" << endl;
    }

    outputFile.close();
}


/*****************************************
*********** GENERAL FUNCTIONS  ***********
*****************************************/

// Incoming Data Processing
Matrix<double, num_data_pts, 1> dataProcessing(string str)
{

    vector<double> tempData;

    size_t start = str.find("[") + 1;
    size_t end = str.find(",");

    while (str.find("]", start) != string::npos) {
        tempData.push_back(stof(str.substr(start, end - start)));
        start = end + 1;
        end = str.find(",", start);
        if (end == string::npos) {
            end = str.find("]", start);
        }
    }

    // Initialize Eigen vector with standard vector
    VectorXd data = Map<VectorXd, Unaligned>(tempData.data(), tempData.size());

    return data;
}


float readSensorData() {

    /***********************************
   *********** ROOM CONFIG ***********
   ***********************************/

    // Room Dimensions
    double length;
    double width;

    // Time Tracking
    std::chrono::high_resolution_clock::time_point time_start;

    // Open the file for writing
    std::ofstream outFile("Tracked Location", std::ios::app);

    // Check if the file was opened successfully
    if (!outFile) {
        std::cerr << "Error opening file." << endl;
    }

    /**************************************
   *********** DATA PROCESSING ***********
   **************************************/

    // get the current timestamp
    auto now = std::chrono::system_clock::now();
    // convert the timestamp to a time_t object
    time_t timestamp = std::chrono::system_clock::to_time_t(now);
    // set the desired timezone
    tm timeinfo;
    localtime_s(&timeinfo, &timestamp);

    int hours = timeinfo.tm_hour;
    int minutes = timeinfo.tm_min;
    int seconds = timeinfo.tm_sec;

    // Defines variables for location coordinates
    Matrix<double, 4, 3> Tags_position;

    double yaw;

    MatrixXd tag_data(num_tags, num_data_pts);
    VectorXd distances(12);

    string completeMessage = "";
    byte buffer[1024];
    size_t MAX_MESSAGE_SIZE = 750;

    bool reading = false; // represents if we are currently reading a message

    DWORD bytesRead;
    char c = '\0';

//    if (!PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
//        // Handle the error, if needed.
//        throw runtime_error("Failed to purge the buffers");
//    }

    while (c != '<' || reading){
        if (!ReadFile(hSerial, &c, 1, &bytesRead, NULL) || bytesRead == 0) {
            // Handle error
            cout << "Bytes Read: " << bytesRead << endl;
            throw runtime_error("Bytes Not Correctly Read, In Loop");
        };

        if (c == '<')
        {
            // Start of a message
            completeMessage.clear();
            reading = true;
        }
        else if (c == '>' && reading)
        {
            // cout << completeMessage << endl;
            size_t start = 1, end = 0;
            for (int u = 0; u < num_tags; ++u)
            {
                // Iterating through each vector within the 2D matrix
                end = completeMessage.find("]", start);
                tag_data.row(u) = dataProcessing(completeMessage.substr(start, end - start + 1)).transpose();
                start = end + 2;
            }
            // Reset the completeMessage for the next read
            completeMessage.clear();
            reading = false;
            break;
        }
        else
        {
            completeMessage += c;
        }
    }

    // Prints out data received
    // cout << tag_data << endl;

    // Loop through the tags
    for (int j = 0; j < num_tags; ++j) {

        distances = tag_data.row(j).head(12);
        yaw = tag_data(j, 12);

        // Call the multilateration function
        Vector3d Tags_current;
        LMsolution result;
        try
        {
            result = RootFinder::LevenbergMarquardt(anchor_positions, distances, Tags_previous.row(j).transpose());
            Tags_current = result.solution;
        }
        catch (_exception& e)
        {
            Tags_current = Tags_previous.row(j).transpose();
            throw runtime_error("Exception in LM");
        }

        if (result.exit_type == ExitType::AboveMaxIterations)
        {
            throw runtime_error("Max Iterations in LM");
        }

        if (result.exit_type == ExitType::BelowDynamicTolerance)
        {
            cout << "DYNAMIC TOLERANCE EXIT WARNING. Iterations: " << result.iterations << endl;
        }

        distances.setZero();

        if (abs(Tags_current.norm() - Tags_previous.row(j).norm()) < 0.25)
        {
            Tags_position.row(j) = Tags_current;
        }
        else if (displayIterations > 0)
        {
            Tags_position.row(j) = Tags_previous.row(j);
            cout << "PREVIOUS USED" << endl;
        }
        else
        {
            Tags_position.row(j) = Tags_current;
        }


        // writes the location data to the console
        cout << "x: " << Tags_position.row(j)[0] << ", y: " << Tags_position.row(j)[1] << ", z: " << Tags_position.row(j)[2] << ", yaw: " << yaw << ", Time: " << hours << ":" << minutes << ":" << seconds << endl;

    }
    cout << endl;

    return yaw;

}

// Close the file
//outFile.close();

void MainWindow::realtimeDataSlot()
{
    static QTime timeStart = QTime::currentTime();
    // calculate two new data points:
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds
    // std::cout << key << std::endl;
    static double lastPointKey = 0;
    // get data from sensors
    float orientation = readSensorData();
    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
        // add data to lines:
        ui->customPlot1->graph(0)->addData(key, qSin(key)+std::rand()/(double)RAND_MAX*1*qSin(key/0.4));
        ui->customPlot1->graph(1)->addData(key, orientation);
        // rescale value (vertical) axis to fit the current data:
//        ui->customPlot1->graph(0)->rescaleValueAxis();
//        ui->customPlot1->graph(1)->rescaleValueAxis(true);
        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot1->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot1->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
                .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
                .arg(ui->customPlot1->graph(0)->data()->size()+ui->customPlot1->graph(1)->data()->size())
            , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}

/***********************************
*********** MAIN PROGRAM ***********
***********************************/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    /***********************************
    *********** ROOM CONFIG ***********
    ***********************************/

    // Room Dimensions
    double length;
    double width;

    // Time Tracking
    std::chrono::high_resolution_clock::time_point time_start;

    cout << "Tracking Startup..." << endl;
    string read_filename = "dev";

    // Checks for existing file
    struct stat buffer;
    if (stat(read_filename.c_str(), &buffer) != 0) {

        cout << "File does not exist, try again";
        return 0;
    }

    readDimensions(read_filename, length, width, anchor_positions);

    cout << anchor_positions.size() << endl;

    const double screen_scale = 150.0;
    double screen_width = width * screen_scale;
    double screen_height = length * screen_scale;

    cout << "Room dimensions (m): " << length << " x " << width << endl;
    cout << "Screen size (pixels): " << screen_height << " x " << screen_width << endl;
    cout << "Anchor points (m): " << endl;
    for (int i = 0; i < anchor_positions.rows(); ++i) {
        cout << "  Point " << (i + 1) << ": " << anchor_positions.row(i) << endl;
    }

    // Open the file for writing
    std::ofstream outFile("Tracked Location", std::ios::app);

    // Check if the file was opened successfully
    if (!outFile) {
        std::cerr << "Error opening file." << endl;
        return 1;
    }

    /*************************************
    *********** SERIAL STARTUP ***********
    *************************************/

    hSerial = CreateFile(TEXT("COM3"), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        // Handle error
        throw runtime_error("Connection Not Init Properly");
    }
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    if (!PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
        // Handle the error, if needed.
        throw runtime_error("Failed to purge the buffers");
    }

    DWORD bytesRead;
    char c;

    if (!ReadFile(hSerial, &c, 1, &bytesRead, NULL) || bytesRead == 0) {
        // Handle error
        cout << "Bytes Read: " << bytesRead << endl;
        throw runtime_error("Bytes Not Correctly Read, In Main");
    };

    cout << "Serial Start Up Complete" << endl;

    return a.exec();

}
