
# 420D User Guide

*A modern firmware enhancement for the Canon EOS 400D (XTi)*

`420D` is a free firmware enhancement software for the Canon EOS 400D, which offers additional functionality and capabilities to the camera, in both a non-destructive, and non-permanent way. It works by loading into your camera's memory automatically upon boot-up, to enhance the native camera firmware; but is not a permanent firmware upgrade, and can easily be removed.

_It is designed specifically for the Canon 400D/XTi and will not work with other camera models._

---

# Table of Contents

0. Welcome to 420D
1. Getting Started
2. Quick Start Guide
3. Shooting Enhancements
4. Advanced Using Guide
5. Automation (Scripts)
6. Custom Modes
7. Personalization
8. Configuration Files (.ini)
9. Reference

---

<p style="page-break-after: always;">&nbsp;</p>

# 0. Welcome to 420D

## What is 420D?

420D is a non-destructive firmware enhancement for the Canon EOS 400D. It extends the camera’s capabilities by loading into memory at startup, without permanently modifying the original firmware.

## Key Features

- Fv mode, as seen in recent Canon cameras
- Enhanced autofocus patterns (up to three patterns stored)
- Spot measurement (multi-measure supported)
- Named color temperatures (from an external file, unlimited)
- Extended ISO and exposure controls
- Advanced shooting modes (configurable delay, long exposure, intervalometer)
- Shooting without touching the camera (using the eye sensor)

## Safety

- No permanent firmware modification
- Can be disabled at boot
- Safe to remove at any time

---

<p style="page-break-after: always;">&nbsp;</p>

# 1. Getting Started

## Accessing 420D features

Press the **Direct Print (DP)** button to access most of the extended features of 420D.

| <!-- --> | <!-- --> |
|:--------------|:----------------|
| While in main 400D dialog | Open the 420D menu for extended configurations and settings |
| In metering mode selection dialog | Enable the spot metering |
| In WB selection dialog | Open the named color temperatures dialog |
| In ISO selection dialog | Enable AutoISO |
| In Drive selection dialog | Open the extended shooting modes dialog |

Half-press the shutter to exit a 420D dialog or menu.

Other features are activated with different button operations, and will be described later in this user guide.

## Menu and dialog navigation

- **UP/DOWN**: navigate items
- **ZOOM+/ZOOM-**: navigate items (page-down, page-up)
- **LEFT/RIGHT**: change values, enter sub-menu, sub-menus are indicated with a > sign on the right
- **SET**: confirm selection, execute the action if applicable; actions are indicated with a ! on the left
- **Front dial**: switch pages
- Hold **AV** / **Front dial**: display the page headers and select the desired page

---

<p style="page-break-after: always;">&nbsp;</p>

# 2. Quick Start Guide

## Select spot metering
- Open the metering mode dialog (**LEFT**)
- Press **DP** button to enable spot metering. Notice the metering mode icon showing a '.'

## Change ISO with finer control
`@TODO`
- Open menu → ISO
- Use LEFT/RIGHT to select intermediate values

## Use Auto ISO
- Enter the ISO selection dialog (**UP** button)
- Press **DP** button to enable AutoISO
- In the menu, configure Auto ISO by setting min/max ISO and shutter speed

## Select AF Pattern
- Press AF selection (**ZOOM+**) button twice
- Access pattern selection
- Choose pattern based on orientation

## Use Extended White balances
- Open the White balance selection dialog (**DOWN** button)
- Press **DP** to display the list of custom white balance
- Select the desired preset
- Adjust the temperature as needed with **LEFT** and **RIGHT** buttons
- Press **SET** to store the white balance value

## Launch various shooting automated actions
- Open the Drive selection dialog **Drive** button
- Press **DP** to display the list of possible automated shooting actions

---

<p style="page-break-after: always;">&nbsp;</p>

# 3. Shooting Enhancements

## Exposure

### Extended ISO
- Finer ISO steps (1/2, 1/4 or 1/8 EV)
- Better exposure tuning
- This feature is activated through the 420D menu: _Settings_ → _Digital ISO step_

