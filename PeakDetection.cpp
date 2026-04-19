/*
  MIT License

  Copyright (c) 2019 Leandro César

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

// Acknowledgment: https://stackoverflow.com/questions/22583391/peak-peaknal-detection-in-realtime-timeseries-data

#include "PeakDetection.h"

using namespace std;
const int DEFAULT_LAG = 32;
const int DEFAULT_THRESHOLD = 2;
const double DEFAULT_INFLUENCE = 0.5;
const double DEFAULT_EPSILON = 0.01;

PeakDetection::PeakDetection() {
  index = 0;
  lag = DEFAULT_LAG;
  threshold = DEFAULT_THRESHOLD;
  influence = DEFAULT_INFLUENCE;
  EPSILON = DEFAULT_EPSILON;
  peak = 0;
}

PeakDetection::~PeakDetection() {
  delete data;
  //delete avg;
  //delete std;
  delete sqdata;
}

void PeakDetection::begin() {
  data = (double *)malloc(sizeof(double) * (lag + 1));
  //avg = (double *)malloc(sizeof(double) * (lag + 1));
  //std = (double *)malloc(sizeof(double) * (lag + 1));
  for (int i = 0; i < lag; ++i) {
    data[i] = 0.0;
    //avg[i] = 0.0;
    //std[i] = 0.0;
    sqdata[i]=0.0;
  }
}

void PeakDetection::begin(int lag, int threshold, double influence) {
  this->lag = lag;
  this->threshold = threshold;
  this->influence = influence;
  data = (double *)malloc(sizeof(double) * (lag));
  //avg = (double *)malloc(sizeof(double) * (lag));
  //std = (double *)malloc(sizeof(double) * (lag);
  avg=0.0;
  std=0.0;
  sqdata = (double *)malloc(sizeof(double) * (lag));
  SUM=0.0;
  SUMOFSQUARES=0.0;
  for (int i = 0; i < lag; ++i) {
    data[i] = 0.0;
    //avg[i] = 0.0;
    //std[i] = 0.0;
    sqdata[i]=0.0;
  }
}

void PeakDetection::setEpsilon(double epsilon) {
  this->EPSILON = epsilon;
}

double PeakDetection::getEpsilon() {
  return(EPSILON);
}

//void PeakDetection::add(double newSample) {
double PeakDetection::add(double newSample) {
  peak = 0;
  int i = index % lag; //current index
  int j = (index + 1) % lag; //next index
  //double deviation = newSample - avg[i];
  double deviation = newSample - avg;
  SUM-=data[j]; // subtract the data to be replaced from the sum
  SUMOFSQUARES-=sqdata[j];
  //if (deviation > threshold * std[i]) {
  if (deviation > threshold * std) {
    data[j] = influence * newSample + (1.0 - influence) * data[i];
    peak = 1;
  }
  //else if (deviation < -threshold * std[i]) {
  else if (deviation < -threshold * std) {
    data[j] = influence * newSample + (1.0 - influence) * data[i];
    peak = -1;
  }
  else
    data[j] = newSample;
  sqdata[j] = data[j]*data[j];
  SUM+=data[j]; //add the new data to the sum
  SUMOFSQUARES+=sqdata[j];
    //avg[j] = getAvg(j, lag);
    //std[j] = getStd(j, lag);
    avg = getAvg(j, lag);
    std = getStd(j, lag);
  index++;
  if (index >= 16383) //2^14
    index = lag + j;
  //return(std[j]);
  return(std);
}

double PeakDetection::getFilt() {
  int i = index % lag;
  //return avg[i];
  return avg;
}

int PeakDetection::getPeak() {
  return peak;
}

double PeakDetection::getAvg(int start, int len) {
  //double x = 0.0;
  //for (int i = 0; i < len; ++i)
  //  x += data[(start + i) % lag];
  //return x / len;
  return SUM/lag;
}

double PeakDetection::getPoint(int start, int len) {
  //double xi = 0.0;
  //for (int i = 0; i < len; ++i)
  //  xi += data[(start + i) % lag] * data[(start + i) % lag];
  //return xi / len;
  return SUMOFSQUARES/lag;
}

double PeakDetection::getStd(int start, int len) {
  double x1 = getAvg(start, len);
  double x2 = getPoint(start, len);
  double powx1 = x1 * x1;
  double std = x2 - powx1;
  if (std > -EPSILON && std < EPSILON)
    if(std < 0.0)
        return(-EPSILON);
    else
        return(EPSILON);
  else
    return sqrt(x2 - powx1);
}
