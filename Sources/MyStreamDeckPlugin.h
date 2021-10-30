//==============================================================================
/**
@file       MyStreamDeckPlugin.h

@brief      Based off of Elgato's CPU plugin example

@copyright  (c) 2018, Corsair Memory, Inc.
                        This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#include "Common/ESDBasePlugin.h"
#include "DcsInterface/DcsInterface.h"
#include "StreamdeckContext/StreamdeckContextFactory.h"
#include <mutex>
#include <unordered_map>

class CallBackTimer;

class MyStreamDeckPlugin : public ESDBasePlugin
{
  public:
    MyStreamDeckPlugin();
    virtual ~MyStreamDeckPlugin();

    void KeyDownForAction(const std::string &inAction,
                          const std::string &inContext,
                          const json &inPayload,
                          const std::string &inDeviceID) override;
    void KeyUpForAction(const std::string &inAction,
                        const std::string &inContext,
                        const json &inPayload,
                        const std::string &inDeviceID) override;

    /**
     * The 'willAppear' event is the first event a key will receive, right before it gets showed on your Stream Deck
     * and/or in Stream Deck software.
     * This event is a good place to setup your plugin and look at current settings (if any), which are embedded in the
     * events payload.
     */
    void WillAppearForAction(const std::string &inAction,
                             const std::string &inContext,
                             const json &inPayload,
                             const std::string &inDeviceID) override;
    void WillDisappearForAction(const std::string &inAction,
                                const std::string &inContext,
                                const json &inPayload,
                                const std::string &inDeviceID) override;

    void DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo) override;
    void DeviceDidDisconnect(const std::string &inDeviceID) override;

    void DidReceiveGlobalSettings(const json &inPayload) override;

    void SendToPlugin(const std::string &inAction,
                      const std::string &inContext,
                      const json &inPayload,
                      const std::string &inDeviceID) override;

  private:
    /**
     * @brief Periodic function which continually updates Streamdeck button contexts according to DCS game state.
     */
    void UpdateFromGameState();

    /**
     * @brief Helper function to extract connection settings from global settings
     *
     * @param global_settings Json object of global settings received from Streamdeck.
     * @return DcsConnectionSettings
     */
    DcsConnectionSettings get_connection_settings(const json &global_settings);

    std::mutex mVisibleContextsMutex;
    std::unordered_map<std::string, std::unique_ptr<StreamdeckContext>> mVisibleContexts = {};

    CallBackTimer *mTimer;
    std::optional<DcsInterface> dcs_interface_ = std::nullopt;
    StreamdeckContextFactory streamdeck_context_factory;
};