It is to be noted that this is a pure software configuration, and does not change the gain of the sensor.

### AutoISO
- Automatically adjusts ISO
- Configurable limits
- Adapts to shooting mode (Av, Tv, M, P)
- This feature is activated either by:
  - selecting the ISO dialog and pressing **DP** button
  - going to the 420D menu and selecting _Parameters_ → _AutoISO_ → _Enable_
- The menu configuration allows clamping the min and max ISO values selected by AutoISO

### Exposure Compensation
- Extended beyond default limits (up to +/-6Ev)
- To select the extended value, go to menu _Parameters_ → _AV comp._

## Multi-metering mode
- Measure the exposure as many times as desired, the photo will be taken with an average exposure value.

## Autofocus

### AF Patterns

420D supports multiple autofocus (AF) patterns allowing zone focusing rather than only point focusing.
In order to activate the zone focus selection, press AF button twice. The detailed feature is described in "Advanced User Guide".


## White Balance

### Color Temperature (K)
- Direct Kelvin input set in the 420D menu _Parameters_ → _Color temp. (K)_.

### Named Temperatures
- Stored in external file
- No fixed limit
- Fully customizable
- Easier workflow than numeric values

## Flash

- Disable flash entirely
- Additional control options

---

<p style="page-break-after: always;">&nbsp;</p>

# 4. Advanced Using Guide

## Improvements in `P` mode

## Improvements in `Av` / `Tv` modes

## Improvements in `M` mode

## `Fv` mode

The original 400D firmware does not provide a `Fv` mode as in more recent Canon cameras. 420D adds this capability, making the Green Square (full auto) dial position work as a flexible exposure mode instead.

### Overview

`Fv` mode (**Flex Value** mode) lets you freely choose whether shutter speed (Tv) and aperture (Av) are controlled automatically or set to a fixed value. Based on your choice, 420D transparently selects the appropriate underlying exposure mode for you:

| Tv | Av | Effective mode |
|:---:|:---:|:---:|
| Auto | Auto | `P` |
| Fixed | Auto | `Tv` |
| Auto | Fixed | `Av` |
| Fixed | Fixed | `M` |

This way, you can stay on the Green Square position and move from fully automatic to partially manual or fully manual simply by adjusting individual parameters, without turning the dial.

### Enabling `Fv` mode

> **Note:** To use the Fv mode, go to menu entry  _Settings_ → _Use Fv mode_, set it to **Yes** to enable this feature; when set to **No** (the default), the Green Square position behaves as normal full auto.

### Using `Fv` mode

Once enabled, set the main dial to the **Green Square** (full auto) position; the camera will be automatically set to the last _actual_ mode used by Fv.
Press and hold the **JUMP** button to open the `Fv` shortcut and setting the desired `Tv` and `Av` values.

> **Note:** When `Fv` mode is enabled, the **JUMP** button is reserved for `Fv` shortcut access and cannot be assigned to another action.

While the `Fv` shortcut is active, the main display shows the name `Fv` along with the current value of the selected parameter. Use the following controls to adjust exposure:

| Button | Action |
|:-------|:-------|
| **AV** | Switch selection between Tv and Av |
| **UP** | Increase the selected value by one whole step |
| **DOWN** | Decrease the selected value by one whole step |
| **RIGHT** | Increase the selected value by 1/3 or 1/2 step, depending on the EV step configured in 400D |
| **LEFT** | Decrease the selected value by 1/3 or 1/2 step, depending on the EV step configured in 400D |
| **SET** | Reset the selected value to Auto |

- When a value is set to Auto it is displayed as `Tv:A` or `Av:A`.
- When you start from an Auto value and press **UP** or **DOWN**, the camera adopts the current metered value as the starting point before incrementing or decrementing.
- Releasing the **JUMP** button applies the selected values immediately and saves them, so the same `Fv` state is restored the next time you turn the camera on.

## Metering improvements

