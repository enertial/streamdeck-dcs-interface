//==============================================================================
/**
@file       MyStreamDeckPlugin.cpp

@brief      CPU plugin

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "MyStreamDeckPlugin.h"
#include <atomic>

#include "Common/ESDConnectionManager.h"
#include "Common/EPLJSONUtils.h"
#include "DcsInterfaceParameters.h"

class CallBackTimer
{
public:
	CallBackTimer() : _execute(false) {}

	~CallBackTimer()
	{
		if (_execute.load(std::memory_order_acquire))
		{
			stop();
		};
	}

	void stop()
	{
		_execute.store(false, std::memory_order_release);
		if (_thd.joinable())
			_thd.join();
	}

	void start(int interval, std::function<void(void)> func)
	{
		if (_execute.load(std::memory_order_acquire))
		{
			stop();
		};
		_execute.store(true, std::memory_order_release);
		_thd = std::thread([this, interval, func]() {
			while (_execute.load(std::memory_order_acquire))
			{
				func();
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			}
		});
	}

	bool is_running() const noexcept
	{
		return (_execute.load(std::memory_order_acquire) && _thd.joinable());
	}

private:
	std::atomic<bool> _execute;
	std::thread _thd;
};

MyStreamDeckPlugin::MyStreamDeckPlugin() : dcs_interface_(kDcsListenerPort, kDcsSendPort, kDcsSendIpAddress)
{
	mTimer = new CallBackTimer();
	mTimer->start(1, [this]() {
		this->CheckDcsState();
	});
}

MyStreamDeckPlugin::~MyStreamDeckPlugin()
{
	if (mTimer != nullptr)
	{
		mTimer->stop();

		delete mTimer;
		mTimer = nullptr;
	}
}

void MyStreamDeckPlugin::CheckDcsState()
{
	//
	// Warning: CheckDcsState() is running in the timer thread
	//
	if (mConnectionManager != nullptr)
	{
		std::vector<DcsIdValueUpdate> dcs_updates = dcs_interface_.get_next_dcs_update();
		mVisibleContextsMutex.lock();
		for (const DcsIdValueUpdate &dcs_update : dcs_updates)
		{
			mConnectionManager->SetTitle(dcs_update.dcs_id_value, dcs_update.context, kESDSDKTarget_HardwareAndSoftware);
		}
		mVisibleContextsMutex.unlock();
	}
}

void MyStreamDeckPlugin::KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID)
{
	// Nothing to do
}

void MyStreamDeckPlugin::KeyUpForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID)
{
	// Nothing to do
}

void MyStreamDeckPlugin::WillAppearForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID)
{
	// Remember the context
	mVisibleContextsMutex.lock();
	mVisibleContexts.insert(inContext);
	mVisibleContextsMutex.unlock();
}

void MyStreamDeckPlugin::WillDisappearForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID)
{
	// Remove the context
	mVisibleContextsMutex.lock();
	mVisibleContexts.erase(inContext);
	mVisibleContextsMutex.unlock();
}

void MyStreamDeckPlugin::DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo)
{
	// Nothing to do
}

void MyStreamDeckPlugin::DeviceDidDisconnect(const std::string &inDeviceID)
{
	// Nothing to do
}

void MyStreamDeckPlugin::SendToPlugin(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID)
{

	const auto event = EPLJSONUtils::GetStringByName(inPayload, "event");

	if (event == "updateRegisteredExportId")
	{
		const auto export_id_str = EPLJSONUtils::GetStringByName(inPayload, "value");

		// Unregister any prior monitoring.
		dcs_interface_.unregister_dcs_monitor(inContext);

		if (!export_id_str.empty())
		{
			dcs_interface_.register_dcs_monitor(std::stoi(export_id_str), inContext);
		}
	}
}
