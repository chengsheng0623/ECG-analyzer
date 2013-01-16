#pragma once
#include <fstream>
#include <sstream>
#include "ModulesInterfaces.h"
#include "ModulesMethods.h"
//#define DEBUG
//#define DEBUG_SIGNAL
//#define DEBUG_SIGNAL_DETAILS
//#define USE_MOCKED_SIGNAL

using namespace std;

/**
 * Class RPeaksDetector provides set of method to detect R peaks in ECG signal.
 * @class RPeaksDetector
 */
class RPeaksDetector : public RPeaksModule
{
public:

	RPeaksDetector();
	~RPeaksDetector();

	void runModule(const ECGSignalChannel &, const ECGInfo &, ECGRs &);
	void setParams(ParametersTypes &);

  /**
  *  Execute R peaks detection
  *  @return false if detection cannot be executed
  *  @return true if detection is complete
  */
  bool detectRPeaks();

private:
  /**
  *  Information about detection status
  */
  bool rsDetected;

  /**
  *  Information about parameters
  */
  bool customParameters;

  /**
  *  Filtered signal from 'ECG_BASALINE'
  */
  ECGSignalChannel filteredSignal;

  /**
  *  R peaks vector
  */
  ECGRs * rsPositions;

  /**
  *  R peaks detection method
  */
  R_PEAKS_DETECTION_METHOD detectionMethod;
  
  /**
  *  PanTompkins movingh window lenght
  */
  int panTompkinsMovinghWindowLenght;

  /**
  *  PanTompkins thersold size
  */
  double panTompkinsThersold;

  /**
  *  PanTompkins R peaks method detection
  *  @param pointer to ECG signal
  */
  bool panTompkinsRPeaksDetection(ECGSignalChannel *signal);
  
  /**
  *  Hilbert R peaks method detection
  *  @param pointer to ECG signal
  */
  bool hilbertRPeaksDetection(ECGSignalChannel *signal);


  /**
  *  Filtered mocked signal
  */
  ECGSignalChannel mockedSignal;

  /*
  * Returns a part of filtered signal
  * This function is used only for tests!
  */
  ECGSignalChannel getMockedSignal();

};

/**
 * Implementation RPeaksDetectionException
 * This exception will be thrown if any problems occour during R peaks detection
 * @class RPeaksDetector
 */
class RPeaksDetectionException
{
public:
  RPeaksDetectionException(string cause)
  {
    this->cause = cause;
  }

private:
	string cause;

  virtual const string what() const throw()
  {
    return "Error during execution R preaks module cause: " + cause;
  }
};