Several metering improvements are provided with 420D.

### Spot metering

The Spot metering mode is similar to the selective metering, but the zone included in the metering is smaller (about 3.5% in the center of the viewer,
compared to about 9% for the selective metering).

To activate the Spot metering, press the `METERING` button (**LEFT**); when the metering mode selection dialog is displayed,
press **DP**. Notice the icon [.] displayed in the main dialog.

### Multi-spot metering

Multi-spot metering allows you to take more than one metering for a scene, and take a photograph using the average exposure. It would be used for example in difficult
lighting situations.

Multi-spot metering only works in `P`, `Tv`, and `Av` modes. The "Use D-Pad" option must be active (set to "Yes") to use this feature.

To use multi-spot metering, follow this procedure:

  * Set the camera to one of `P`, `Tv`, `Av` modes.
  * Look through the viewfinder, and take a measurement by half-pressing the shutter button.
  * Still while looking through the viewfinder, press the **DOWN** button; you'll hear a beep, and `420D` will register that measurement.
  * Repeat this procedure to add as many readings as you need; after each reading is registered, the "burst counter" in the view finder will display the number of readings registered (notice that, while the counter has a upper limit of 9, `420D` does not have a limit on the number of readings you can register).
  * If you want to delete the last reading, just press the **DOWN** button and hold it for two seconds; you'll hear another beep confirming the delete.
  * Once you have taken enough readings, press the **SET** button (keep looking through the viewfinder); `420D` will set your camera to `M` mode, with the aperture and shutter speed set to the average of the apertures and shutter speeds from the readings registered.
  * Take a photograph, and `420D` will automatically revert back to the `P` / `Av` / `Tv` mode you where using.


<p style="page-break-after: always;">&nbsp;</p>

## AF improvements

This chapter describes in details the AF patterns the user can select. Up to three patterns are memorized; for example, one can use one pattern per
camera orientation. First select the AF pattern you are working with either through the menu or by assigning a shortcut.
Then, press twice the AF pattern selection button **ZOOM-**.


**Legend**:
- `[X]` = active AF point
- `[ ]` = inactive AF point

#### **LEFT**  and  **RIGHT**  buttons: change horizontal focus zone

```
        [ ]           |          [ ]           |          [X]           |
    [X]     [ ]       |      [X]     [ ]       |      [X]     [ ]       |
[X]     [ ]     [ ]   |  [X]     [X]     [ ]   |  [X]     [X]     [ ]   |
    [X]     [ ]       |      [X]     [ ]       |      [X]     [ ]       |
        [ ]           |          [ ]           |          [X]           |
```

```
        [X]           |
    [ ]     [ ]       |
[ ]     [X]     [ ]   |
    [ ]     [ ]       |
        [X]           |
```

```
        [X]           |          [ ]           |           [ ]
    [ ]     [X]       |      [ ]     [X]       |       [ ]     [X]
[ ]     [X]     [X]   |  [ ]     [X]     [X]   |   [ ]     [ ]     [X]
    [ ]     [X]       |      [ ]     [X]       |       [ ]     [X]
        [X]           |          [ ]           |           [ ]
```

<p style="page-break-after: always;">&nbsp;</p>

#### **UP**  and  **DOWN**  buttons: change vertical focus zone

```
        [X]           |          [X]           |          [X]           |
    [X]     [X]       |      [X]     [X]       |      [X]     [X]       |
[ ]     [ ]     [ ]   |  [ ]     [X]     [ ]   |  [X]     [X]     [X]   |
    [ ]     [ ]       |      [ ]     [ ]       |      [ ]     [ ]       |
        [ ]           |          [ ]           |          [ ]           |
```

```
        [ ]           |
    [ ]     [ ]       |
[X]     [X]     [X]   |
    [ ]     [ ]       |
        [ ]           |
```

```
        [ ]           |          [ ]           |          [ ]
    [ ]     [ ]       |      [ ]     [ ]       |      [ ]     [ ]
[X]     [X]     [X]   |  [ ]     [X]     [ ]   |  [ ]     [ ]     [ ]
    [X]     [X]       |      [X]     [X]       |      [X]     [X]
        [X]           |          [X]           |          [X]
```

