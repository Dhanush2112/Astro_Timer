# Astronomical Timer – Features and Data Reference

## Core Function
- Dual-channel astronomical time-based switching device
- Automatic control of electrical loads based on calculated sunrise and sunset times
- Fully autonomous operation without external optical sensors

## Astronomical Calculation
- Sunrise and sunset times are calculated daily
- Calculations depend on:
  - Current date
  - Latitude
  - Longitude
  - Time zone
- Astronomical times are automatically adjusted throughout the year

## Astronomical Offset
- Global offset applied to sunrise and sunset times
- Offset range: ±120 minutes
- Offset value is applied equally to both channels
- Offset remains fixed until updated by the user

## Channel Switching Modes
Each channel independently supports:
- Sunset → ON, Sunrise → OFF
- Sunrise → ON, Sunset → OFF
- Astronomical switching disabled

## Time Programs
- Maximum number of programs: 100 (shared across both channels)
- Time resolution: 1 minute
- Supported program types:
  - Daily programs (weekday-based)
  - Yearly programs (specific calendar date)

## Combined Operation
- Astronomical switching and time programs can operate at the same time on a single channel
- Multiple switching conditions may apply to a channel simultaneously

## Operating Modes
- Automatic mode (program-controlled switching)
- Random switching mode
- Holiday (block) mode
- Manual override mode

## Random Mode
- Random switching interval range: 10–120 minutes
- Random mode replaces scheduled switching while active

## Holiday Mode
- User-defined start and end date
- Automatic switching suppressed during the holiday period

## Manual Mode
- Manual ON/OFF control per channel
- Manual state overrides all automatic switching

## Real-Time Clock
- Internal real-time clock maintains time and date
- RTC continues operation during power loss using backup battery
- Backup retention duration: up to 3 years
- Accuracy target: ±1 second per day at 23 °C

## Time Correction
- User-adjustable clock correction
- Adjustment range: ±12.6 seconds per day
- Adjustment resolution: 0.1 seconds per day

## Daylight Saving Time
- Automatic summer/winter time switching
- Switching behavior based on selected geographic region

## Location Configuration
- Location can be defined by:
  - Predefined country and city selection
  - Manual entry of latitude, longitude, and time zone
- Manual entry requires all parameters to be provided together

## Data Retention
- All user settings retained during power loss
- Normal operation resumes automatically when power is restored

## Display Indicators
- Individual channel status indication
- Mode indicators for:
  - Automatic operation
  - Offset active
  - Random mode
  - Holiday mode
  - Manual mode

## Physical Channels
- Two independent output channels
- Each channel supports independent configuration and operation
