#include "NECAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "NECAnalyzer.h"
#include "NECAnalyzerSettings.h"
#include <iostream>
#include <fstream>

NECAnalyzerResults::NECAnalyzerResults( NECAnalyzer* analyzer, NECAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

NECAnalyzerResults::~NECAnalyzerResults()
{
}

void NECAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	if(frame.mType==DONE) {
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 64, number_str, 128 );
	} else {
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 1, number_str, 128 );
	}
	AddResultString( number_str);
}

void NECAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 64, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void NECAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	if(frame.mType==DONE) {
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 64, number_str, 128 );
	} else {
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 1, number_str, 128 );
	}
	AddTabularText( number_str );
}

void NECAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void NECAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}