#### **SET** button: toggle central zone focus or central point focus

```
        [ ]           |          [ ]           |          [ ]
    [ ]     [ ]       |      [X]     [X]       |      [ ]     [ ]
[ ]     [X]     [ ]   |  [ ]     [X]     [ ]   |  [ ]     [X]     [ ]
    [ ]     [ ]       |      [X]     [X]       |      [ ]     [ ]
        [ ]           |          [ ]           |          [ ]
```


<p style="page-break-after: always;">&nbsp;</p>

# 5. Shooting automation

## Overview

420D offers several advanced automated shooting tasks.

- Start:
   - press Shooting mode selection button
   - press **DP** to display the list of advanced options
   - select the desired shooting mode
   - press **LEFT** to change parameters of the task, **AV** to come back
   - press **SET** to start the shooting
- Stop: press **DP**
- Configure: Advanced shooting modes can be configured in the 420D menu:
   - add a 2s delay before shooting

## Common Use Cases

### Time-lapse
- Use dedicated timelapse automation, or intervalometer for advanced parameters

### HDR Photography
- Use Extended AEB

### Night Photography
- Use Long Exposure

### Sunset / Sunrise
- Use Bulb Ramping

---

<p style="page-break-after: always;">&nbsp;</p>

## Advanced Shooting Modes Reference

### Timelapse HD

The timelapse HD can be used to simplify the task of taking picture with the final goal to obtain a video, for
example with ffmpeg. The timelapse function targets a configuration as simple as possible. As the timelapse will shoot
a large number of pictures at a rapid frequency, the camera buffer may fill and prevent a smooth result.
For both a maximal simplicity and preventing the buffer to be filled, this mode forces pictures in JPEG and size 'S',
which is appropriate for a full HD final video.

To configure the timelapse, set the number of frames per second expected in the final video format (), the duration desired
for the final video (play time), and the duration of the shoot sequence (recording time).

For more configuration possibilities (RAW, other sizes, etc..), the intervalometer can be used with the timelapse
calculator.

#### Configuration ####

| <!-- --> | <!-- --> |
|:--------------|:----------------|
|Start             | Select how the shooting starts: immediately, wait for 2 seconds or use the eye-sensor.|
|Video format (fps)| Intended playback frame-rate, if preparing a time-lapse.|
|Recording time    | Estimated time that the camera will need to record the entire time-lapse (informative only).|
|Playback time     | Estimated playback time of the resulting time-lapse (informative only).|


### Intervalometer

The intervalometer will fire the camera as many times as configured, or endlessly, with a configurable delay between shots. It also includes a calculator
for video use case (stop motion movie).

#### Configuration ####

| <!-- --> | <!-- --> |
|:--------------|:----------------|
|Delay             | Select whether the first shot should be taken immediately or wait for 2 seconds.|
|Interval          | The interval time (in hours:minutes:seconds) between each shot (or group of shots); values range from 0:00:01 to 5:00:00.|
|Action            | Shooting task to launch at each timeout |
|Shots             | The total number of times the action configured above will be taken; values range from 1 to 9000, and also include a "No Limit" value to shoot endlessly.|
|Video format (fps)| Intended playback frame-rate, if preparing a time-lapse.|
|Recording time    | Estimated time that the camera will need to record the entire time-lapse (informative only).|
|Playback time     | Estimated playback time of the resulting time-lapse (informative only).|


#### Time-lapse calculator ####

You can use the time-lapse calculator to specify a desired reproduction frame-rate, and `420D` will calculate the resulting reproduction time. You can then use these values to configure the intervalometer.


---

<p style="page-break-after: always;">&nbsp;</p>

### Extended AEB
- Up to 9 frames
- Up to ±6 EV

### Bulb Ramping
- Gradual exposure changes

---

<p style="page-break-after: always;">&nbsp;</p>

