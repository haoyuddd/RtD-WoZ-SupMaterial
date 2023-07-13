`RtD-WoZ-SupMaterial`

# On-road exploration of interaction design in automated vehicles to reduce automation surprise using WoZ vehicles
`PhD-Study1`
`Period: 2021.9 - 2021.12`

The present repository includes some of the supplementary materials of the paper:

Dong, H., Wang, Y., Bazilinskyy, P., Martens, M. (2023). On-road exploration of non-intrusive interaction design to reduce automation surprise in non-critical automated driving scenarios. Submitted for review.

The present Github repository is associated with the [4TU.ResearchData repository](#). 

This dataset includes source code used for data acquisition and analysis, and anonymized experiment data.
Whereas the remaining supplementary materials (including the data used in the study) are available from OSF.

Below, we list and describe the full set of supplementary materials. 

## Source Code
- Data logger code
- Fake steering wheel code

## Experimental material
- Intake questionnaire
- Consent form
- Voice notification
- NDRAs - video link (tbd❓)

## Data
### Sensor data
- Name:
  - IMU-001 = with cue
  - IMU-000 = no cue

- Column
  - GPS (1Hz)
	- Real-time-clock (absolute time)
	- h = hour
	- m = minute
	- s = second
	- Lat = latitude: GPS location
	- Lon = longitude: GPS location
	- Speed (knots) = 1.852 km/h
	- Angle
	- Altitude
  - Accelerometer (100hz) ms
	- x = lateral movement (+ left, - right)
	- y = longitudinal movement
	- (Calibration problem: <0.1 deceleration, >0.1 acceleration)
  - cue
  	- 1 (no cue)
	- 0 (with cue)

### Pre-questionnaire
- The questionnaire
- The raw data from the questionnaire

### Video recording analysis
- Outwards: front view
- Inwards: participants' behavior
- Main (BORIS file)
- Coding Criteria
- ICR-Kvalue

The video data was analyzed in [BORIS](https://doi.org/10.1111/2041-210X.12584)

- Main (BORIS file) includes both coders
	- first coder: all videos
	- second coder: 25% (5 participants)

Calculate inter-rater reliability (Cohen’s Kappa) in BORIS
  1.	Open project Main.boris in BORIS.
  2.	Click ‘Analysis- Inter-rater reliability- Cohen’s Kappa(time-unit)’ on the toolbar.
  3.	Select the two observations you would like to compare in the popping-up window and click OK.
  4.	Select the subjects and behaviors you would like to compare in the window popping up. In all our calculations, only the behavior ‘End of the trial’ is excluded.
  5.	Then the time-unit window will pop up. Keep the time unit with 1.000 and click OK.
  6.	Then the result would show in the new window.
	
How to export video annotation excel?
  1.	Open project Main.boris in BORIS.
  2.	Click ‘Observations’ on the toolbar. Choose ‘Export events-Aggregated events’.
  3.	Select the observations you would like to save, and save as csv file.

### Interview recording
- Transcripts in MAXQDA

The interview was analyzed in [MAXQDA](https://www.maxqda.com/)
