void ThreeGaussFit(TH1* hist) {
    TF1 *fit = new TF1("fit", "[0]/(sqrt(2*TMath::Pi()))/[2]*exp(-0.5*(x-[1])*(x-[0])/[2]/[2])+[3]/(sqrt(2*TMath::Pi()))/[5]*exp(-0.5*(x-[4])*(x-[3])/[5]/[5])+[6]/(sqrt(2*TMath::Pi()))/[8]*exp(-0.5*(x-[7])*(x-[6])/[8]/[8])");
    fit->SetParameters(1, 0, 1, 1, 0, 1, 1, 0, 1);

    hist->Fit("fit");
    hist->Draw();
}