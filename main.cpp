// Signal K application template file.
//
// This application demonstrates core SensESP concepts in a very
// concise manner. You can build and upload the application as is
// and observe the value changes on the serial port monitor.
//
// You can use this source file as a basis for your own projects.
// Remove the parts that are not relevant to you, and add your own code
// for external hardware libraries.

#include <memory>

#include "sensesp.h"
#include "sensesp/sensors/analog_input.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp_app_builder.h"
#include "sensesp/transforms/curveinterpolator.h"
#include "sensesp/transforms/moving_average.h"

using namespace sensesp;

// Curve Interpolater for fuel tank gauge - PIN 34
class TankInterpreter : public CurveInterpolator {
public:
TankInterpreter(String config_path = "")
: CurveInterpolator(NULL, config_path) {
// Populate a lookup table to translate readout GPIO34 to tank status
clear_samples();
// addSample(CurveInterpolator::Sample(GPIO34, tank status));
add_sample(CurveInterpolator::Sample(50, 0));
add_sample(CurveInterpolator::Sample(30000, 0.125));
add_sample(CurveInterpolator::Sample(80000, 0.25));
add_sample(CurveInterpolator::Sample(120000, 0.375));
add_sample(CurveInterpolator::Sample(250000, 0.5));
add_sample(CurveInterpolator::Sample(290000, 0.625));
add_sample(CurveInterpolator::Sample(320000, 0.75));
add_sample(CurveInterpolator::Sample(420000, 0.875));
add_sample(CurveInterpolator::Sample(510000, 1));
add_sample(CurveInterpolator::Sample(520000, 1.1));
}
};

// The setup function performs one-time application initialization.
void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("FW PACIENCIA")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    ->set_wifi_client("SV-Paciencia", "tahichepagatuwifi")
                    ->set_wifi_access_point("FW Paciencia", "tahichepagatuwifi")
                    //->set_sk_server("192.168.10.3", 80)
                    ->get_app();

  
  uint8_t FW1Pin = 36;
  AnalogInput* input = new AnalogInput(FW1Pin, 5000);
   
  //uint8_t FWpin = 34;
  //AnalogInput* input2 = new AnalogInput(FWpin, 5000);
  
  input->attach([input]() {
    debugD("Analog input value TK 1 36: %f", input->get()); 
  });
  //input2->attach([input2]() {
  //  debugD("Analog input value TK 2 34: %f", input2->get()); 
  //});
  

  input->connect_to(new MovingAverage(10, 1))
  ->connect_to(new TankInterpreter("/freshWater Tank/curve"))
  ->connect_to(new SKOutputFloat("tanks.freshWater.0.currentLevel" , "/freshWater Tank/sk_path"));
  //input2->connect_to(new MovingAverage(10, 1))
  //->connect_to(new TankInterpreter("/freshWater Tank/curve"))
  //->connect_to(new SKOutputFloat("tanks.freshWater.1.currentLevel" , "/freshWater Tank/sk_path"));
  
  
  
  
  
  
}

void loop() { event_loop()->tick(); }
