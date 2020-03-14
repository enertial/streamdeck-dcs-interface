//==============================================================================
/**
@file       MyStreamDeckPlugin.h

@brief      CPU plugin

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "Common/ESDBasePlugin.h"
#include "DcsInterface.h"
#include "StreamdeckContext.h"
#include <mutex>
#include <unordered_map>

class CallBackTimer;

class MyStreamDeckPlugin : public ESDBasePlugin
{
public:
	MyStreamDeckPlugin();
	virtual ~MyStreamDeckPlugin();

	void KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;
	void KeyUpForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;

	/** 
     	  * The 'willAppear' event is the first event a key will receive, right before it gets
     	  * showed on your Stream Deck and/or in Stream Deck software.
     	  * This event is a good place to setup your plugin and look at current settings (if any),
     	  * which are embedded in the events payload.
     	  */
	void WillAppearForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;
	void WillDisappearForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;

	void DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo) override;
	void DeviceDidDisconnect(const std::string &inDeviceID) override;

	void SendToPlugin(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;

private:
	void UpdateFromGameState();

	std::mutex mVisibleContextsMutex;
	std::unordered_map<std::string, StreamdeckContext> mVisibleContexts = {};

	CallBackTimer *mTimer;
	DcsInterface dcs_interface_;
};
