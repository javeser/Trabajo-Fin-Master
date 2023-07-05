#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TROOT.h>

using namespace std;

void cut_all_plots()
{
    auto myTree = new TChain("Hlt2DetachedJpsiTuple/Tuple");
    myTree->Add("/Users/javiervelillaserna/Desktop/TFG/codigo.cpp/Lb2_JpsiLambda_vtx.root");

    std::vector<TString> names;
    std::vector<TString> filtered_names;
    std::vector<float> minCuts = {0.9996,0,0,0,0,0,0,0,0,0.9999,0,0,0,1000,0,0,-40,-40,0,0,0,0,0}; // Agrega los valores mínimos de los cortes para cada variable
    std::vector<float> maxCuts = {1,0.2,4,50,400000,30000,25,20000,1200,1,15,
        100,50,3000,15000,10,60,40,400,200000,15000,50000,4000}; // Agrega los valores máximos de los cortes para cada variable
    
    for (auto i : myTree->GetListOfBranches()[0]) {
        names.push_back(i->GetName());
    }

    for (size_t i = 0; i < names.size(); i++) {
        TString var = names[i];
        // Comprueba si la variable contiene las palabras "TRUE" o "MOTHER"
        if (var.EqualTo("Jpsi_PT") || var.EqualTo("Lambda0_END_VRHO") || var.EqualTo("Lambda0_BPVDIRA") || var.EqualTo("Lambda0_BPVIP") || var.EqualTo("Lambda0_BPVIPCHI2") || var.EqualTo("Lambda0_MAXSDOCA") || var.EqualTo("Lambda0_PT") || var.EqualTo("Lambda0_CHI2") || var.EqualTo("Lambda0_MASS") ||  var.EqualTo("Lb_BPVDIRA") || var.EqualTo("Lb_BPVIP") || var.EqualTo("Lb_BPVVDRHO") || var.EqualTo("Lb_MAXSDOCA") ||  var.EqualTo("Lb_P") || var.EqualTo("Lb_PT") || var.EqualTo("Lb_CHI2") || var.EqualTo("p_plus_PID_P") || var.EqualTo("p_plus_PID_K") || var.EqualTo("p_plus_MINIP") || var.EqualTo("p_plus_P")|| var.EqualTo("p_plus_PT")|| var.EqualTo("pi_minus_P")|| var.EqualTo("pi_minus_PT")) {
            // Si no contiene ninguna de las palabras, se añade la variable al vector.
            filtered_names.push_back(var);
            cout << "Comprobando variable: " << var << endl;
        }
    }

    float tam = filtered_names.size();

    cout << "El tamaño del vector de variables es de: " << tam << endl;

    const int rows = 4;
    const int cols = 3;

    // contadores del canvas
    int canv_count = 0;
    int hist_count = 0;

    // create the initial canvas
    auto c1 = new TCanvas(("c" + to_string(canv_count)).c_str(), ("c" + to_string(canv_count)).c_str(), 1600, 1200);
    c1->Divide(cols, rows);

    // loop para crear todos los histogramas
    for (size_t i = 0; i < filtered_names.size(); i++) {
        TString var = filtered_names[i];

        auto histoSignal = new TH1F(var + "h_signal", var, 100, minCuts[i], maxCuts[i]); // Establece los valores mínimos y máximos del corte para cada variable
        myTree->Project(var + "h_signal", var, "Lb_BKGCAT==0");
        auto histoBg = new TH1F(var + "h_bg", var, 100, minCuts[i], maxCuts[i]); // Establece los valores mínimos y máximos del corte para cada variable
        myTree->Project(var + "h_bg", var, "Lb_BKGCAT!=0");
        histoBg->SetLineColor(kRed);
        
        histoSignal->Scale(1. / histoSignal->Integral());
        histoBg->Scale(1. / histoBg->Integral());
        
        histoSignal->GetXaxis()->SetTitle(var);
        histoSignal->GetYaxis()->SetTitle("Entries");
        

        // create a new canvas every cols*rows histograms
        if (hist_count % (rows * cols) == 0) {
            canv_count++;
            auto c1 = new TCanvas(("c"+to_string(canv_count)).c_str(), ("c"+to_string(canv_count)).c_str(), 1600, 1200);
            c1->Divide(cols, rows);
        }
        

        // dibuja los hostograma en el canvas
        auto c1 =(TCanvas*)gROOT->GetListOfCanvases()->FindObject(("c"+to_string(canv_count)).c_str());
        c1->cd(hist_count % (rows * cols) + 1);
        histoBg->SetStats(false);
        histoSignal->SetStats(false);
        
        if (var.EqualTo("Lb_BPVDIRA") || var.EqualTo("Lambda0_BPVDIRA")|| var.EqualTo("Lambda0_BPVIPCHI2")|| var.EqualTo("Lambda0_CHI2"))
                {
                    auto c1 = (TCanvas *)gROOT->GetListOfCanvases()->FindObject(("c" + to_string(canv_count)).c_str());
                    c1->cd(hist_count % (rows * cols) + 1);
                    
                    gPad->SetLogx();
                    
                }
        
        histoSignal->Draw("HIST");
        histoBg->Draw("HIST,SAME");
        
        if (var.EqualTo("Lb_BPVVDRHO") || var.EqualTo("Lb_P")|| var.EqualTo("Lb_PT")|| var.EqualTo("Jpsi_PT")|| var.EqualTo("Lambda0_END_VRHO")|| var.EqualTo("Lambda0_PT")|| var.EqualTo("p_plus_PID_P")|| var.EqualTo("p_plus_P")|| var.EqualTo("p_plus_PT")|| var.EqualTo("pi_minus_P")|| var.EqualTo("pi_minus_PT"))
                {
                    histoBg->Draw("HIST");
                    histoSignal->Draw("HIST,SAME");
                    
                }

        
        hist_count++;

        // guarda el canvas tanto si esta lleno como si es el ultimo y no esta del todo lleno
        if (hist_count % (rows * cols) == 0 || hist_count == filtered_names.size()) {
            c1->Print(("all_plots_"+to_string(canv_count)+".pdf").c_str());
        }
    }
}