### Long Exposure

This shooting mode allows taking a long exposure shot, witout using the Bulb mode and counting the time manually. The exposure duration can be set from 1 seconds to 5 hours.

#### Configuration ####

| <!-- --> | <!-- --> |
|:--------------|:----------------|
| Delay | Select whether this script should take the shot immediately or wait for 2 seconds.|
| Time  | The length (in minutes:seconds) of the exposure; values range from 00:00:01 to 05:00:00.|

When using this shooting mode, `420D` will automatically set the camera in M + BULB mode. You just need to set the Aperture value.

#### Long Exposure Calculator ####

The long exposure calculator allows setting the correct long exposure parameters in various use cases:

- if the light is not sufficient to use the camera measurement with the desired ISO and aperture settings (the exposure time would exceed 30s), you can temporarily measure a scene
    using a high ISO and a large aperture, then lower the ISO value and reduce the aperture and adjust the exposure time accordingly.

- if you're using a ND filter, you can measure the exposure time with the desired ISO and aperture settings, then install the ND filter. The Ev indicator will indicate an Ev result inverse
    to the value of the ND filter; you can then adjust the Tv value to raise the Ev indicator back to 0. Finally, just press "Apply" to configure the camera with these parameters,
    and launch the long exposure script.

_**Notice**_: The Ev field in this calculator cannot display values smaller than -15EV or larger than +15EV.

### Other Scripts
- ISO AEB: bracketing by ISO value
- Aperture AEB: bracketing by aperture value
- Self Timer: configurable self timer
- Hand trigger: trigger the shutter using the eye detection sensor



---

<p style="page-break-after: always;">&nbsp;</p>

# 6. Custom Modes

## What are Custom Modes?

Custom modes store full camera configurations.

## Creating a Mode

1. Set camera parameters
2. Save in Custom Modes menu

## Using a Mode

- Assign to a scene mode
- Switch via mode dial

## Notes

- Stored on CF card
- Fast switching between setups

---

<p style="page-break-after: always;">&nbsp;</p>

# 7. 420D Configuration

The 420D is configured in its own menu, accessed by pressing **DP**.

## Parameters

| <!--    -->       | <!--    --> |
| ----------------- | -------------------------------- |
| AutoISO           | [AutoISO]                        |
| ISO               | Select ISO by smaller increment (1/8Ev) |
| Av comp           | Select Exposure Compensation value by configurable increment |
| AEB               | Select Exposure Bracketing value by configurable increment   |
| Color Temperature | Select color temperature in K    |
| Mirror Lockup     | Toggle mirror lockup |
| Safety Shift      | Toggle safety shift functionality  |
| IR remote enable  | Enable/Disable Infrared remote   |
| Flash Config      | [Flash config]                   |

### AutoISO

| <!--  --> | <!--  --> |
| ----------------- | -------------------------------- |
|Enabled|When active (set to "Yes"), AutoISO is enabled.|
|Min ISO|Minimum ISO value that AutoISO will use.|
|Max ISO|Maximum ISO value that AutoISO will use.|
|Min Tv|Minimum shutter speed that AutoISO will try to maintain.|
|Max Av|Maximum aperture (relative to maximum aperture supported by the lens) that AutoISO will try to maintain.|
|Relaxed|When active (set to "Yes"), AutoISO in M mode will allow larger exposure deviations.|

### Flash

| <!--  --> | <!--  --> |
| ----------------- | -------------------------------- |
| Flash Comp. | Extended Flash Compensation, up to +/-6Ev |
| Disable flash | Prevent flash from firing |
| AF Flash | Prevent AF assist from firing from flash |
| Flash 2nd curtain | Toggle Flash 2nd curtain |

## Scripts

This menu allows the configuration of advanced shooting modes.

| <!--  --> | <!--  -->  |
| --------- | ---------- |
| Extended AEB | Extended Auto exposure bracketing |
| Flash AEB    | Extended flash auto exposure bracketing |
| Aperture AEB | Exposure bracketing on aperture |
| ISO AEB      | Exposure bracketing on ISO value |
| Bulb ramping | Series of Bulb exposures with incrementing time values |

