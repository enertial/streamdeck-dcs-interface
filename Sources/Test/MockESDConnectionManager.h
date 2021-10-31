// Copyright 2021 Charles Tytler

#pragma once

#include "ElgatoSD/ESDConnectionManager.h"

class MockPlugin : public ESDBasePlugin
{
  public:
    MockPlugin() {}
    virtual ~MockPlugin() {}

    void SetConnectionManager(ESDConnectionManager *inConnectionManager) { mConnectionManager = inConnectionManager; }

    virtual void KeyDownForAction(const std::string &inAction,
                                  const std::string &inContext,
                                  const json &inPayload,
                                  const std::string &inDeviceID)
    {
    }
    virtual void KeyUpForAction(const std::string &inAction,
                                const std::string &inContext,
                                const json &inPayload,
                                const std::string &inDeviceID)
    {
    }

    virtual void WillAppearForAction(const std::string &inAction,
                                     const std::string &inContext,
                                     const json &inPayload,
                                     const std::string &inDeviceID)
    {
    }
    virtual void WillDisappearForAction(const std::string &inAction,
                                        const std::string &inContext,
                                        const json &inPayload,
                                        const std::string &inDeviceID)
    {
    }

    virtual void DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo) {}
    virtual void DeviceDidDisconnect(const std::string &inDeviceID) {}

    virtual void DidReceiveGlobalSettings(const json &inPayload) {}

    virtual void SendToPlugin(const std::string &inAction,
                              const std::string &inContext,
                              const json &inPayload,
                              const std::string &inDeviceID)
    {
    }
};

// Create mock version of ESDConnectionManager for testing.
// #define UNIT_TEST
// const int kESDSDKTarget_HardwareAndSoftware = 0;

class MockESDConnectionManager : public ESDConnectionManager
{
  public:
    // MOCK_METHOD(void, SetState, (int state, std::string context), (override));
    // MOCK_METHOD(void, SetTitle, (std::string title, std::string context, int flag), (override));
    MockESDConnectionManager() : ESDConnectionManager(0, "", "", "", &plugin_) {}
    // ESDBasePlugin *inPlugin) {}

    void SetState(int inState, const std::string &inContext)
    {
        context_ = inContext;
        state_ = inState;
    }

    void SetImage(const std::string &inBase64ImageString, const std::string &inContext, ESDSDKTarget inTarget) {}

    void SetTitle(const std::string &inTitle, const std::string &inContext, ESDSDKTarget inTarget)
    {
        context_ = inContext;
        title_ = inTitle;
    }

    // Only in mock class:
    void clear_buffer()
    {
        context_ = "";
        state_ = 0;
        title_ = "";
    }

    MockPlugin plugin_;
    std::string context_ = "";
    int state_ = 0;
    std::string title_ = "";
};