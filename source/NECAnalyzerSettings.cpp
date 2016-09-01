#include "NECAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


NECAnalyzerSettings::NECAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mPulseWidth( 550 ),
	mInverted( 1 ),
	mBigEndian( 0 ),
	mDisplayLevel( 0 )
{
	mInputChannelInterface.SetTitleAndTooltip( "NEC", "Standard NEC/IR-Remote Analyzer" );
	mInputChannelInterface.SetChannel( mInputChannel );

	mPulseWidthInterface.SetTitleAndTooltip( "Pulse width µs", "Pulse burst width (550µs) - for example bit 1 divided by 4." );
	mPulseWidthInterface.SetMax( 6000000 );
	mPulseWidthInterface.SetMin( 1 );
	mPulseWidthInterface.SetInteger( mPulseWidth );

	mInvertedInterface.SetTitleAndTooltip( "Digital Interpretation", "" );
	mInvertedInterface.AddNumber( 0, "+5V is HIGH, 0V is LOW", "" );
	mInvertedInterface.AddNumber( 1, "+5V is LOW, 0V is HIGH", "" );
	mInvertedInterface.SetNumber( mInverted );


	mDisplayLevelInterface.SetTitleAndTooltip( "Display Level", "" );
	mDisplayLevelInterface.AddNumber( 0, "Whole Word", "" );
	mDisplayLevelInterface.AddNumber( 1, "Single Bits", "" );
	mDisplayLevelInterface.SetNumber( mDisplayLevel );


	mBigEndianInterface.SetTitleAndTooltip( "Endianness for Word", "" );
	mBigEndianInterface.AddNumber( 0, "Most Significant Bit First", "" );
	mBigEndianInterface.AddNumber( 1, "Least Significant Bit First", "" );
	mBigEndianInterface.SetNumber( mBigEndian );

	AddInterface( &mInputChannelInterface );
	AddInterface( &mPulseWidthInterface );
	AddInterface( &mInvertedInterface );
	AddInterface( &mDisplayLevelInterface );
	AddInterface( &mBigEndianInterface );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "NEC", false );
}

NECAnalyzerSettings::~NECAnalyzerSettings()
{
}

bool NECAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface.GetChannel();
	mPulseWidth = mPulseWidthInterface.GetInteger();
	mInverted = mInvertedInterface.GetNumber();
	mBigEndian = mBigEndianInterface.GetNumber();
	mDisplayLevel = mDisplayLevelInterface.GetNumber();

	ClearChannels();
	AddChannel( mInputChannel, "NEC/IR-Remote Analyzer", true );

	return true;
}

void NECAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface.SetChannel( mInputChannel );
	mPulseWidthInterface.SetInteger( mPulseWidth );
	mInvertedInterface.SetNumber( mInverted );
	mBigEndianInterface.SetNumber( mBigEndian );
	mDisplayLevelInterface.SetNumber( mDisplayLevel );
}

void NECAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mPulseWidth;
	text_archive >> mInverted;
	text_archive >> mBigEndian;
	text_archive >> mDisplayLevel;

	ClearChannels();
	AddChannel( mInputChannel, "NEC/IR-Remote Analyzer", true );

	UpdateInterfacesFromSettings();
}

const char* NECAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mPulseWidth;
	text_archive << mInverted;
	text_archive << mBigEndian;
	text_archive << mDisplayLevel;

	return SetReturnString( text_archive.GetString() );
}
