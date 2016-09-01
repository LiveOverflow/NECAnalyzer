#ifndef NEC_ANALYZER_SETTINGS
#define NEC_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class NECAnalyzerSettings : public AnalyzerSettings
{
public:
	NECAnalyzerSettings();
	virtual ~NECAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mPulseWidth;
	U32 mInverted;
	U32 mBigEndian;
	U32 mDisplayLevel;

protected:
	AnalyzerSettingInterfaceChannel		mInputChannelInterface;
	AnalyzerSettingInterfaceInteger		mPulseWidthInterface;
	AnalyzerSettingInterfaceNumberList		mInvertedInterface;
	AnalyzerSettingInterfaceNumberList		mBigEndianInterface;
	AnalyzerSettingInterfaceNumberList		mDisplayLevelInterface;
};

#endif //NEC_ANALYZER_SETTINGS
