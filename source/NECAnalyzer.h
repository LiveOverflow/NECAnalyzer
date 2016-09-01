#ifndef NEC_ANALYZER_H
#define NEC_ANALYZER_H

#include <Analyzer.h>
#include "NECAnalyzerSettings.h"
#include "NECAnalyzerResults.h"
#include "NECSimulationDataGenerator.h"

class ANALYZER_EXPORT NECAnalyzer : public Analyzer2
{
public:
	NECAnalyzer();
	virtual ~NECAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

	virtual void SetupResults();

protected: //vars
	NECAnalyzerSettings mSettings;
	std::auto_ptr<NECAnalyzerResults> mResults;
	AnalyzerChannelData* mNEC;

	NECSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	
	BitState _BIT_LOW;
	BitState _BIT_HIGH;
	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

enum STATE { IDLE, CHECKSTART_HIGH, CHECKSTART_LOW, DATA, ONE, ZERO, DONE };

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //NEC_ANALYZER_H
