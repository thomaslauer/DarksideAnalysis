#pragma once

#include <string>
#include <vector>

#include "Event.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

class SladLoader {
public:
    std::vector<TString> sladFiles; // names of files
    std::vector<TString> sladTrees; // names of trees in those files

    TString mainFileName;

    SladLoader(TString mainFile) {
        mainFileName = mainFile;
    }

    void addSladFile(TString filename, TString treeName) {
        TString thisFileName = mainFileName;
        thisFileName.Remove(thisFileName.Length()-5);
        thisFileName+=filename;
        sladFiles.push_back(thisFileName);
        sladTrees.push_back(treeName);
    }

    TTree* getFullTree() {
        TFile* main = new TFile(mainFileName);
        TTree* events = (TTree*) main->Get("events");

        for(unsigned int i = 0; i < sladFiles.size(); i++) {
            TFile* currentFile = TFile::Open(sladFiles[i]);
            TTree* currentTree = (TTree*) currentFile->Get(sladTrees[i]);
            events->AddFriend(currentTree);
        }

        return events;
    }

    void linkTree(TTree* events, Event& e) {
        events->SetBranchStatus("*",0); //disable all
        
        events->SetBranchStatus("run_id", 1);
        events->SetBranchAddress("run_id", &e.run_id);

        events->SetBranchStatus("event_id", 1);
        events->SetBranchAddress("event_id", &e.event_id);
        
        events->SetBranchStatus("hasV1724", 1);
        events->SetBranchAddress("hasV1724", &e.hasV1724);
        
        events->SetBranchStatus("nchannel", 1);
        events->SetBranchAddress("nchannel", &e.nchannels);

        events->SetBranchStatus("SumChannelHasNoBaseline",1);
        events->SetBranchAddress("SumChannelHasNoBaseline", &e.baseline_not_found);

        events->SetBranchStatus("lifetime",1);
        events->SetBranchAddress("lifetime", &e.live_time);

        events->SetBranchStatus("inhibittime",1);
        events->SetBranchAddress("inhibittime", &e.inhibit_time);

        events->SetBranchStatus("n_phys_pulses",1);
        events->SetBranchAddress("n_phys_pulses", &e.npulses);

        events->SetBranchStatus("has_s3",1);
        events->SetBranchAddress("has_s3", &e.has_s3);

        events->SetBranchStatus("s1_start_time", 1);
        events->SetBranchAddress("s1_start_time", &e.s1_start_time);

        events->SetBranchStatus("total_s1_corr", 1);
        events->SetBranchAddress("total_s1_corr", &e.s1);

        events->SetBranchStatus("total_s2_corr", 1);
        events->SetBranchAddress("total_s2_corr", &e.s2);

        events->SetBranchStatus("pulse_info_start_time", 1);
        events->SetBranchAddress("pulse_info_start_time", e.pulse_start_time);

        events->SetBranchStatus("pulse_info_end_time", 1);
        events->SetBranchAddress("pulse_info_end_time", e.pulse_end_time);
    
        events->SetBranchStatus("tdrift.tdrift",1);
        events->SetBranchAddress("tdrift.tdrift", &e.tdrift);
        
        events->SetBranchStatus("allpulses_xy.allpulses_x", 1);
        events->SetBranchAddress("allpulses_xy.allpulses_x", e.pulse_x_masa);
        
        events->SetBranchStatus("allpulses_xy.allpulses_y", 1);
        events->SetBranchAddress("allpulses_xy.allpulses_y", e.pulse_y_masa);
        
        events->SetBranchStatus("allpulses_xy.allpulses_r", 1);
        events->SetBranchAddress("allpulses_xy.allpulses_r", e.pulse_r_masa);
        
        events->SetBranchStatus("allpulses_xy.allpulses_chi2", 1);
        events->SetBranchAddress("allpulses_xy.allpulses_chi2", e.pulse_chi2_masa);
        
        events->SetBranchStatus("allpulses_xyl_xy.allpulses_xyl_x", 1);
        events->SetBranchAddress("allpulses_xyl_xy.allpulses_xyl_x", e.pulse_x_jason);
        
        events->SetBranchStatus("allpulses_xyl_xy.allpulses_xyl_y", 1);
        events->SetBranchAddress("allpulses_xyl_xy.allpulses_xyl_y", e.pulse_y_jason);
        
        events->SetBranchStatus("allpulses_xyl_xy.allpulses_xyl_r", 1);
        events->SetBranchAddress("allpulses_xyl_xy.allpulses_xyl_r", e.pulse_r_jason);
        
        events->SetBranchStatus("allpulses_xyl_xy.allpulses_xyl_chi2", 1);
        events->SetBranchAddress("allpulses_xyl_xy.allpulses_xyl_chi2", e.pulse_chi2_jason);
        
        events->SetBranchStatus("allpulses_aww_xy.allpulses_aww_x", 1);
        events->SetBranchAddress("allpulses_aww_xy.allpulses_aww_x", e.pulse_x_andrew);
        
        events->SetBranchStatus("allpulses_aww_xy.allpulses_aww_y", 1);
        events->SetBranchAddress("allpulses_aww_xy.allpulses_aww_y", e.pulse_y_andrew);
        
        events->SetBranchStatus("allpulses_aww_xy.allpulses_aww_r", 1);
        events->SetBranchAddress("allpulses_aww_xy.allpulses_aww_r", e.pulse_r_andrew);
        
        events->SetBranchStatus("pulse_info.pulse_info_f90", 1);
        events->SetBranchAddress("pulse_info.pulse_info_f90", e.pulse_f90);
        
        events->SetBranchStatus("pulse_info.pulse_info_total_npe", 1);
        events->SetBranchAddress("pulse_info.pulse_info_total_npe", e.pulse_total_npe);
        
        events->SetBranchStatus("pulse_info.pulse_info_satcorr_fixed_int1", 1);
        events->SetBranchAddress("pulse_info.pulse_info_satcorr_fixed_int1", e.pulse_satCorS1);
        
        events->SetBranchStatus("pulse_info.pulse_info_satcorr_fixed_int2", 1);
        events->SetBranchAddress("pulse_info.pulse_info_satcorr_fixed_int2", e.pulse_satCorS2);
        
        events->SetBranchStatus("pulse_info.pulse_info_saturated", 1);
        events->SetBranchAddress("pulse_info.pulse_info_saturated", e.pulse_saturated);
        
        events->SetBranchStatus("s1.total_s1_top",1);
        events->SetBranchAddress("s1.total_s1_top", &e.total_s1_top);

        events->SetBranchStatus("s1.total_s1_bottom",1);
        events->SetBranchAddress("s1.total_s1_bottom",&e.total_s1_bottom);
        
        events->SetBranchStatus("s1_f90.total_f90",1);
        events->SetBranchAddress("s1_f90.total_f90",&e.total_f90);
        
        events->SetBranchStatus("veto.veto_lsv_total_charge", 1);
        events->SetBranchAddress("veto.veto_lsv_total_charge", &e.veto_lsv_charge);
        
        events->SetBranchStatus("veto.veto_wt_total_charge", 1);
        events->SetBranchAddress("veto.veto_wt_total_charge", &e.veto_wt_charge);
    }
};