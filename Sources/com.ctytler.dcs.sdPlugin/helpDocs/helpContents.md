# DCS Interface Streamdeck Plugin
##### *Developed by Charles Tytler, 2020.*

- [DCS Interface Streamdeck Plugin](#dcs-interface-streamdeck-plugin)
        - [*Developed by Charles Tytler, 2020.*](#developed-by-charles-tytler-2020)
- [Description of Settings](#description-of-settings)
  - [DCS Command (on Button Press) Settings](#dcs-command-on-button-press-settings)
    - [Button ID](#button-id)
    - [Device ID](#device-id)
    - [Send Value while Pressed](#send-value-while-pressed)
    - [Send Value while Released](#send-value-while-released)
    - [NOTE: More Complex Send Values](#note-more-complex-send-values)
  - [DCS Function Monitor (Change Button Image) Settings](#dcs-function-monitor-change-button-image-settings)
    - [DCS ID](#dcs-id)
    - [Show 2nd State Image When](#show-2nd-state-image-when)
    - [Value](#value)
  - [DCS Function Text Update Monitor](#dcs-function-text-update-monitor)
    - [DCS ID](#dcs-id-1)

# Description of Settings

In order to use the DCS Interface buttons on Streamdeck you must use ID values that the aircraft modules use internally via their .lua scripts.

---

## DCS Command (on Button Press) Settings
These set what will be commanded to DCS when you physically press the StreamDeck button.

### Button ID
The Button ID can be found in the file: 

`Mods\aircraft\<AC_NAME>\Cockpit\clickabledata.lua`

For example, within AV8BNA the A/G Master Mode button has an id of `280` as seen from the following line:  

```
elements["PTN_280"] = default_button(_("AG Master Mode Selector"),
    devices.MSC, msc_commands.AG_Selector, 280)
```

### Device ID
The Device ID is a number representing different categorizations of clickable items in the cockpit. The definition of the number is defined in:  

`Mods\aircraft\<AC_NAME>\Cockpit\devices.lua`  

For the example of the A/G Master Mode button for AV8B above, the device is defined as `"MSC"`. You will need to then reference the `devices.lua` file to see that the Device ID number is `12` from this line:  

```
devices["MSC"]     = counter() -- 12
```  

*Note: The `"--"` indicate comments, the true value is defined by the number of `counter()` calls.*

### Send Value while Pressed
This value is sent for the Button ID when the Streamdeck button is pressed. For the example we have of our A/G Master Mode button we can see it is defined via the `default_button()` function.

If we now look inside the `clickable_defs.lua` file we can see that the function is defined with the following inputs:

```
function default_button(hint_,device_,command_,arg_,arg_val_,arg_lim_)

    local   arg_val_ = arg_val_ or 1
    local   arg_lim_ = arg_lim_ or {0,1}
```
The `arg_` input is what we already saw and named the Button ID, however this shows that when `arg_val_` and `arg_lim_` are not provided in the `default_button()` function they take on default values of `1` and `{0,1}`, respectively.

The `arg_val_` is what we care about here as that is the value that is sent when the button is pressed.

### Send Value while Released
This value is sent for the Button ID when the Streamdeck button is released.

From the example above we can see the button also has `arg_lim_={0,1}`. This is the range of values possible. Since this is simply an on/off button the range implies that 0 should be sent when the button is released.

### NOTE: More Complex Send Values

For buttons that are not simple on/off, but perhaps 3-way toggle or rotary dials, the `arg_val_` and `arg_lim_` become more important. Let's take a 2-way momentary switch as an example with the following define in `clickabledata.lua`:

```
elements["PTN_397"] = default_2_way_spring_switch(_("Fuzing Control"),
    devices.SMC, smc_commands.Switch_FUZE, 397)
```

This one again does not provide anything other than `arg_` (the Button ID) to the function. Looking at the definition for `default_2_way_spring_switch()` in `clickable_defs.lua` we see it returns (with `inversed` defaulted to `1.0`):

```
    arg 		= {arg_, arg_},
    arg_value	= {1.0*inversed, -1.0*inversed},
    arg_lim 	= {{0.0, 1.0}, {-1.0, 0.0}},
```

The fact that it has a set of two `arg_`'s means it actually has two buttons defined with the same Button ID of `397`. This means that if you want to send a command to press the Fuze switch up you would use the following settings:
```
Button ID=397, Device ID=29, SendPressed=1.0, SendReleased=0.0
```
And to send a command for Fuze switch down on a button press:
```
Button ID=397, Device ID=29, SendPressed=-1.0, SendReleased=0.0
```

---

## DCS Function Monitor (Change Button Image) Settings
![Change Button State](change_button_image.png "Change Button Image (State)")  

These settings will monitor the DCS game and change the image of the Streamdeck button when conditions are met.

### DCS ID

This field is set to the numeric value of the DCS function you would like to monitor. As this Streamdeck plugin relies on the **DCSExportScript** project for communication with the game, I recommend finding these values directly from their curated export scripts - [GitHub Link](https://github.com/s-d-a/DCS-ExportScripts/tree/master/Scripts/DCS-ExportScript/ExportsModules).

Continuing with our example of the AV8B A/G Mode Button, we can see the indicator lamp has a DCS ID of `281` from the following excerpt of **DCSExportScript**'s `AV8BNA.lua` file:

```
    --MIP OUTER LEFT SIDE
    [283] = "%.f", --NAV
    [285] = "%.f", --VSTOL
    [281] = "%.f", --A/G
```

### Show 2nd State Image When

This setting is a selection of how you would like to evaluate the received state of the function (`<`, `==`, or `>`). If the evaluation is `True`, then the state of the button will be set to the 2nd state (the illuminted button if looking at the above image).

Example: `DCS ID = 281, Show When = ">", Value = 0.0`  
For these settings the second state of the button will be activated (showing the second image) if the A/G Mode lamp function returns a value of `1.0`, and will reset to the first state if it drops back to `0.0`.

### Value

This is the value of the condition evaluated with either `<`, `==`, or `>`. This is generally a numeric value, but strings could also be evaluated.

*Text Strings* -- If you would like to identify if the DCS function value is "not empty" you could set the condition to greater than (`>`) and the Value=`""`. Any non-empty string value received will be evaluated as `True` if compared against an empty string.

---

## DCS Function Text Update Monitor

This section is to monitor the same type of DCS functions as the "DCS Function Monitor" section above, but instead of changing the Streamdeck button image it will set the button Title equal to the text received. Examples of where this would be useful is for Aircraft Scratchpads and UFC Text Displays.

### DCS ID

The only setting specified here is the function ID number which returns values you'd like to have displayed as text.

For example, with the `AV8BNA.lua` export script, setting a DCS ID equal to `250` would display the text for the ODU Option1 panel.

```
    -- ODU Panel
    [250] = "%1d", --ODU Option1
    [251] = "%1d", --ODU Option2
    [252] = "%1d", --ODU Option3
    [248] = "%1d", --ODU Option4
    [249] = "%1d", --ODU Option5
```

The font, size, and color of the text can be set with the Streamdeck Title settings.

**NOTE:** The "Title" field must be left empty in the Streamdeck settings for this to work.