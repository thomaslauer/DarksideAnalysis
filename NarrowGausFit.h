void NarrowGausFit(TH1* hist) {
    TF1 *fit = new TF1("fit", "gaus", -1, 1);
    // fit->SetParameters(1, 0, 1);

    hist->Fit(fit, "R");
    hist->Draw();
}