### Extended AEB

Extension of bracketing feature: set an unlimited of frames, and an Ev step up to +/- 6Ev.

| <!--  --> | <!--  -->  |
| --------- | ---------- |
| 2s Delay  | Wait 2s before first frame |
| Frames    | Number of frames of bracketing |
| Step (EV) | Ev Step between frames |
| Direction | Increment, Decrement or Alternate Ev values (+, -, +/-) |
| Bulb min  |The minimum shutter speed in `BULB` mode. Values range from 1/4000th of a second to 32 minutes.|
| Bulb max  |The maximum shutter speed in `BULB` mode. Values range from 1/4000th of a second to 32 minutes.|

### Flash AEB

Extension of flash bracketing feature: set an unlimited of frames, and an Ev step up to +/- 6Ev.

| <!--  --> | <!--  --> |
| --------- | ---------- |
|2s Delay  | Wait 2s before first frame |
|Frames    | Number of frames of bracketing |
|Step (EV) | Ev Step between frames |
|Direction | Increment, Decrement or Alternate Ev values (+, -, +/-) |

### Aperture AEB

Extension of bracketing feature: use aperture only for bracketing.

| <!--  --> | <!--  --> |
| --------- | ---------- |
|2s Delay  | Wait 2s before first frame |
|Frames    | Number of frames of bracketing |
|Step (EV) | Ev Step between frames |
|Direction | Increment, Decrement or Alternate Ev values (+, -, +/-) |

### ISO AEB

Extension of bracketing feature: take one frame at each selected ISO value.

| <!--  --> | <!--  --> |
| --------- | ---------- |
|2s Delay  | Wait 2s before first frame |
|100    | Shoot at 100 ISO |
|200    | Shoot at 200 ISO |
|400    | Shoot at 400 ISO |
|800    | Shoot at 800 ISO |
|1600   | Shoot at 1600 ISO |

### Bulb ramping

`@TODO`

The bulb ramping script is similar to the intervalometer, but with the added peculiarities that it is used for long exposures (1s and longer) only, and that `400plus` will gradually change the exposure time and / or the duration of the interval between shots. Bulb ramping is used mostly to make time-lapses in situations when the amount of light is expected to change, such as sunsets.

| <!--  --> | <!--  -->  |
| --------- | ---------- |
|2s Delay           | Wait 2s before first frame. |
|Shots              | The total number of frames (shots) the script will take; values range from 1 to 9000, and also include a "No Limit" value to shoot endlessly. |
|Interval           | The initial interval time (in hours:minutes:seconds) between each shot; values range from 0:00:01 to 5:00:00. |
|Exposure           | The length (in minutes:seconds) of the first exposure; values range from 00:00:01 to 05:00:00. |
|Ramp size (time)   | The length (in minutes:seconds) of the ramp; values range from 00:00:01 to 05:00:00.|
|Ramp size (shots)  | The length (in number of shots) of the ramp; values range from 0 to 9000.|
|Ramping (interval) | Ev stops between each frame (+/-6).|
|Ramping (exposure) | The strength (in EV stops in range +/- 6Ev) of the ramp, as applied to the exposure time.|

## Info

| <!--  -->         | <!--  --> |
| ----------------- | -------------------------------- |
|Version            |The version number of the `420D` software currently installed.
|Release count      |The total number of shots (actuations) made by this camera body.
|Body ID            |The "serial number" of the camera body (an internal number, unique to each camera).
|Firmware           |The firmware version (should be 1.1.1 for `420D` to be working).
|Owner              |The owner of this camera (use Canon software to change it).

## Settings

