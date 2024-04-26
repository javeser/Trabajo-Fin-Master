#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TROOT.h>

using namespace std;

void ploting_variables()
{
    auto myTree = new TChain("Hlt2DetachedJpsiTuple/Tuple");
    myTree->Add("/home/javeser/Desktop/Lb2_JpsiLambda_vtx.root");

    std::vector<TString> names;
    std::vector<TString> filtered_names;
    std::vector<float> minCuts = {0.9996,0,0,0,0,0,0,0,0,0.9999,0,0,1000,0,0,-40,0,0,0,0,0}; // Agrega los valores mínimos de los cortes para cada variable
    std::vector<float> maxCuts = {1,0.2,4,50,400000,30000,25,20000,1200,1,
        100,50,3000,15000,10,60,400,200000,15000,50000,4000}; // Agrega los valores máximos de los cortes para cada variable
    
    bool selec_var;
    bool cortes;
    bool corr;
    
    for (auto i : myTree->GetListOfBranches()[0]) {
        names.push_back(i->GetName());
    }
    
    cout << "Do you want to plot all de physic variables (no MOTHER o TRUE) or just the selected one? (all->1/selected->0)" << endl;
    cin >> selec_var;
    
    if(selec_var == true){
    
    	for (size_t i = 0; i < names.size(); i++){
        	TString var2 = names[i];
       		 // Comprueba si la variable contiene las palabras "TRUE" o "MOTHER"
       		 if(var2.Index("TRUE") == kNPOS && var2.Index("MOTHER") == kNPOS) {
          	  // Si contiene ninguna de las palabras, se añade la variable al vector.
            	filtered_names.push_back(var2);
            	cout << "Checking variables: " << var2 << endl;
        	}
        }
    }
    else
    {
    
    	for(auto& var: names){	
    		// Selecciona las variables deseadas
        	if (var.EqualTo("Jpsi_PT") || var.EqualTo("Lambda0_END_VRHO") || var.EqualTo("Lambda0_BPVDIRA") || var.EqualTo("Lambda0_BPVIP") ||  var.EqualTo("Lambda0_MAXSDOCA") || var.EqualTo("Lambda0_PT") || var.EqualTo("Lambda0_CHI2") || var.EqualTo("Lambda0_MASS") ||  var.EqualTo("Lb_BPVDIRA") || var.EqualTo("Lb_BPVIP") || var.EqualTo("Lb_BPVVDRHO") || var.EqualTo("Lb_MAXSDOCA") ||  var.EqualTo("Lb_P") || var.EqualTo("Lb_PT") || var.EqualTo("Lb_CHI2") || var.EqualTo("p_plus_PID_P") || var.EqualTo("p_plus_MINIP") || var.EqualTo("p_plus_P")|| var.EqualTo("p_plus_PT")|| var.EqualTo("pi_minus_P")|| var.EqualTo("pi_minus_PT")) {
            	// Si contiene ninguna de las palabras, se añade la variable al vector.
            	filtered_names.push_back(var);
            	cout << "Comprobando variable: " << var << endl;
        	}
    	
    	}
    	
    }
    
    if(selec_var == false){
    cout << "Do you want to apply cuts to the selected variables? (yes->1/no->0)" << endl; 
    cin >> cortes;
    }else{
    cortes = false; 
    }
    
    float tam = filtered_names.size();

    cout << "The variable vector size is: " << tam << endl;

    const int rows = 4;
    const int cols = 3;

    // contadores del canvas
    int canv_count = 0;
    int hist_count = 0;
    
    // create the initial canvas
    auto c1 = new TCanvas(("c" + to_string(canv_count)).c_str(), ("c" + to_string(canv_count)).c_str(), 1600, 1200);
    c1->Divide(cols, rows);

    if(cortes == false){
	
	// loop para crear todos los histogramas
   	for (size_t i = 0; i < filtered_names.size(); i++) {
        TString var = filtered_names[i];
        
        auto histoSignal = new TH1F(var + "h_signal", var, 100,  myTree->GetMinimum(var), myTree->GetMaximum(var) );
        
        myTree->Project(var + "h_signal", var, "Lb_BKGCAT==0");
        auto histoBg = new TH1F(var + "h_bg", var, 100,  myTree->GetMinimum(var), myTree->GetMaximum(var) );
        
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

        TPaveText *header = new TPaveText(0.1, 0.85, 0.9, 0.95, "NDC");
        header->SetFillColor(0);
        header->SetLineColor(0);
        header->SetTextAlign(22);
        header->SetTextSize(0.06); // Ajusta el tamaño del título de la cabecera
        header->AddText("Título de la Cabecera"); // Cambia el texto por el título deseado
        header->Draw();
        histoSignal->Draw("HIST");
        histoBg->Draw("HIST,SAME");
       
        
        hist_count++;

        // guarda el canvas tanto si esta lleno como si es el ultimo y no esta del todo lleno
        if (hist_count % (rows * cols) == 0 || hist_count == filtered_names.size()) {
            c1->Print(("all_plots_"+to_string(canv_count)+".pdf").c_str());
        }
        
    }
    }
    else
    {
    	// loop para crear todos los histogramas
   	for (size_t i = 0; i < filtered_names.size(); i++) {
        TString var = filtered_names[i];
        
        	auto histoSignal = new TH1F(var + "h_signal", var, 100,  minCuts[i], 	maxCuts[i] );
        
        	myTree->Project(var + "h_signal", var, "Lb_BKGCAT==0");
        	auto histoBg = new TH1F(var + "h_bg", var, 100, minCuts[i], maxCuts[i] );
        
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

        	TPaveText *header = new TPaveText(0.1, 0.85, 0.9, 0.95, "NDC");
        	header->SetFillColor(0);
        	header->SetLineColor(0);
        	header->SetTextAlign(22);
        	header->SetTextSize(0.06); // Ajusta el tamaño del título de la cabecera
        	header->AddText("Título de la Cabecera"); // Cambia el texto por el título deseado
        	header->Draw();
        	histoSignal->Draw("HIST");
        	histoBg->Draw("HIST,SAME");
       
        
        	hist_count++;

        	// guarda el canvas tanto si esta lleno como si es el ultimo y no esta del todo lleno
        	if (hist_count % (rows * cols) == 0 || hist_count == filtered_names.size()) {
            c1->Print(("all_plots_"+to_string(canv_count)+".pdf").c_str());
        	}
    	}
    
    }


    
}