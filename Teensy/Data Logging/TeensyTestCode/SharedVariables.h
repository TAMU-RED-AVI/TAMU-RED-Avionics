#ifndef SHARED_VARIABLES_H
#define SHARED_VARIABLES_H

#include <ArduinoJson.h>
#include <vector>

enum SensorType {
  THERMOCOUPLE,
  PRESSURE_TRANSDUCER,
  LOAD_CELL,
  SOLENOID
};

struct SensorData {
  SensorType type;
  virtual void addToJson(JsonObject& json) = 0;
  virtual ~SensorData() = default; 
};

struct ThermocoupleData : public SensorData {
  std::vector<double> temperatures;

  ThermocoupleData() {
    type = THERMOCOUPLE;
  }

  void addToJson(JsonObject& json) override {
    json["type"] = type;
    JsonArray tempArr = json.createNestedArray("temperatures");
    for (double temp : temperatures) {
      tempArr.add(temp);
    }
  }
};

struct PressureTransducerData : public SensorData {
  std::vector<double> pressures;

  PressureTransducerData() {
    type = PRESSURE_TRANSDUCER;
  }

  void addToJson(JsonObject& json) override {
    json["type"] = type;
    JsonArray pressArr = json.createNestedArray("pressures");
    for (double press : pressures) {
      pressArr.add(press);
    }
  }
};

struct LoadCellData : public SensorData {
  std::vector<double> forces;

  LoadCellData() {
    type = LOAD_CELL;
  }

  void addToJson(JsonObject& json) override {
    json["type"] = type;
    JsonArray forceArr = json.createNestedArray("forces");
    for (double force : forces) {
      forceArr.add(force);
    }
  }
};

struct SolenoidData : public SensorData {
  std::vector<double> percentOpen;

  SolenoidData() {
    type = SOLENOID;
  }

  void addToJson(JsonObject& json) override {
    json["type"] = type;
    JsonArray POArr = json.createNestedArray("percentOpen");
    for (double PO : percentOpen) {
      POArr.add(PO);
    }
  }
};

#endif // SHARED_VARIABLES_H