| <!--  --> | <!--  --> |
| ----------------- | -------------------------------- |
|Language|[Internationalization]
|Digital ISO step|[Configuring Extended ISOs]
|Persist AEB|[Persisting AEB]
|Config. Scripts|[Configuring Scripts]
|Config. Buttons|[Configuring Buttons]
|Config. Custom modes|[Configuring Custom Modes]
|Config. Menus|[Configuring Menus]
|Config. Quick exposure|[Configuring Quick Exposure]
|Config. Pages|[Configuring Pages]
|Restore config.|[Restore Configuration]
|Developers menu|[Developers Page]

### Config. Scripts

| <!--  --> | <!--  -->  |
| --------- | ---------- |
|Disable power-off|  |
|Disable review|  |
|LCD display|  |
|Indicator|  |

### Config.Buttons

| <!--  --> | <!--  -->  |
| --------- | ---------- |
|Use D-Pad|  |
|Better DISP button|  |
|Jump|  |
|Trash|  |

### Config. Custom modes

| <!--  --> | <!--  -->  |
| --------- | ---------- |
|Camera|  |
|420D|  |
|Menu order|  |
|Settings|  |
|Image|  |

### Config. Menus

| <!--  --> | <!--  -->  |
| --------- | ---------- |
|Menus wrap       |  |
|Navigate to main |  |
|Enter to main    |  |
|Autosave         |  |

### Config. Quick Exposure

| <!--  --> | <!--  -->  |
| --------- | ---------- |
|Min Tv|  |
|Weight|  |

### Config Pages

Allows ordering the main menu pages

## Custom modes

## Developers



---

<p style="page-break-after: always;">&nbsp;</p>

# 8. Advanced Configuration
---

<p style="page-break-after: always;">&nbsp;</p>

# 9. Personalization

## Menu Customization

- Reorder items
- Simplify navigation

## Button Customization

Assign actions to buttons:

- ISO adjustment
- Script repeat
- Flash toggle

## Display Options

- Display luminosity can be adjusted by increments
- Feedback indicators

---

<p style="page-break-after: always;">&nbsp;</p>

# 9. Configuration Files (.ini)

420D uses external configuration files for flexibility.

## Benefits

- Editable without recompilation
- Human-readable format
- Future-proof design

## Named Temperatures File

- Define custom presets
- Add unlimited entries

Example:

```
[temps]
Daylight=5500
Tungsten=3200
Custom=4800
```

## General Configuration

- Camera behavior
- Feature settings
- User preferences

---

<p style="page-break-after: always;">&nbsp;</p>

# 10. Technical Reference

This chapter goes into technical details of some functions provided by 420D.

## Intervalometer

### Strict scheduling ###

In Intervalometer, `420D` will always try to maintain a constant cadence of shots, independent of the exposure time: this means that, if configured to shot at 15s intervals, for example, then one shot will be taken exactly every 15s, even if the exposure is 10s.
If one shot in the sequence has an exposure time longer than the configured interval time, `420D` will skip as many shots as needed and reschedule the sequence accordingly. This is also true when the intervalometer is configured to fire any multi-shot action.

For example, imagine an interval of 5 seconds, and exposures ranging between 1 and 4 seconds (let's suppose camera is in Av mode, and light conditions change during the timelapse):

```
Time (s) : # · · · · # · · · · # · · · · # · · · ·
Exposure : * *       * * * *   *         * *
```

Notice how the length of the pause between the end of an exposure and the beginning of the next one adapts to the exposure time, so photographs always start at regular intervals.

In the case that one of the exposures takes longer than 5 seconds, one shot will be skipped:

```
Time (s) : # · · · · # · · · · # · · · · # · · · ·
Exposure : * *       * * * * * * *       * *
```

Notice that the third photograph does not take place 5 seconds after the second one, but at second 15.

<p style="page-break-after: always;">&nbsp;</p>

# 11. Reference

## Limitations

- Some values displayed by camera may differ
- Hardware constraints apply

## Troubleshooting

- Disable 420D at boot if needed
- Reset configuration

## Compatibility

- Designed for Canon EOS 400D only

---

# Philosophy

- Keep it lightweight
- Enhance, don’t replace
- Prioritize usability
- Enable advanced workflows without complexity

