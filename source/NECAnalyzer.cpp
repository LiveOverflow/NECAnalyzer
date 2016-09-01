#include "NECAnalyzer.h"
#include "NECAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

NECAnalyzer::NECAnalyzer()
:	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
}

NECAnalyzer::~NECAnalyzer()
{
	KillThread();
}


void NECAnalyzer::SetupResults()
{
	// reset the results
	mResults.reset(new NECAnalyzerResults(this, &mSettings));
	SetAnalyzerResults(mResults.get());

	// set which channels will carry bubbles
	mResults->AddChannelBubblesWillAppearOn(mSettings.mInputChannel);
}

void NECAnalyzer::WorkerThread()
{
	//mResults.reset( new NECAnalyzerResults( this, mSettings.get() ) );
	//SetAnalyzerResults( mResults.get() );
	//mResults->AddChannelBubblesWillAppearOn( mSettings.mInputChannel );

	mSampleRateHz = GetSampleRate();

	mNEC = GetAnalyzerChannelData( mSettings.mInputChannel );

	// skipping all low or all high unti reaching data
	if(mSettings.mInverted) {
		_BIT_LOW = BIT_HIGH;
		_BIT_HIGH = BIT_LOW;
	}

	if(!mSettings.mInverted) {
		_BIT_LOW = BIT_LOW;
		_BIT_HIGH = BIT_HIGH;
	}

	//if(mSettings.mInverted && mNEC->GetBitState() == _BIT_LOW){
	//	mNEC->AdvanceToNextEdge();
	//}

	double NECRateHz = 1000000.0 / double(mSettings.mPulseWidth);
	U32 samples_per_pulse = U32(double(mSampleRateHz) / NECRateHz);
	U32 samples_per_half_pulse = U32(0.5 * (double(mSampleRateHz) / NECRateHz));

	//mNEC->Advance( samples_per_half_pulse );

	STATE parseState = CHECKSTART_HIGH;
	STATE currentState;
	U64 decodedFrame = 0;
	U64 starting_frame;
	U64 finishing_frame;
	for(;;)
	{
		U8 data = 0;
		U8 mask = 1 << 7;
		U64 samplesUntilNextEdge;
		currentState = parseState;

		switch(parseState) {
			case IDLE:
				decodedFrame = 0;
				mNEC->AdvanceToNextEdge();
				starting_frame = mNEC->GetSampleNumber();
				mNEC->Advance( samples_per_half_pulse );
				//mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
				parseState = DATA;
				break;
			case CHECKSTART_HIGH:
				samplesUntilNextEdge = (mNEC->GetSampleOfNextEdge() - mNEC->GetSampleNumber())/samples_per_pulse;
				if(samplesUntilNextEdge >= 14 && samplesUntilNextEdge <= 16) {
					mNEC->AdvanceToNextEdge();
					//mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
					parseState = CHECKSTART_LOW;
				} else {
					mNEC->Advance(1);
					parseState = IDLE;
				}
				break;
			case CHECKSTART_LOW:
				samplesUntilNextEdge = (mNEC->GetSampleOfNextEdge() - mNEC->GetSampleNumber())/samples_per_pulse;
				if(samplesUntilNextEdge >= 7 && samplesUntilNextEdge <= 9) {
					mNEC->AdvanceToNextEdge();
					//mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
					mNEC->Advance( samples_per_half_pulse );
					//mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
					parseState = DATA;
				} else {
					mNEC->Advance(1);
					parseState = IDLE;
				}
				break;
			case DATA:
				mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
				if(mNEC->GetBitState()==_BIT_HIGH) {
					// 1. if we have a starting high bit move to the next pulse
					mNEC->Advance( samples_per_pulse );
					mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
					if(mNEC->GetBitState()==_BIT_LOW) {
						// 2. if the 2nd pulse is low we move on
						mNEC->Advance( samples_per_pulse );
						mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
						if(mNEC->GetBitState()==_BIT_LOW) {
							// if the 3rd pulse is low we move on
							mNEC->Advance( samples_per_pulse );
							mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
							if(mNEC->GetBitState()==_BIT_LOW) {
								// if the 4th pulse is low we move on
								mNEC->Advance( samples_per_pulse );
								mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );
								if(mNEC->GetBitState()==_BIT_HIGH) {
									// but if the third pulse was high we have a zero
									finishing_frame = mNEC->GetSampleNumber();
									parseState = ONE;
									if (mSettings.mDisplayLevel==1) {
										Frame frame;
										frame.mData1 = 1;
										frame.mFlags = 0;
										frame.mType = parseState;
										frame.mStartingSampleInclusive = starting_frame-samples_per_half_pulse;
										frame.mEndingSampleInclusive = finishing_frame-samples_per_half_pulse;
										mResults->AddFrame( frame );
										starting_frame = mNEC->GetSampleNumber();
									}
									if(mSettings.mBigEndian) {
										decodedFrame >>= 1;
										decodedFrame |= 0x8000000000000000;
									} else {
										decodedFrame <<= 1;
										decodedFrame |= 1;
									}
									break;
								} else {
									parseState = DONE;
									break;
								}
							}
						} else if(mNEC->GetBitState()==_BIT_HIGH) {
							// but if the third pulse was high we have a zero
							finishing_frame = mNEC->GetSampleNumber();
							parseState = ZERO;
							if (mSettings.mDisplayLevel==1) {
								Frame frame;
								frame.mData1 = 0;
								frame.mFlags = 0;
								frame.mType = parseState;
								frame.mStartingSampleInclusive = starting_frame-samples_per_half_pulse;
								frame.mEndingSampleInclusive = finishing_frame-samples_per_half_pulse;
								mResults->AddFrame( frame );
								starting_frame = mNEC->GetSampleNumber();
							}
							if(mSettings.mBigEndian) {
								decodedFrame >>= 1;
							} else {
								decodedFrame <<= 1;
							}
							break;
						}
					} 
				}
				// if this failed we go back to IDLE
				parseState = IDLE;
				break;
			case ONE:
			case ZERO:
				parseState = DATA;
				break;
			case DONE:
				if (mSettings.mDisplayLevel==0) {
					Frame frame;
					frame.mData1 = decodedFrame;
					frame.mFlags = 0;
					frame.mType = parseState;
					frame.mStartingSampleInclusive = starting_frame;
					frame.mEndingSampleInclusive = finishing_frame;
					mResults->AddFrame( frame );
				}
				parseState = IDLE;
				break;
		}

		// try to detect start

		//  1.000 Hz
		// is 1000 samples per second
		// meaning each width is 0,001s wide, or 1000000us
		// 
		/*
		if(parseState == ONE) {
			Frame frame;
			frame.mData1 = 1;
			frame.mFlags = 0;
			frame.mType = parseState;
			frame.mStartingSampleInclusive = starting_frame;
			frame.mEndingSampleInclusive = mNEC->GetSampleNumber();
			mResults->AddFrame( frame );
		} else if(parseState == ZERO) {
			Frame frame;
			frame.mData1 = 0;
			frame.mFlags = 0;
			frame.mType = parseState;
			frame.mStartingSampleInclusive = starting_frame;
			frame.mEndingSampleInclusive = mNEC->GetSampleNumber();
			mResults->AddFrame( frame );
		}
		*/
		mResults->CommitResults();
		
		ReportProgress( mNEC->GetSampleNumber() );

		
		//mNEC->Advance( samples_per_pulse );

		/*
		for( U32 i=0; i<8; i++ )
		{
			//let's put a dot exactly where we sample this bit:
			mResults->AddMarker( mNEC->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );

			if( mNEC->GetBitState() == BIT_HIGH )
				data |= mask;

			mNEC->Advance( samples_per_pulse );

			mask = mask >> 1;
		}*/


		//we have a byte to save. 
		/*
		
		
		*/
		
	}
}

bool NECAnalyzer::NeedsRerun()
{
	return false;
}

U32 NECAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 NECAnalyzer::GetMinimumSampleRateHz()
{
	return mSettings.mPulseWidth * 4;
}

const char* NECAnalyzer::GetAnalyzerName() const
{
	return "NEC/IR-Remote Analyzer";
}

const char* GetAnalyzerName()
{
	return "NEC/IR-Remote Analyzer";
}

Analyzer* CreateAnalyzer()
{
	return new NECAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}