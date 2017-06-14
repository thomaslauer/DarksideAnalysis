#pragma once

struct Event {
	// variables from main file
	int    run_id;
	int    event_id;
	int    nchannels;
	short  baseline_not_found;
	double live_time;
	double inhibit_time;
	int    npulses;
	int    has_s3;
	float  s1_start_time;
	float  s1;
	float  s2;
	double tdrift;
	short  hasV1724;
	
	float total_f90;
	
	float total_s1_top;
	float total_s1_bottom;

	// variables from xy file
	float  x;
	float  y;

	// variables from allpulses file
	float pulse_start_time[100];
	float pulse_end_time[100];
	float pulse_total_npe[100];
	float pulse_satCorS1[100];
	float pulse_satCorS2[100];
	int pulse_saturated[100];
	
	// from masas file
	float pulse_x_masa[100];
	float pulse_y_masa[100];
	float pulse_r_masa[100];
	float pulse_chi2_masa[100];
	
	float pulse_x_jason[100];
	float pulse_y_jason[100];
	float pulse_r_jason[100];
	float pulse_chi2_jason[100];
	
	float pulse_x_andrew[100];
	float pulse_y_andrew[100];
	float pulse_r_andrew[100];
	float pulse_chi2_andrew[100];
	
	// from pulses file
	float pulse_f90[100];
	
	// from vetoes file
	float veto_lsv_charge;
	float veto_wt_charge;
};