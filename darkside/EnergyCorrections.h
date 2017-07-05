#pragma once

#include "Module.h"
#include "Event.h"
#include "Engine.h"

#include "TH2.h"
#include "TFile.h"

class EnergyCorrections : public Module {
public:
    Double_t s1_TBAcorr_factor(Double_t s1_top, Double_t s1_bottom, Double_t s1) {
        //TBAsymmetry correction
        Double_t par2[] = {-0.0397956, -0.27216, 0.794036, 1.70427, -3.98323, -8.50783, -2.66051};
        Double_t x = (s1_top-s1_bottom)/ s1;// TBAsym;
        Double_t diff_total_s1 = par2[0]+(par2[1]+(par2[2]+(par2[3]+(par2[4]+par2[5]*x)*x)*x)*x)*x; //(total_s1-total_s1_corr)/total_s1
        //   Double_t total_s1_TBAcorr =  s1*(1.-diff_total_s1);
        return (1.-diff_total_s1);
    }

    Double_t s1_corr_factor(Double_t t_drift_max, Double_t t_drift) {
        Double_t z = t_drift/(0.5*t_drift_max); // note normalization is to 0.5*t_drift_max
        // looked at Kr peaks in 15us t_drift windows (Run5330+5340), and
        // fit these to [0]*z^5 + [1]*z^4 + [2]*z^3+[3]*z^2+[4]*z+[5].
        Double_t fit_par0 = 0.0407;
        Double_t fit_par1 = -0.206;
        Double_t fit_par2 = 0.407;
        Double_t fit_par3 = -0.389;
        Double_t fit_par4 = 0.247;
        Double_t fit_par5 = 0.898;
        // normalizing all points on fitted curve to expected Kr peak at t_drift_max/2
        Double_t exp_Kr_peak_at_half_t_drift_max = fit_par0 + fit_par1 + fit_par2 + fit_par3 + fit_par4 + fit_par5;
        Double_t exp_Kr_peak_at_t_drift = fit_par0*z*z*z*z*z + fit_par1*z*z*z*z + fit_par2*z*z*z + fit_par3*z*z + fit_par4*z + fit_par5;
        return exp_Kr_peak_at_half_t_drift_max/exp_Kr_peak_at_t_drift; // s1 correction factor
    }

    TH2D* openHS2Corr() {
        TString fname = "darkside/S2CorrectionFactor_Kr_pS2VsXY_Kr_Jason_Masa.root";
        TFile *g = (TFile*)gROOT->GetListOfFiles()->FindObject(fname.Data());
        if (!g || !g->IsOpen()) {
            g = new TFile(fname.Data(), "READ");
            TString hname = "hS2KrVsXY_norm_center_Masa";
            fhS2Correction_factor = (TH2D*) g->Get(hname.Data());
            if(!fhS2Correction_factor) std::cout<<"profile:"<<hname.Data()<<" is not found."<<std::endl;
            fhS2Correction_factor->SetDirectory(0);
            g->Close();
            Engine::getInstance()->outputFile->cd();
            return fhS2Correction_factor;
        }
    }

    Double_t s2XYCorrection(Double_t x, Double_t y, TH2D* fhS2Correction_factor) {
        return 1./fhS2Correction_factor->GetBinContent(fhS2Correction_factor->FindBin(x,y) );
    }

    TH2D* fhS2Correction_factor;

    /*
    Calculates the S2 corrected values for an event
    This doesn't correct for saturation, we need to make a method to do that
    */
    double calculateS2Correction(Event &e, int pulse, int xyCorrection) {
        double correctedS2 = 0;
        double totalNPE = 0;
        
        float currentS2Correction = -1;
        switch(xyCorrection) {
            case 0:
                currentS2Correction=s2XYCorrection(e.pulse_x_masa[pulse], e.pulse_y_masa[pulse], fhS2Correction_factor);
                break;
            case 1:
                currentS2Correction=s2XYCorrection(e.pulse_x_jason[pulse], e.pulse_y_jason[pulse], fhS2Correction_factor);
                break;
            case 2:
                currentS2Correction=s2XYCorrection(e.pulse_x_andrew[pulse], e.pulse_y_andrew[pulse], fhS2Correction_factor);
                break;
            case -1:
                return -999;
        }
        return currentS2Correction;
    }

    void init() {
        openHS2Corr();
    }
    
    void processCorrections(Event& e) {
        double totalS2Corrected = 0;

        double rawTimeCorr = 0;
        for(int pulse = 0; pulse < e.npulses; pulse++) {
            if(e.pulse_f90[pulse] < 0.15) {
                // Correct S2 pulses
                float s2Corr = calculateS2Correction(e, pulse, 0);
                float pulseEnergyCorr = e.pulse_total_npe[pulse] * s2Corr;
                totalS2Corrected += pulseEnergyCorr;

                rawTimeCorr += (e.pulse_start_time[pulse]-e.pulse_start_time[0])*pulseEnergyCorr;
            }
        }

        double averageTime = 0;
        if(totalS2Corrected > 0) averageTime = (float)rawTimeCorr/(float)totalS2Corrected;

        double s1Corrected = (e.total_s1_top + e.total_s1_bottom)*s1_corr_factor(373.3, averageTime);

        e.correctedS1 = s1Corrected;
        e.correctedS2 = totalS2Corrected;
    }
    void cleanup() {}
};